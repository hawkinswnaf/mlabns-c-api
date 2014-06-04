%{
    #include<stdio.h>
    #include "json.tab.h"
    char *strclone(char *str);
    int input_pos = 0;
    int input_len = 0;
    char *input_string = NULL;
    extern int json_parse_success;
    /*
    #define STRING "str"
    #define NUMBER "number"
    #define O_BEGIN "o_begin"
    #define O_END "o_end"
    #define A_BEGIN "a_begin"
    #define A_END "a_end"
    #define COMMA "comma"
    */
    #define YY_INPUT(buf, result, max_size) \
        {\
            result = (input_pos >= input_len) ? YY_NULL : \
	    (buf[0] = input_string[input_pos++], 1); \
        }

%}
DIGIT1to9 [1-9]
DIGIT [0-9]
DIGITS {DIGIT}+
INT {DIGIT}|{DIGIT1to9}{DIGITS}|-{DIGIT}|-{DIGIT1to9}{DIGITS}
FRAC [.]{DIGITS}
EXP {E}{DIGITS}
E [eE][+-]?
HEX_DIGIT [0-9a-f]
NUMBER {INT}|{INT}{FRAC}|{INT}{EXP}|{INT}{FRAC}{EXP}
UNESCAPEDCHAR [ -!#-\[\]-~]
ESCAPEDCHAR \\["\\bfnrt/]
UNICODECHAR \\u{HEX_DIGIT}{HEX_DIGIT}{HEX_DIGIT}{HEX_DIGIT}
CHAR {UNESCAPEDCHAR}|{ESCAPEDCHAR}|{UNICODECHAR}
CHARS {CHAR}+
DBL_QUOTE ["]
%%
{DBL_QUOTE}{DBL_QUOTE} |
{DBL_QUOTE}{CHARS}{DBL_QUOTE} {
    yylval=strclone(yytext);
    return STRING;
};
{NUMBER} {
    yylval=strclone(yytext);
    return NUMBER;
}
[ ]+ /* eat whitespace */;
true {
    return true;
};
false {
    return false;
};
null {
    return null;
};
\{ {
    return O_BEGIN;
};

\} {
    return O_END;
};

\[ {
    return A_BEGIN;
};

\] {
    return A_END;
};

, {
    return COMMA;
};
: {
    return COLON;
};
. {
    json_parse_success = 0;
}
%%
char *strclone(char *str)
{
    int len = strlen(str);
    char *clone = (char *)malloc(sizeof(char)*(len+1));
    strcpy(clone,str);
    return clone;
}
