#include "cmds.h"

#include <prpc/lex.h>
#include <prpc/msg.h>

#include <io/pwm.h>
#include <io/freqmeter.h>


/* ┌────────────────────────────────────────┐
   │ Generic commands                       │
   └────────────────────────────────────────┘ */

PRPC_Parse_Function_t prpc_cmd_parser_get( const char **ptr, const char *end );
size_t prpc_cmd_has(const char **ptr, char *resp_buf, const size_t max_resp_len, PRPC_ID_t id)
{
    const char *name_begin, *name_end;
    PRPC_Status_t stat = prpc_cmd_parse_args( ptr, id, 1, TOKEN_IDENTIFIER, &name_begin, &name_end );
    if( stat.status == PRPC_OK ) {
        PRPC_Parse_Function_t cmd = prpc_cmd_parser_get(&name_begin, name_end);
        return prpc_build_result_boolean( resp_buf, max_resp_len, id, cmd != NULL );
    }
    else return prpc_build_error_status( resp_buf, max_resp_len, id, stat );
}

size_t prpc_cmd_hello(const char **ptr, char *resp_buf, const size_t max_resp_len, PRPC_ID_t id)
{
    return prpc_build_ok( resp_buf, max_resp_len, id );
}


/* ┌────────────────────────────────────────┐
   │ PWM control                            │
   └────────────────────────────────────────┘ */

/* ────────────── start/stop ────────────── */

size_t prpc_cmd_pwmx_start_set(struct PWM_Data *pwm, const char **ptr, char *resp_buf, const size_t max_resp_len, PRPC_ID_t id)
{
	uint8_t value;
	PRPC_Status_t stat = prpc_cmd_parse_args(ptr,id,1,TOKEN_BOOLEAN,&value);

	if(stat.status == PRPC_OK) {
		if(value) pwm_start(pwm);
		else      pwm_stop (pwm);

		return prpc_build_ok(resp_buf, max_resp_len, id);
	}

	else {
		return prpc_build_error_status(resp_buf, max_resp_len, id, stat);
	}
}

size_t prpc_cmd_pwmx_start_get(struct PWM_Data *pwm, const char **ptr, char *resp_buf, const size_t max_resp_len, PRPC_ID_t id)
{
	return prpc_build_result(resp_buf, max_resp_len, id, 1, PRPC_BOOLEAN, pwm_started_get(pwm));
}

/* ─────────────── frequency ────────────── */

size_t prpc_cmd_pwmx_freq_set(struct PWM_Data *pwm, const char **ptr, char *resp_buf, const size_t max_resp_len, PRPC_ID_t id)
{
	float v;
	PRPC_Status_t stat = prpc_cmd_parse_args(ptr, id, 1, TOKEN_FLOAT, &v);

	if(stat.status == PRPC_OK) {
		if((v < 0.f)) return prpc_build_error(resp_buf, max_resp_len, id, "Value must be >= 0.0");
		else {
			pwm_freq_set(pwm, v);
			return prpc_build_ok(resp_buf, max_resp_len, id);
		}
	}

	else {
		return prpc_build_error_status(resp_buf, max_resp_len, id, stat);
	}
}


/* ───────────────── duty ───────────────── */

size_t prpc_cmd_pwmx_duty_set(struct PWM_Data *pwm, const char **ptr, char *resp_buf, const size_t max_resp_len, PRPC_ID_t id)
{
	float v;
	PRPC_Status_t stat = prpc_cmd_parse_args(ptr, id, 1, TOKEN_FLOAT, &v);

	if(stat.status == PRPC_OK) {
		if((v > 1.f) || (v < 0.f)) {
			return prpc_build_error(resp_buf, max_resp_len, id, "Value must be between 0.0 and 1.0");
		}

		else {
			pwm_duty_set(pwm, v);
			return prpc_build_ok(resp_buf, max_resp_len, id);
		}
	}
	
	else {
		return prpc_build_error_status(resp_buf, max_resp_len, id, stat);
	}
}

/* ─────────────── polarity ─────────────── */

enum PWM_Polarity __parse_pwm_polarity(const char *start, const char *end)
{
	/* Character that is above one char → cannot be valid*/
	if((start+1) != end) return PWM_POLARITY_UNKNOWN;
	switch(*start) {
		case '+': return PWM_POSITIVE;
		case '-': return PWM_NEGATIVE;
		default:  return PWM_POLARITY_UNKNOWN;
	}
}

