#include "unity.h"
#include <string.h>
#include <stdio.h>
#include "line.h"

/*-----------------------------------------------------------------------------
 *  Helper functions and variables.
 *-----------------------------------------------------------------------------*/
Line_CharRxCallback MockUART_ChrRxCallback;

uint8_t chr_rx_not_attached;

/* The following function helps on emulation of a UART peripheral that sends/ */
/* receives characters and notifies the Line module via callbacks. */
void MockUART_SendChar(uint8_t ch) {
  uint8_t buff = ch;
  MockUART_ChrRxCallback((void*)&buff);
}

/* The following function helps on emulation of a UART peripheral that sends/ */
/* receives characters and notifies the Line module via callbacks. */
void MockUART_SendCharArray(char* chrarr, uint8_t sz) {
  uint8_t idx = 0;
  for(; idx < sz; idx++) {
    MockUART_SendChar((uint8_t)chrarr[idx]);
  }
}

uint8_t test_buff[LINE_BUFF_SIZE] = {0};

void helper_LINE_fill_buff(uint8_t val) {
  uint8_t idx = 0;
  for(; idx < LINE_BUFF_SIZE - 1; idx++) {
    MockUART_SendChar(val);
  }
  /* Last char must be EOL character or buffer will overflow and flush. */
  MockUART_SendChar(LINE_CHAR_CR);
}

void chr_rx_callback_not_attached(void* params) {
  chr_rx_not_attached = *((uint8_t*)params);
}

void dummy_callback(void* arg) {
  arg = arg;
}

/*-----------------------------------------------------------------------------
 *  Tests for Line module.
 *-----------------------------------------------------------------------------*/
void test_Line_Init_function(void) {
  chr_rx_not_attached = 0xff;
  MockUART_ChrRxCallback = chr_rx_callback_not_attached;
  MockUART_SendChar('a'); 
  TEST_ASSERT_EQUAL_UINT8('a', chr_rx_not_attached);
  Line_Init(&MockUART_ChrRxCallback);
  /* Check buffer is empty. */
  TEST_ASSERT_TRUE(Line_BuffIsEmpty());
  /* Check count is zero. */
  TEST_ASSERT_EQUAL_UINT8(0, Line_GetCnt());
  /* Check buffer is not full. */
  TEST_ASSERT_FALSE(Line_BuffIsFull());
  /* Check buffer is initialized to zero. */
  /* Set a non-zero value prior to getting buffer to verify */
  /* it was, indeed, set to zero. */
  test_buff[0] = 'a';
  Line_GetBuff(test_buff);
  TEST_ASSERT_EACH_EQUAL_UINT8(0, test_buff, sizeof(test_buff));
  MockUART_SendChar('b');
  /* Check value is not modified now that callback is attached. */
  TEST_ASSERT_EQUAL_UINT8('a', chr_rx_not_attached);
  /* Make sure buffer is flushed on init. */
  Line_Init(&MockUART_ChrRxCallback);
  TEST_ASSERT_EQUAL_UINT8(0, Line_GetCnt());
  Line_GetBuff(test_buff);
  TEST_ASSERT_EACH_EQUAL_UINT8(0, test_buff, sizeof(test_buff));
  TEST_ASSERT_TRUE(Line_BuffIsEmpty());
  TEST_ASSERT_FALSE(Line_IsCmplt());
}

void test_Line_NewCharCallback_add_chars(void) {
  Line_Init(&MockUART_ChrRxCallback);
  /* Check character count is zero. */
  TEST_ASSERT_EQUAL_UINT8(0, Line_GetCnt());
  MockUART_SendChar('a');
  /* Check character count increases. */
  TEST_ASSERT_EQUAL_UINT8(1, Line_GetCnt());
  /* Check first element is character 'a'. */
  Line_GetBuff(test_buff);
  TEST_ASSERT_EQUAL_UINT8('a', test_buff[0]);
  /* Add another char. Check count increases. */
  MockUART_SendChar('b');
  Line_GetBuff(test_buff);
  TEST_ASSERT_EQUAL_UINT8('a', test_buff[0]);
  TEST_ASSERT_EQUAL_UINT8('b', test_buff[1]);
  TEST_ASSERT_EQUAL_UINT8(2, Line_GetCnt());
}

