#include <stdint.h>
#include <assert.h>
#include <string.h>
#include "utils.h"
#include "cmd.h"

ErrCode_e cmd_1(Arg_s* args, void* usrargs) {
   return E_OK;
}

extern ErrCode_e _get_cmdinfo(const char* cmdstr, const CmdTable_s * cmd_table, const CmdInfo_s** info);

extern ErrCode_e _get_param(const char* rawstr, char* param, uint8_t* done);

extern ErrCode_e _parse_string(const char* rawstr, const CmdTable_s* table_sa, CmdHandle_s* handle);

extern ErrCode_e _get_arg(const char* rawstr, const ArgDesc_s* argdesc_a, Arg_s* arg);

void test__get_param(void) {
   /*************************************************************************/
   /* TEST SETUP ************************************************************/
   /*************************************************************************/
   const uint8_t StrArrSz = 10;
   char str[] = "pwmfreq f2.33 m1";
   char actualname[] = "pwmfreq";
   char str_a[][10] = { "pwmfreq", "f2.33", "m1" };
   char name[CMD_NAME_MAX_SIZE];
   char* ofs = str;
   ErrCode_e res;
   uint8_t done = 0;
   size_t i;
   memset(name, 0, sizeof(name));

   /*************************************************************************/
   /* TEST ARGUMENT VALIDATION **********************************************/
   /*************************************************************************/
   res = _get_param(NULL, name, &done);
   assert(res == E_NULL_PTR);

   res = _get_param(str, NULL, &done);
   assert(res == E_NULL_PTR);

   res = _get_param(str, name, NULL);
   assert(res == E_NULL_PTR);

   /*************************************************************************/
   /* TEST BODY AND VALIDATION **********************************************/
   /*************************************************************************/
   for (i = 0; (!done) && (i < sizeof(str_a) / 10); ++i) {
      res = _get_param(ofs, name, &done);
      assert(res == E_OK);
      assert(strcmp(str_a[i], name) == 0);
      ofs += strlen(name) + 1;
      if (i == 2) {
         assert(done);
      }
   }
}

ErrCode_e dummy_handle(Arg_s* args, void* usrargs) {
   int8_t r = CMD_ARG(args, 0, int8_t);
   int16_t q = CMD_ARG(args, 1, int16_t);
   int32_t f = CMD_ARG(args, 2, int32_t);

   return E_OK;
}

void test__get_cmdinfo(void) {
   /*************************************************************************/
   /* TEST SETUP ************************************************************/
   /*************************************************************************/

   uint8_t i;

   const CmdInfo_s info = {
      /* Command Name. */
      "pwmfreq",

      /* Command Handle. */
      dummy_handle,

      /* Argument Description. */
      {
         {E_ARG_U8, 'r'},
         {E_ARG_I16, 'q'},
         {E_ARG_I32, 'f'},
      },
   };

   const CmdInfo_s info_a[] = {

      /*********************************************************************/
      {
         /* Command Name. */
         "pwmfreq",

         /* Command Handle. */
         dummy_handle,

         /* Argument Description. */
         {
            {E_ARG_U8, 'r'},
            {E_ARG_I16, 'q'},
            {E_ARG_I32, 'f'},
         },
      },
      /*********************************************************************/
      {
         /* Command Name. */
         "pid",

         /* Command Handle. */
         dummy_handle,

         /* Argument Description. */
         {
            {E_ARG_I32, 'p'},
            {E_ARG_I32, 'i'},
            {E_ARG_I32, 'd'},
         },
      },
      /*********************************************************************/
      {
         /* Command Name. */
         "ctrlmode",

         /* Command Handle. */
         dummy_handle,

         /* Argument Description. */
         {
            {E_ARG_U8, 'x'},
            {E_ARG_I16, 'y'},
            {E_ARG_I32, 'z'},
         },
      },
   };

   CmdTable_s cmdtable = { info_a, sizeof(info_a) / sizeof(CmdInfo_s) };

   const CmdInfo_s* p_cmdinfo_s;
   char cmdname[] = "pwmfreq";
   ErrCode_e ret;

   const char cmdname_a[][CMD_NAME_MAX_SIZE] = { "pwmfreq", "pid", "ctrlmode" };

   /*************************************************************************/
   /* TEST ARGUMENT VALIDATION **********************************************/
   /*************************************************************************/
   ret = _get_cmdinfo(NULL, NULL, NULL);
   assert(ret == E_NULL_PTR);

   ret = _get_cmdinfo("pwmfreq", NULL, NULL);
   assert(ret == E_NULL_PTR);

   cmdtable.size = 0;
   ret = _get_cmdinfo("pwmfreq", &cmdtable, &p_cmdinfo_s);
   assert(ret == E_INV_SIZE);
   cmdtable.size = sizeof(info_a) / sizeof(CmdInfo_s);

   /*************************************************************************/
   /* TEST BODY AND VALIDATION **********************************************/
   /*************************************************************************/
   ret = _get_cmdinfo(cmdname, &cmdtable, &p_cmdinfo_s);
   assert(ret == E_OK);
   assert(cmdtable.info_a == p_cmdinfo_s);

   for (i = 0; i < sizeof(info_a) / sizeof(CmdInfo_s); i++) {
      ret = _get_cmdinfo(cmdname_a[i], &cmdtable, &p_cmdinfo_s);
      assert(ret == E_OK);
      assert(&cmdtable.info_a[i] == p_cmdinfo_s);
   }

   // If command is non-existant, p_cmdinfo_s pointer is returned as NULL.
   p_cmdinfo_s = &info_a[0];
   ret = _get_cmdinfo("dummy", &cmdtable, &p_cmdinfo_s);
   assert(ret == E_OK);
   assert(p_cmdinfo_s == NULL);
}

