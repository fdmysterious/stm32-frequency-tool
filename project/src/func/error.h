/* ┌─────────────────┐
   │ Error utilities │
   └─────────────────┘
   
    Florian Dupeyron
    March 2022
*/

#pragma once

#include <inttypes.h>

void error_handler(void);
void assert_failed(uint8_t *file, uint32_t line);
