#ifndef INC_SYSTEM_H
#define INC_SYSTEM_H

#include "common-defines.h"


#define SYSTICK_FREQ (1000)
#define CPU_FREQ (84000000)

uint64_t system_get_ticks(void);
void system_setup(void);


#endif //INC_SYSTEM_H