void test__parse_string(void) {
   /*************************************************************************/
   /* TEST SETUP ************************************************************/
   /*************************************************************************/

   const CmdInfo_s info_a[] = {

      /*********************************************************************/
      {
         /* Command Name. */
         "pwmfreq",

         /* Command Handle. */
         dummy_handle,

         /* Argument Description. */
         {
            {E_ARG_U8, 'r'},
            {E_ARG_I16, 'q'},
            {E_ARG_I32, 'f'},
         },
      },
      /*********************************************************************/
      {
         /* Command Name. */
         "pid",

         /* Command Handle. */
         dummy_handle,

         /* Argument Description. */
         {
            {E_ARG_I32, 'p'},
            {E_ARG_I32, 'i'},
            {E_ARG_I32, 'd'},
         },
      },
      /*********************************************************************/
      {
         /* Command Name. */
         "ctrlmode",

         /* Command Handle. */
         dummy_handle,

         /* Argument Description. */
         {
            {E_ARG_U8, 'x'},
            {E_ARG_I16, 'y'},
            {E_ARG_I32, 'z'},
         },
      },
   };

   ErrCode_e ret;
   const char cmdname_a[][CMD_NAME_MAX_SIZE] = { "pwmfreq", "pid", "ctrlmode" };
   char rawstr[CMD_RAW_STR_MAX_SIZE] = "pwmfreq f233 r10 q-40";

   CmdTable_s table_sa;
   CmdHandle_s handle = { NULL, {0}, NULL };
   table_sa.info_a = &info_a[0];
   table_sa.size = 0;
   ret = E_NULL_PTR;
   /*************************************************************************/
   /* TEST ARGUMENT VALIDATION **********************************************/
   /*************************************************************************/
   ret = _parse_string(NULL, NULL, NULL);
   assert(ret == E_NULL_PTR);

   ret = _parse_string(rawstr, NULL, NULL);
   assert(ret == E_NULL_PTR);

   ret = _parse_string(rawstr, &table_sa, NULL);
   assert(ret == E_INV_SIZE);

   /*************************************************************************/
   /* TEST BODY AND VALIDATION **********************************************/
   /*************************************************************************/
   table_sa.size = sizeof(info_a) / sizeof(CmdInfo_s);
   ret = _parse_string(rawstr, &table_sa, &handle);
   assert(handle.callback == dummy_handle);
   assert(ret == E_OK);
}

void test__get_arg(void) {
   /*************************************************************************/
   /* TEST SETUP ************************************************************/
   /*************************************************************************/

   uint8_t i;

   const ArgDesc_s argdesc_a[] = {
      {E_ARG_U8, 'a'},
      {E_ARG_I8, 'b'},
      {E_ARG_U16, 'c'},
      {E_ARG_I16, 'd'},
      {E_ARG_U32, 'e'},
      {E_ARG_I32, 'f'},
   };

   const char argname_a[][CMD_RAW_STR_MAX_SIZE] = {
      "a20",
      "b-32",
      "c65000",
      "d-32000",
      "e70000",
      "f-70000"
   };

   ErrCode_e ret;
   /* Done by Init function. */
   Arg_s arg;
   int32_t values[] = { 20, -32, 65000, -32000, 70000, -70000 };

   uint32_t buf;

   memset(arg.data, 0, CMD_ARG_BYTES_MAX_SIZE);
   /*************************************************************************/
   /* TEST ARGUMENT VALIDATION **********************************************/
   /*************************************************************************/

   /*************************************************************************/
   /* TEST BODY AND VALIDATION **********************************************/
   /*************************************************************************/
   ret = E_OK;
   for (i = 0; i < sizeof(argdesc_a) / sizeof(ArgDesc_s); i++) {
      ret = _get_arg(argname_a[i], &argdesc_a[i], &arg);
      assert(ret == E_OK);
      frombytes((void*)&arg.data[0], ((size_t)CMD_ARG_BYTES_MAX_SIZE), (void*)&buf, sizeof(buf));
      switch (argdesc_a[i].argtype) {
      case E_ARG_U8:
         assert(!memcmp((void*)&buf, (void*)&values[i], sizeof(uint8_t)));
         break;
      case E_ARG_I8:
         assert(!memcmp((void*)&buf, (void*)&values[i], sizeof(int8_t)));
         break;
      case E_ARG_U16:
         assert(!memcmp((void*)&buf, (void*)&values[i], sizeof(uint16_t)));
         break;
      case E_ARG_I16:
         assert(!memcmp((void*)&buf, (void*)&values[i], sizeof(int16_t)));
         break;
      case E_ARG_U32:
         assert(!memcmp((void*)&buf, (void*)&values[i], sizeof(uint32_t)));
         break;
      case E_ARG_I32:
         assert(!memcmp((void*)&buf, (void*)&values[i], sizeof(int32_t)));
         break;
      default:
         break;

      }
   }
}

