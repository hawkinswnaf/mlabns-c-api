/* vim: set expandtab ts=2: */
%{
    #include<stdio.h>
    #include<string.h>
    #include<stdlib.h>
    #include"json_object.h"
    struct json_object *root = NULL;
    #define YYSTYPE char*
    char *strconcat(char *str1, char *str2);
    void yyerror(char const *);
%}
%token NUMBER
%token STRING
%token true false null
%left O_BEGIN O_END A_BEGIN A_END
%left COMMA
%left COLON
%%
START: ARRAY {
  root = $$;
  }
| OBJECT {
  root = $$;
  }
;
OBJECT: O_BEGIN O_END {
    $$ = new_object_object(NULL);
  }
| O_BEGIN MEMBERS O_END {
    $$ = $2;
  }
;
MEMBERS: PAIR {
    $$ = $1;
  }
| PAIR COMMA MEMBERS {
    ((struct json_object*)$1)->next = $3;
    $$ = $1;
  }
;
PAIR: STRING COLON VALUE {

    ((struct json_object*)$3)->name=(char*)malloc(sizeof(char)*(strlen($1)+1));
    sprintf(((struct json_object*)$3)->name,"%s",$1);
    $$ = $3;
}
;
ARRAY: A_BEGIN A_END {
    $$ = new_object_array(NULL);
  }
| A_BEGIN ELEMENTS A_END {
    $$ = $2;
}
;
ELEMENTS: VALUE {
    $$ = new_object_array($1);
  }
| VALUE COMMA ELEMENTS {
    ((struct json_object*)$1)->a_value = $3;
    $$ = $1;
  }
;
VALUE: STRING {
  $$=new_object_single(yylval);
}
| NUMBER {
  $$=new_object_single(yylval);
}
| OBJECT {
  $$=$1;
}
| ARRAY {
  $$=$1;
}
| true {
  $$=new_object_single("true");
}
| false {
  $$=new_object_single("false");
}
| null {
  $$=new_object_single("null");
}
;
%%

int yywrap()
{
   return 1;
}
char *strconcat(char *str1, char *str2)
{
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    char *str3 = (char *)malloc(sizeof(char)*(len1+len2+1));
    strcpy(str3,str1);
    strcpy(&(str3[len1]),str2);
    return str3;
}

void yyerror(char const *s) {
    fprintf(stderr, "%s\n", s);
}

