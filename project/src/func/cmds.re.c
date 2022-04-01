#include "cmds.h"

#include <prpc/lex.h>
#include <prpc/msg.h>

#include <io/pwm.h>

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

size_t prpc_cmd_start_set(const char **ptr, char *resp_buf, const size_t max_resp_len, PRPC_ID_t id)
{
	uint8_t value;
	PRPC_Status_t stat = prpc_cmd_parse_args(ptr,id,1,TOKEN_BOOLEAN,&value);

	if(stat.status == PRPC_OK) {
		if(value) pwm_start(&pwm_ch1);
		else      pwm_stop (&pwm_ch1);

		return prpc_build_ok(resp_buf, max_resp_len, id);
	}

	else {
		return prpc_build_error_status(resp_buf, max_resp_len, id, stat);
	}
}

size_t prpc_cmd_start_get(const char **ptr, char *resp_buf, const size_t max_resp_len, PRPC_ID_t id)
{
	return prpc_build_result(resp_buf, max_resp_len, id, 1, PRPC_BOOLEAN, pwm_started_get(&pwm_ch1));
}

/* ─────────────── frequency ────────────── */

size_t prpc_cmd_freq_set(const char **ptr, char *resp_buf, const size_t max_resp_len, PRPC_ID_t id)
{
	float v;
	PRPC_Status_t stat = prpc_cmd_parse_args(ptr, id, 1, TOKEN_FLOAT, &v);

	if(stat.status == PRPC_OK) {
		if((v < 0.f)) return prpc_build_error(resp_buf, max_resp_len, id, "Value must be >= 0.0");
		else {
			pwm_freq_set(&pwm_ch1, v);
			return prpc_build_ok(resp_buf, max_resp_len, id);
		}
	}

	else {
		return prpc_build_error_status(resp_buf, max_resp_len, id, stat);
	}
}


/* ───────────────── duty ───────────────── */

size_t prpc_cmd_duty_set(const char **ptr, char *resp_buf, const size_t max_resp_len, PRPC_ID_t id)
{
	float v;
	PRPC_Status_t stat = prpc_cmd_parse_args(ptr, id, 1, TOKEN_FLOAT, &v);

	if(stat.status == PRPC_OK) {
		if((v > 1.f) || (v < 0.f)) {
			return prpc_build_error(resp_buf, max_resp_len, id, "Value must be between 0.0 and 1.0");
		}

		else {
			pwm_duty_set(&pwm_ch1, v);
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

size_t prpc_cmd_polarity_set(const char **ptr, char *resp_buf, const size_t max_resp_len, PRPC_ID_t id)
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
			pwm_polarity_set(&pwm_ch1, pol);
			return prpc_build_ok(resp_buf, max_resp_len, id);
		}

	}

	else {
		return prpc_build_error_status(resp_buf, max_resp_len, id, stat);
	}
}


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

        *                       { return NULL;                 }
		'has'               end { return prpc_cmd_has;         }
        'hello'             end { return prpc_cmd_hello;       }

		'pwm1/started/set'  end { return prpc_cmd_start_set;   }
		'pwm1/started/get'  end { return prpc_cmd_start_get;   }
		'pwm1/freq/set'     end { return prpc_cmd_freq_set;    }
		'pwm1/duty/set'     end { return prpc_cmd_duty_set;    }
		'pwm1/polarity/set' end { return prpc_cmd_polarity_set;}
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