struct MyAppData {
   uint8_t a;
   int16_t b;
   int32_t c;
};
struct MyAppData appdata = { 1, 2, 3 };

ErrCode_e pwmfreq_handle(Arg_s* args, void* usrargs) {
   uint8_t r = CMD_ARG(args, 0, uint8_t);
   int16_t q = CMD_ARG(args, 1, int16_t);
   int32_t f = CMD_ARG(args, 2, int32_t);

   assert(f == 233);
   assert(CMD_ARG_IS_VALID(args, 2));
   assert(r == 10);
   assert(CMD_ARG_IS_VALID(args, 0));
   assert(q == -40);
   assert(CMD_ARG_IS_VALID(args, 1));
   assert(usrargs == &appdata);

   return E_OK;
}

ErrCode_e pid_handle(Arg_s* args, void* usrargs) {


   return E_GENERIC;
}

ErrCode_e ctrlmode_handle(Arg_s* args, void* usrargs) {
   Arg_s _args[CMD_ARG_MAX_SIZE];
   memset(_args, 0, sizeof(Arg_s) * (CMD_ARG_MAX_SIZE));
   assert(!memcmp(_args, args, sizeof(Arg_s) * (CMD_ARG_MAX_SIZE)));
   return E_OK;
}

void test_cmd(void) {
   /*************************************************************************/
   /* TEST SETUP ************************************************************/
   /*************************************************************************/


   CmdInfo_s info_a[] = {

      /*********************************************************************/
      {
         /* Command Name. */
         "pwmfreq",

         /* Command Handle. */
         pwmfreq_handle,

         /* Argument Description. */
         {
            {E_ARG_U8, 'r'},
            {E_ARG_I16, 'q'},
            {E_ARG_I32, 'f'},
         },

         /* User Arguments */
         (void*)&appdata,
      },
      /*********************************************************************/
      {
         /* Command Name. */
         "pid",

         /* Command Handle. */
         pid_handle,

         /* Argument Description. */
         {
            {E_ARG_I32, 'p'},
            {E_ARG_I32, 'i'},
            {E_ARG_I32, 'd'},

         },

         /* User Arguments */
         CMD_ARG_USER_NONE,
      },
      /*********************************************************************/
      {
         /* Command Name. */
         "ctrlmode",

         /* Command Handle. */
         ctrlmode_handle,

         /* Arguments. */
         CMD_ARG_NONE,

         /*User Arguments */
         CMD_ARG_USER_NONE,
      },
   };

   ErrCode_e ret;
   const char cmdname_a[][CMD_NAME_MAX_SIZE] = { "pwmfreq", "pid", "ctrlmode" };
   char rawstr[CMD_RAW_STR_MAX_SIZE] = "pwmfreq r10 f233 q-40";
   char rawstr1[CMD_RAW_STR_MAX_SIZE] = "ctrlmode";
   /*************************************************************************/
   /* TEST ARGUMENT VALIDATION **********************************************/
   /*************************************************************************/
   ret = Cmd_InitTable(NULL, 0);
   assert(ret == E_NULL_PTR);
   ret = Cmd_InitTable(info_a, 0);
   assert(ret == E_INV_SIZE);
   ret = Cmd_InitTable(info_a, sizeof(info_a) / sizeof(CmdInfo_s));
   assert(ret == E_OK);
   /*************************************************************************/
   /* TEST BODY AND VALIDATION **********************************************/
   /*************************************************************************/
   ret = Cmd_Run(0);
   assert(ret == E_NULL_PTR);
   ret = Cmd_Run(rawstr);
   assert(ret == E_OK);
   ret = Cmd_Run(rawstr1);
   assert(ret == E_OK);
}