size_t prpc_cmd_pwmx_polarity_set(struct PWM_Data *pwm, const char **ptr, char *resp_buf, const size_t max_resp_len, PRPC_ID_t id)
{
	enum PWM_Polarity   pol;
	char *id_start, *id_end;
	PRPC_Status_t      stat = prpc_cmd_parse_args(ptr, id, 1, TOKEN_STRING, &id_start, &id_end);

	/* Parse arguments */
	if(stat.status == PRPC_OK) {
		pol = __parse_pwm_polarity(id_start, id_end);
		if(pol == PWM_POLARITY_UNKNOWN) {
			return prpc_build_error(resp_buf, max_resp_len, id, "Invalid polarity, can be '+' or '-'");
		}

		else {
			pwm_polarity_set(pwm, pol);
			return prpc_build_ok(resp_buf, max_resp_len, id);
		}

	}

	else {
		return prpc_build_error_status(resp_buf, max_resp_len, id, stat);
	}
}


/* ┌────────────────────────────────────────┐
   │ PWM commands for each channel          │
   └────────────────────────────────────────┘ */

/* ────────────── Start/Stop ────────────── */

#define PWM_START_GET_IMPL(ch) size_t prpc_cmd_pwm##ch##_start_get(const char **ptr, char *resp_buf, const size_t max_resp_len, PRPC_ID_t id) {\
	return prpc_cmd_pwmx_start_get(&pwm_ch##ch, ptr, resp_buf, max_resp_len, id); \
}

PWM_START_GET_IMPL(1)
PWM_START_GET_IMPL(2)
PWM_START_GET_IMPL(3)
PWM_START_GET_IMPL(4)

#define PWM_START_SET_IMPL(ch) size_t prpc_cmd_pwm##ch##_start_set(const char **ptr, char *resp_buf, const size_t max_resp_len, PRPC_ID_t id) {\
	return prpc_cmd_pwmx_start_set(&pwm_ch##ch, ptr, resp_buf, max_resp_len, id); \
}

PWM_START_SET_IMPL(1)
PWM_START_SET_IMPL(2)
PWM_START_SET_IMPL(3)
PWM_START_SET_IMPL(4)

/* ─────────── Frequency control ────────── */

#define PWM_FREQ_SET_IMPL(ch) size_t prpc_cmd_pwm##ch##_freq_set(const char **ptr, char *resp_buf, const size_t max_resp_len, PRPC_ID_t id) {\
	return prpc_cmd_pwmx_freq_set(&pwm_ch##ch, ptr, resp_buf, max_resp_len, id); \
}

PWM_FREQ_SET_IMPL(1)
PWM_FREQ_SET_IMPL(2)
PWM_FREQ_SET_IMPL(3)
PWM_FREQ_SET_IMPL(4)


/* ───────────── Duty control ───────────── */

#define PWM_DUTY_SET_IMPL(ch) size_t prpc_cmd_pwm##ch##_duty_set(const char **ptr, char *resp_buf, const size_t max_resp_len, PRPC_ID_t id) {\
	return prpc_cmd_pwmx_duty_set(&pwm_ch##ch, ptr, resp_buf, max_resp_len, id); \
}

PWM_DUTY_SET_IMPL(1)
PWM_DUTY_SET_IMPL(2)
PWM_DUTY_SET_IMPL(3)
PWM_DUTY_SET_IMPL(4)

/* ─────────── Polarity control ─────────── */

#define PWM_POLARITY_SET_IMPL(ch) size_t prpc_cmd_pwm##ch##_polarity_set(const char **ptr, char *resp_buf, const size_t max_resp_len, PRPC_ID_t id) {\
	return prpc_cmd_pwmx_polarity_set(&pwm_ch##ch, ptr, resp_buf, max_resp_len, id); \
}

PWM_POLARITY_SET_IMPL(1)
PWM_POLARITY_SET_IMPL(2)
PWM_POLARITY_SET_IMPL(3)
PWM_POLARITY_SET_IMPL(4)


/* ┌────────────────────────────────────────┐
   │ Frequency meter commands               │
   └────────────────────────────────────────┘ */

#define FMETER_PERIOD_GET_IMPL(num) size_t prpc_cmd_fmeter##num##_period_get(const char **ptr, char *resp_buf, const size_t max_resp_len, PRPC_ID_t id) {\
	float tmp = (float)fmeter##num.movmean_period.mean/((float)HAL_RCC_GetHCLKFreq()/1e6f);\
	return prpc_build_result(resp_buf, max_resp_len, id, 1, PRPC_FLOAT, tmp);\
}