void test_Line_BufferIsFull_flag(void) {
  uint8_t idx = 0;
  Line_Init(&MockUART_ChrRxCallback);
  for(; idx < LINE_BUFF_SIZE - 1; idx++) {
    test_buff[idx] = idx;
    TEST_ASSERT_FALSE(Line_BuffIsFull());
    MockUART_SendChar(idx);
  }
  /* Last character must be EOL char or buffer will automatically flush. */
  MockUART_SendChar(LINE_CHAR_LF);
  /* Check full flag is set after filling the buffer. */
  TEST_ASSERT_TRUE(Line_BuffIsFull());
  /* Check count does not increment because of buffer full. */
  idx = Line_GetCnt();
  MockUART_SendChar('a');
  TEST_ASSERT_EQUAL_UINT8(idx, Line_GetCnt());
}

void test_Line_new_ch_proc_ignore_eol_on_empty_buf(void) {
  Line_Init(&MockUART_ChrRxCallback);
  TEST_ASSERT_TRUE(Line_BuffIsEmpty());
  MockUART_SendChar(LINE_CHAR_LF);
  /* Buffer shall remain upmty on LF character. */
  TEST_ASSERT_TRUE(Line_BuffIsEmpty());
  MockUART_SendChar(LINE_CHAR_CR);
  /* Buffer shall remain upmty on CR character. */
  TEST_ASSERT_TRUE(Line_BuffIsEmpty());
  /* Buffer shall remain upmty on SPACE character. */
  MockUART_SendChar(LINE_CHAR_SPACE);
  TEST_ASSERT_TRUE(Line_BuffIsEmpty());
  /* Now send a valid initial character followed by LF or CR. */
  MockUART_SendChar('a');
  TEST_ASSERT_EQUAL_UINT8(1, Line_GetCnt());
  MockUART_SendChar(LINE_CHAR_CR);
  /* Verify it was accepted. */
  TEST_ASSERT_EQUAL_UINT8(2, Line_GetCnt());
}

void test_Line_FlushBuff(void) {
  memset((void*)test_buff, 0, sizeof(test_buff));
  Line_Init(&MockUART_ChrRxCallback);
  TEST_ASSERT_TRUE(Line_BuffIsEmpty());
  /* Fill buffer with 'a' values. */
  helper_LINE_fill_buff('a');
  Line_GetBuff(test_buff);
  /* Make sure all buffer is filled with correct values. */
  /* The last character must always be an eol character when filling the buffer. */
  /* It will be replaced by a null character so that it is a valid C-string. */
  TEST_ASSERT_EACH_EQUAL_UINT8('a', test_buff, sizeof(test_buff - 1));
  TEST_ASSERT_EQUAL_UINT8(0, test_buff[LINE_BUFF_SIZE - 1]);
  /* Now flush it and make sure it is all zeros after. */
  Line_FlushBuff();
  Line_GetBuff(test_buff);
  TEST_ASSERT_EACH_EQUAL_UINT8(0, test_buff, sizeof(test_buff));
  /* Make sure count goes to zero after a flush. */
  TEST_ASSERT_EQUAL_UINT8(0, Line_GetCnt());
}

void test_Line_IsCmplt(void) {
  Line_Init(&MockUART_ChrRxCallback);
  TEST_ASSERT_TRUE(Line_BuffIsEmpty());
  /* Make sure LF/CR are ignored on buffer empty and do not cause */
  /* complete flag to be set. */
  TEST_ASSERT_FALSE(Line_IsCmplt());
  MockUART_SendChar(LINE_CHAR_LF);
  TEST_ASSERT_FALSE(Line_IsCmplt());
  MockUART_SendChar(LINE_CHAR_CR);
  TEST_ASSERT_FALSE(Line_IsCmplt());
  /* Set minimal command. */
  MockUART_SendChar('a');
  MockUART_SendChar(LINE_CHAR_CR);
  TEST_ASSERT_TRUE(Line_IsCmplt());
  Line_FlushBuff();
  /* Make sure flag is cleared on flush. */
  TEST_ASSERT_FALSE(Line_IsCmplt());
  /* The same but for LF characeter. */
  MockUART_SendChar('a');
  MockUART_SendChar(LINE_CHAR_LF);
  TEST_ASSERT_TRUE(Line_IsCmplt());
  Line_FlushBuff();
  TEST_ASSERT_FALSE(Line_IsCmplt());
  /* Test null termination. */
  memset((void*)test_buff, 0, sizeof(test_buff));
  MockUART_SendChar('a');
  MockUART_SendChar('z');
  MockUART_SendChar('d');
  MockUART_SendChar('q');
  /* MockUART_SendChar(LINE_CHAR_LF); */
  Line_GetBuff(test_buff);
  TEST_ASSERT_EQUAL_STRING("azdq", test_buff);
}

