#include "cmds.h"

#include <prpc/lex.h>
#include <prpc/msg.h>

PRPC_Parse_Function_t prpc_cmd_parser_get( const char **ptr, const char *end );
PRPC_CMD( has )
{
    const char *name_begin, *name_end;
    PRPC_Status_t stat = prpc_cmd_parse_args( ptr, id, 1, TOKEN_IDENTIFIER, &name_begin, &name_end );
    if( stat.status == PRPC_OK ) {
        PRPC_Parse_Function_t cmd = prpc_cmd_parser_get(&name_begin, name_end);
        prpc_build_result_boolean( resp_buf, max_resp_len, id, cmd != NULL );
    }
    else prpc_build_error_status( resp_buf, max_resp_len, id, stat );
}

PRPC_CMD( hello )
{
    prpc_build_ok( resp_buf, max_resp_len, id );
}

PRPC_Parse_Function_t prpc_cmd_parser_get( const char **ptr, const char *end )
{
    const char *YYMARKER;

    /*!re2c
        re2c:define:YYCTYPE  = char;
        re2c:define:YYCURSOR = (*ptr);
        re2c:define:YYLIMIT  = end;
        re2c:yyfill:enable   = 0;

        end = [ \t\r\n] | '\x00';

        *               { return NULL;               }
        'hello'     end { return prpc_cmd_hello;     }
     */
}

void process_cmd( char *resp, const size_t max_len, const PRPC_ID_t id, const char *name_start, const char *name_end, const char **ptr )
{
    PRPC_Parse_Function_t cmd = prpc_cmd_parser_get(&name_start, name_end);
    if( cmd != NULL ) {
        cmd( ptr, resp, max_len, id );
    }

    else {
        prpc_build_error( resp, max_len, id, "Uknown method" );
    }
}

void cmds_init()
{
    prpc_process_callback_register( process_cmd, NULL );
}
