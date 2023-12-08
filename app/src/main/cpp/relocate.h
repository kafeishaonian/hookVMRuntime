//
// Created by MOMO on 2023/11/22.
//

#ifndef BAA_RELOCATE_H
#define BAA_RELOCATE_H

#include <stdio.h>

void relocateInstruction(uint32_t target_addr, void *orig_instructions, int length, void *trampoline_instructions, int *orig_boundaries, int *trampoline_boundaries, int *count);


#endif //BAA_RELOCATE_H
