#include "utils.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

#define MODUDLE_NAME "utils"

void test_utils_findch(void) {
  /*************************************************************************/
  /* TEST SETUP ************************************************************/
  /*************************************************************************/
  char test_str[] = "This is a test string";
  ErrCode_e res;
  int16_t idx;
  char dch = 'a';

  /*************************************************************************/
  /* TEST ARGUMENT VALIDATION **********************************************/
  /*************************************************************************/
  res = findch(NULL, 0, &idx);
  assert(res == E_NULL_PTR);

  res = findch(test_str, 0, NULL);
  assert(res == E_NULL_PTR);

  /*************************************************************************/
  /* TEST BODY AND VALIDATION **********************************************/
  /*************************************************************************/
  res = findch(test_str, dch, &idx);
  assert(res == E_OK);
  assert(idx == 8);

  res = findch(test_str, 'z', &idx);
  assert(res == E_OK);
  assert(idx == -1);

  res = findch(test_str, 'T', &idx);
  assert(res == E_OK);
  assert(idx == 0);
}

void test_utils_asbytes(void) {
  /*************************************************************************/
  /* TEST SETUP ************************************************************/
  /*************************************************************************/

  // TEST VARS.
  float fval[] = {-23.44f, 0.0f, 2444.23f};
  int8_t i8val[] = {-12, 0, 240};
  int16_t i16val[] = {-400, 0, 500};
  int32_t i32val[] = {-80000, 0, 90123};
  
  // TEMP VARS.
  uint8_t i;
  uint8_t buf[8];
  ErrCode_e res;

  // RESULT VARS.
  float fres = 0.f;
  int8_t i8res = 0;
  int16_t i16res = 0;
  int32_t i32res = 0;


  /*************************************************************************/
  /* TEST ARGUMENT VALIDATION **********************************************/
  /*************************************************************************/

  // Null pointers.
  res = frombytes(NULL, sizeof(buf), (void*)&fval[0], sizeof(float)); 
  assert(res == E_NULL_PTR);
  res = frombytes(buf, sizeof(buf), NULL, sizeof(float)); 
  assert(res == E_NULL_PTR);

  // Invalid buffer size as 0.
  res = frombytes(buf, 0, (void*)&fval[0], sizeof(float));
  assert(res == E_INV_SIZE);

  // Invalid buffer size as smaller than data.
  res = frombytes(buf, 1, (void*)&fval[0], sizeof(float));
  assert(res == E_INV_SIZE);

  // Invalid data size as 0.
  res = frombytes(buf, sizeof(buf), (void*)&fval[0], 0);
  assert(res == E_INV_SIZE);

  /*************************************************************************/
  /* TEST BODY AND VALIDATION **********************************************/
  /*************************************************************************/


  // FLOAT VALUES
  for(i = 0; i < sizeof(fval) / sizeof(float); i++) {
    res = tobytes(buf, sizeof(buf), (void*)&fval[i], sizeof(float));
    assert(res == E_OK);
    res = frombytes(buf, sizeof(buf), (void*)&fres, sizeof(float));
    assert(res == E_OK);
    assert(fabs(fres - fval[i]) <= 1.0e-4);
  }

  // INT8 VALUES
  for(i = 0; i < sizeof(i8val) / sizeof(int8_t); i++) {
    res = tobytes(buf, sizeof(buf), (void*)&i8val[i], sizeof(int8_t));
    assert(res == E_OK);
    res = frombytes(buf, sizeof(buf), (void*)&i8res, sizeof(int8_t));
    assert(res == E_OK);
    assert(i8res == i8val[i]);
  }

  // INT16 VALUES
  for(i = 0; i < sizeof(i16val) / sizeof(int16_t); i++) {
    res = tobytes(buf, sizeof(buf), (void*)&i16val[i], sizeof(int16_t));
    assert(res == E_OK);
    res = frombytes(buf, sizeof(buf), (void*)&i16res, sizeof(int16_t));
    assert(res == E_OK);
    assert(i16res == i16val[i]);
  }

  // INT32 VALUES
  for(i = 0; i < sizeof(i32val) / sizeof(int32_t); i++) {
    res = tobytes(buf, sizeof(buf), (void*)&i32val[i], sizeof(int32_t));
    assert(res == E_OK);
    res = frombytes(buf, sizeof(buf), (void*)&i32res, sizeof(int32_t));
    assert(res == E_OK);
    assert(i32res == i32val[i]);
  }
}

void test_strtou32(void) {
  /*************************************************************************/
  /* TEST SETUP ************************************************************/
  /*************************************************************************/
  uint32_t num;
  ErrCode_e ret;

  /*************************************************************************/
  /* TEST ARGUMENT VALIDATION **********************************************/
  /*************************************************************************/
  ret = strtou32(NULL, NULL);
  assert(ret == E_NULL_PTR);

  ret = strtou32("230", NULL);
  assert(ret == E_NULL_PTR);

  ret = strtou32("", &num);
  assert(ret == E_INV_SIZE);
  /*************************************************************************/
  /* TEST BODY AND VALIDATION **********************************************/
  /*************************************************************************/

  ret = strtou32("4294967295", &num);
  assert(ret == E_OK);
  assert(num == 4294967295);

  ret = strtou32("0", &num);
  assert(ret == E_OK);
  assert(num == 0);

  ret = strtou32("12--23", &num);
  assert(ret == E_OUT_OF_RANGE);
}

void test_strtoi32(void) {
  /*************************************************************************/
  /* TEST SETUP ************************************************************/
  /*************************************************************************/
  int32_t num;
  ErrCode_e ret;

  /*************************************************************************/
  /* TEST ARGUMENT VALIDATION **********************************************/
  /*************************************************************************/
  ret = strtoi32(NULL, NULL);
  assert(ret == E_NULL_PTR);

  ret = strtoi32("230", NULL);
  assert(ret == E_NULL_PTR);

  ret = strtoi32("", &num);
  assert(ret == E_INV_SIZE);
  /*************************************************************************/
  /* TEST BODY AND VALIDATION **********************************************/
  /*************************************************************************/
  //TODO: Test positive and negative extreme values.

  ret = strtoi32("230", &num);
  assert(ret == E_OK);
  assert(num == 230);


  ret = strtoi32("0", &num);
  assert(ret == E_OK);
  assert(num == 0);

  ret = strtoi32("-0", &num);
  assert(ret == E_OK);
  assert(num == 0);

  ret = strtoi32("-123", &num);
  assert(ret == E_OK);
  assert(num == -123);

  ret = strtoi32("12--23", &num);
  assert(ret == E_OUT_OF_RANGE);
}