/* ┌─────────────────┐
   │ Pins definition │
   └─────────────────┘
   
    Florian Dupeyron
    April 2022
*/

#include "pin.h"


/* ┌────────────────────────────────────────┐
   │ Pins defines                           │
   └────────────────────────────────────────┘ */

const struct Pin PIN_LD2        = { .port = GPIOB, .pin = GPIO_PIN_13 };
const struct Pin PIN_USART_TX   = { .port = GPIOA, .pin = GPIO_PIN_2  };
const struct Pin PIN_USART_RX   = { .port = GPIOA, .pin = GPIO_PIN_3  };

const struct Pin PIN_PWM1_OUT   = { .port = GPIOB, .pin = GPIO_PIN_14 };
const struct Pin PIN_PWM2_OUT   = { .port = GPIOA, .pin = GPIO_PIN_12 };
const struct Pin PIN_PWM3_OUT   = { .port = GPIOA, .pin = GPIO_PIN_7  };
const struct Pin PIN_PWM4_OUT   = { .port = GPIOA, .pin = GPIO_PIN_8  };

const struct Pin PIN_FMETER1_IN = { .port = GPIOA, .pin = GPIO_PIN_0  };
const struct Pin PIN_FMETER2_IN = { .port = GPIOA, .pin = GPIO_PIN_4  };
const struct Pin PIN_FMETER3_IN = { .port = GPIOB, .pin = GPIO_PIN_6  };
