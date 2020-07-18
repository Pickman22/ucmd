#ifndef UCMD_H
#define UCMD_H

#include "err.h"
#include <stdint.h>

#define CMD_ARG_BYTES_MAX_SIZE 4
#define CMD_DATA_TYPE_BYTES_MAX_SIZE CMD_ARG_BYTES_MAX_SIZE
#define CMD_TABLE_MAX_SIZE 8
#define CMD_ARG_MAX_SIZE 4
#define CMD_NAME_MAX_SIZE 16
#define CMD_RAW_STR_MAX_SIZE 128
#define CMD_FP_TABLE_MAX_SIZE 32

#define CMD_ARG(_args, _idx, _type) (_type)(*(((_type*)(&(_args)[(_idx)].data))))
#define CMD_ARG_IS_VALID(_args, _idx) ((_args)[(_idx)].is_valid)
#define CMD_ARG_NONE {{E_ARG_NONE_TYPE, 0}}
#define CMD_ARG_USER_NONE (NULL)

typedef enum ArgType {
  E_ARG_U8 = 0,
  E_ARG_U16,
  E_ARG_U32,
  E_ARG_I8,
  E_ARG_I16,
  E_ARG_I32,
  E_ARG_STR,
  E_ARG_NONE_TYPE = 254,
  E_ARG_INV_TYPE = 255,
} ArgType_e;

typedef struct ArgDesc {
  ArgType_e argtype;
  char argname;
} ArgDesc_s;

typedef struct Arg {
  const ArgDesc_s* desc;
  uint8_t data[CMD_ARG_BYTES_MAX_SIZE];
  uint8_t is_valid;
} Arg_s;

typedef ErrCode_e Callback_t(Arg_s* args, void* usrargs);

typedef Callback_t* CallbackPtr_t;

typedef struct CmdInfo {
  const char cmdname[CMD_NAME_MAX_SIZE];
  const CallbackPtr_t handle;
  const ArgDesc_s argdesc[CMD_ARG_MAX_SIZE];
  void* userarg;
} CmdInfo_s;

typedef struct CmdTable {
  const CmdInfo_s* info_a;
  size_t size;
} CmdTable_s;

typedef struct CmdHandle {
   CallbackPtr_t callback;
   Arg_s args[CMD_ARG_MAX_SIZE];
   void* userarg;
} CmdHandle_s;


ErrCode_e Cmd_InitTable(const CmdInfo_s* cmdtable, size_t table_sz);

ErrCode_e Cmd_Run(const char* cmdstr);

#endif