void test_Line_longest_command_wo_oveflow(void) {
  Line_Init(&MockUART_ChrRxCallback);
  /* Longest possible correct command. */
  MockUART_SendCharArray("abdlkslfkdlqksu\n", LINE_BUFF_SIZE);
  TEST_ASSERT_TRUE(Line_BuffIsFull());
  TEST_ASSERT_FALSE(Line_BuffIsOvrFlwn());
  TEST_ASSERT_TRUE(Line_IsCmplt());
  /* Longest possible correct command. */
  MockUART_SendCharArray("abdlkslfkdlqksu\r", LINE_BUFF_SIZE);
  TEST_ASSERT_TRUE(Line_BuffIsFull());
  TEST_ASSERT_FALSE(Line_BuffIsOvrFlwn());
  TEST_ASSERT_TRUE(Line_IsCmplt());
  /* Incorrect command. */
  Line_FlushBuff();
  MockUART_SendCharArray("abdlkslfkdlqksuz", LINE_BUFF_SIZE);
  TEST_ASSERT_TRUE(Line_BuffIsEmpty());
  TEST_ASSERT_TRUE(Line_BuffIsOvrFlwn());
  TEST_ASSERT_FALSE(Line_IsCmplt());
}

void test_Line_buffer_overflow_behavior(void) {
  uint8_t idx;
  Line_Init(&MockUART_ChrRxCallback);
  for(idx = 0; idx < LINE_BUFF_SIZE - 1; idx++) {
    MockUART_SendChar('a');
    TEST_ASSERT_FALSE(Line_BuffIsOvrFlwn());
  }
  MockUART_SendChar('a');
  /* Make sure buffer overflown flag is set and buffer is flushed. */
  TEST_ASSERT_TRUE(Line_BuffIsOvrFlwn());
  TEST_ASSERT_TRUE(Line_BuffIsEmpty());
  TEST_ASSERT_EQUAL_UINT8(0, Line_GetCnt());
  /* When an overflow happens, all characters will be ignored until the next */
  /* eol character is received. Then the buffer is considered 'reset' and */
  /* will start accepting characters again. */
  MockUART_SendCharArray("adlz", 4);
  /* Verify character is ignored as an eol has not been received. */
  TEST_ASSERT_TRUE(Line_BuffIsEmpty());
  TEST_ASSERT_EQUAL_UINT8(0, Line_GetCnt());
  TEST_ASSERT_TRUE(Line_BuffIsOvrFlwn());
  /* Verify recovery conditions from buffer overflow. */
  MockUART_SendChar(LINE_CHAR_CR);
  TEST_ASSERT_FALSE(Line_BuffIsOvrFlwn());
  TEST_ASSERT_TRUE(Line_BuffIsEmpty());
  TEST_ASSERT_EQUAL_UINT8(0, Line_GetCnt());
  /* Buffer will receive characters again. */
  MockUART_SendCharArray("ad", 2);
  TEST_ASSERT_FALSE(Line_BuffIsEmpty());
  TEST_ASSERT_EQUAL_UINT8(2, Line_GetCnt());
}

void test_line_all_tests(void) {
  RUN_TEST(test_Line_Init_function);
  RUN_TEST(test_Line_NewCharCallback_add_chars);
  RUN_TEST(test_Line_BufferIsFull_flag);
  RUN_TEST(test_Line_new_ch_proc_ignore_eol_on_empty_buf);
  RUN_TEST(test_Line_FlushBuff);
  RUN_TEST(test_Line_IsCmplt);
  RUN_TEST(test_Line_longest_command_wo_oveflow);
  RUN_TEST(test_Line_buffer_overflow_behavior);
}
