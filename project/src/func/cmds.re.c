#include "cmds.h"

#include <prpc/lex.h>
#include <prpc/msg.h>

#include <io/pwm.h>

/* ┌────────────────────────────────────────┐
   │ Generic commands                       │
   └────────────────────────────────────────┘ */

PRPC_Parse_Function_t prpc_cmd_parser_get( const char **ptr, const char *end );
PRPC_CMD( has )
{
    const char *name_begin, *name_end;
    PRPC_Status_t stat = prpc_cmd_parse_args( ptr, id, 1, TOKEN_IDENTIFIER, &name_begin, &name_end );
    if( stat.status == PRPC_OK ) {
        PRPC_Parse_Function_t cmd = prpc_cmd_parser_get(&name_begin, name_end);
        return prpc_build_result_boolean( resp_buf, max_resp_len, id, cmd != NULL );
    }
    else return prpc_build_error_status( resp_buf, max_resp_len, id, stat );
}

PRPC_CMD( hello )
{
    return prpc_build_ok( resp_buf, max_resp_len, id );
}

/* ┌────────────────────────────────────────┐
   │ PWM control                            │
   └────────────────────────────────────────┘ */

/* ────────────── start/stop ────────────── */

PRPC_CMD(start_set)
{
	uint8_t value;
	PRPC_Status_t stat = prpc_cmd_parse_args(ptr,id,1,TOKEN_BOOLEAN,&value);

	if(stat.status == PRPC_OK) {
		if(value) pwm_start();
		else      pwm_stop ();

		return prpc_build_ok(resp_buf, max_resp_len, id);
	}

	else {
		return prpc_build_error_status(resp_buf, max_resp_len, id, stat);
	}
}

PRPC_CMD(start_get)
{
	return prpc_build_result(resp_buf, max_resp_len, id, 1, PRPC_BOOLEAN, pwm_started_get());
}

/* ─────────────── frequency ────────────── */

PRPC_CMD( freq_set )
{
	float v;
	PRPC_Status_t stat = prpc_cmd_parse_args(ptr, id, 1, TOKEN_FLOAT, &v);

	if(stat.status == PRPC_OK) {
		if((v < 0.f)) return prpc_build_error(resp_buf, max_resp_len, id, "Value must be >= 0.0");
		else {
			pwm_freq_set(v);
			return prpc_build_ok(resp_buf, max_resp_len, id);
		}
	}

	else {
		return prpc_build_error_status(resp_buf, max_resp_len, id, stat);
	}
}


/* ───────────────── duty ───────────────── */

PRPC_CMD(duty_set)
{
	float v;
	PRPC_Status_t stat = prpc_cmd_parse_args(ptr, id, 1, TOKEN_FLOAT, &v);

	if(stat.status == PRPC_OK) {
		if((v > 1.f) || (v < 0.f)) {
			return prpc_build_error(resp_buf, max_resp_len, id, "Value must be between 0.0 and 1.0");
		}

		else {
			pwm_duty_set(v);
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

PRPC_CMD(polarity_set)
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
			pwm_polarity_set(pol);
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

        *                      { return NULL;                 }
		'has'              end { return prpc_cmd_has;         }
        'hello'            end { return prpc_cmd_hello;       }

		'pwm/started/set'  end { return prpc_cmd_start_set;   }
		'pwm/started/get'  end { return prpc_cmd_start_get;   }
		'pwm/freq/set'     end { return prpc_cmd_freq_set;    }
		'pwm/duty/set'     end { return prpc_cmd_duty_set;    }
		'pwm/polarity/set' end { return prpc_cmd_polarity_set;}
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