#define FMETER_POSITIVE_GET_IMPL(num) size_t prpc_cmd_fmeter##num##_positive_get(const char **ptr, char *resp_buf, const size_t max_resp_len, PRPC_ID_t id) {\
	float tmp = (float)fmeter##num.movmean_positive.mean/((float)HAL_RCC_GetHCLKFreq()/1e6f);\
	return prpc_build_result(resp_buf, max_resp_len, id, 1, PRPC_FLOAT, tmp);\
}

FMETER_PERIOD_GET_IMPL(1)
FMETER_PERIOD_GET_IMPL(2)
FMETER_PERIOD_GET_IMPL(3)

FMETER_POSITIVE_GET_IMPL(1)
FMETER_POSITIVE_GET_IMPL(2)
FMETER_POSITIVE_GET_IMPL(3)


/* ┌────────────────────────────────────────┐
   │ Function name parser                   │
   └────────────────────────────────────────┘ */

PRPC_Parse_Function_t prpc_cmd_parser_get( const char **ptr, const char *end )
{
    const char *YYMARKER;

    /*!re2c
        re2c:define:YYCTYPE  = char;
        re2c:define:YYCURSOR = (*ptr);
        re2c:define:YYLIMIT  = end;
        re2c:yyfill:enable   = 0;

        end = [ \t\r\n] | '\x00';

        *                          { return NULL;                         }
		'has'                  end { return prpc_cmd_has;                 }
        'hello'                end { return prpc_cmd_hello;               }

		'pwm/1/started/set'     end { return prpc_cmd_pwm1_start_set;      }
		'pwm/1/started/get'     end { return prpc_cmd_pwm1_start_get;      }
		'pwm/1/freq/set'        end { return prpc_cmd_pwm1_freq_set;       }
		'pwm/1/duty/set'        end { return prpc_cmd_pwm1_duty_set;       }
		'pwm/1/polarity/set'    end { return prpc_cmd_pwm1_polarity_set;   }

		'pwm/2/started/set'     end { return prpc_cmd_pwm2_start_set;      }
		'pwm/2/started/get'     end { return prpc_cmd_pwm2_start_get;      }
		'pwm/2/freq/set'        end { return prpc_cmd_pwm2_freq_set;       }
		'pwm/2/duty/set'        end { return prpc_cmd_pwm2_duty_set;       }
		'pwm/2/polarity/set'    end { return prpc_cmd_pwm2_polarity_set;   }

		'pwm/3/started/set'     end { return prpc_cmd_pwm3_start_set;      }
		'pwm/3/started/get'     end { return prpc_cmd_pwm3_start_get;      }
		'pwm/3/freq/set'        end { return prpc_cmd_pwm3_freq_set;       }
		'pwm/3/duty/set'        end { return prpc_cmd_pwm3_duty_set;       }
		'pwm/3/polarity/set'    end { return prpc_cmd_pwm3_polarity_set;   }

		'pwm/4/started/set'     end { return prpc_cmd_pwm4_start_set;      }
		'pwm/4/started/get'     end { return prpc_cmd_pwm4_start_get;      }
		'pwm/4/freq/set'        end { return prpc_cmd_pwm4_freq_set;       }
		'pwm/4/duty/set'        end { return prpc_cmd_pwm4_duty_set;       }
		'pwm/4/polarity/set'    end { return prpc_cmd_pwm4_polarity_set;   }

		'fmeter/1/period/get'   end { return prpc_cmd_fmeter1_period_get;  }
		'fmeter/1/positive/get' end { return prpc_cmd_fmeter1_positive_get;}
		'fmeter/2/period/get'   end { return prpc_cmd_fmeter2_period_get;  }
		'fmeter/2/positive/get' end { return prpc_cmd_fmeter2_positive_get;}
		'fmeter/3/period/get'   end { return prpc_cmd_fmeter3_period_get;  }
		'fmeter/3/positive/get' end { return prpc_cmd_fmeter3_positive_get;}
     */
}


/* ┌────────────────────────────────────────┐
   │ Process command                        │
   └────────────────────────────────────────┘ */

size_t process_cmd( char *resp, const size_t max_len, const PRPC_ID_t id, const char *name_start, const char *name_end, const char **ptr )
{
    PRPC_Parse_Function_t cmd = prpc_cmd_parser_get(&name_start, name_end);
    if( cmd != NULL ) {
        return cmd( ptr, resp, max_len, id );
    }

    else {
        return prpc_build_error( resp, max_len, id, "Uknown method" );
    }
}


/* ┌────────────────────────────────────────┐
   │ Init                                   │
   └────────────────────────────────────────┘ */

void cmds_init()
{
    prpc_process_callback_register( process_cmd, NULL );
}
