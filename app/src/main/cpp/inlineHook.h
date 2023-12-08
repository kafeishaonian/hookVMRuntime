//
// Created by MOMO on 2023/12/5.
//

#ifndef BAA_INLINEHOOK_H
#define BAA_INLINEHOOK_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/ptrace.h>

#include "relocate.h"

#ifdef __cplusplus
extern "C" {
#endif
enum ele7en_status {
    ELE7EN_ERROR_UNKNOWN = -1,
    ELE7EN_OK = 0,
    ELE7EN_ERROR_NOT_INITIALIZED,
    ELE7EN_ERROR_NOT_EXECUTABLE,
    ELE7EN_ERROR_NOT_REGISTERED,
    ELE7EN_ERROR_NOT_HOOKED,
    ELE7EN_ERROR_ALREADY_REGISTERED,
    ELE7EN_ERROR_ALREADY_HOOKED,
    ELE7EN_ERROR_SO_NOT_FOUND,
    ELE7EN_ERROR_FUNCTION_NOT_FOUND
};

enum ele7en_status
registerInlineHook(uint32_t target_addr, uint32_t new_addr, uint32_t **proto_addr);
enum ele7en_status inlineUnHook(uint32_t target_addr);
void inlineUnHookAll();
enum ele7en_status inlineHook(uint32_t target_addr);
void inlineHookAll();

#ifdef __cplusplus
};
#endif

#endif //BAA_INLINEHOOK_H
