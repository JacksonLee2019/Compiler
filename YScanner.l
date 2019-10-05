/* Author:      Jackson Lee
   Created:     05/15/19
   Resources:   Dr. Steven Senger
                Kailey Linn
*/

Letter	[A-Za-z]
Digit   [0-9]

%{
  #include "IOMngr.h"
  #include "YSemantics.h"
  #include "y.tab.h"

  #define YY_INPUT(buf,result,max_size) \
  { int c = GetSourceChar(); \
		result = (c == EOF) ? YY_NULL : (buf[0] = c, 1); \
    }

    // Set to true to log each token returned to stderr
    bool shouldLog = false;

    // The column of the returned token's first char
    unsigned int tokenStartPosition;

    // Return token number
    // offset = 1 for tokens that include the last char read
    // offset = 0 for tokens that do not include the last char read
    // also computes and sets span of token
    #define RETURN(val,offset) \
      { if (shouldLog) fprintf(stderr,"Token#=%d, Length=%zu, Text=\"%s\"\n",val,(unsigned long)yyleng,yytext); \
        tokenStartPosition = GetLastPosition() - yyleng + offset; \
        yylloc.first_column = tokenStartPosition; \
        yylloc.last_column = yylloc.first_column + yyleng - 1; \
        return(val); \
      }

    // comment implementation
    bool IN_COMMENT;
    int lineComment = 0;
    int comments = 0;

%}

%%
int                             { if (!IN_COMMENT) RETURN(INT_TOK,0); }
chr                             { if (!IN_COMMENT) RETURN(CHR_TOK,0); }
bool                            { if (!IN_COMMENT) RETURN(BOOL_TOK,0); }
void                            { if (!IN_COMMENT) RETURN(VOID_TOK,0); }
true                            { if (!IN_COMMENT) RETURN(BOOLLIT_TOK,0); }
false                           { if (!IN_COMMENT) RETURN(BOOLLIT_TOK,0); }

\>\=                            { if (!IN_COMMENT) RETURN(BGE_TOK,0); }
\>                              { if (!IN_COMMENT) RETURN(BG_TOK,0); }
\<\=                            { if (!IN_COMMENT) RETURN(BLE_TOK,0); }
\<                              { if (!IN_COMMENT) RETURN(BL_TOK,0); }
\#\#                            { if (!IN_COMMENT) RETURN(BNE_TOK,0); }
\=\=                            { if (!IN_COMMENT) RETURN(BEQ_TOK,0); }

put                             { if (!IN_COMMENT) RETURN(PUT_TOK,0); }
get                             { if (!IN_COMMENT) RETURN(GET_TOK,0); }
if                              { if (!IN_COMMENT) RETURN(IF_TOK, 0); }
else                            { if (!IN_COMMENT) RETURN(ELSE_TOK, 0); }
while                           { if (!IN_COMMENT) RETURN(WHILE_TOK, 0); }

-?{Digit}+                      { if (!IN_COMMENT) RETURN(INTLIT_TOK,0); }
'([^'\\\n]|\\.)'                { if (!IN_COMMENT) RETURN(CHRLIT_TOK,0); }
\"(\\.|[^"\\])*\"               { if (!IN_COMMENT) RETURN(STRLIT_TOK,0); }
{Letter}({Letter}|{Digit})*			{ if (!IN_COMMENT) RETURN(IDENT_TOK,0); }

\:\:                            { if (!IN_COMMENT) RETURN(DBLCOLON_TOK,1); }
\-\>                            { if (!IN_COMMENT) RETURN(ARROW_TOK,0); }
\:                              { if (!IN_COMMENT) RETURN(':',1); }
\;                              { if (!IN_COMMENT) RETURN(';',1); }
\)                              { if (!IN_COMMENT) RETURN(')',1); }
\(                              { if (!IN_COMMENT) RETURN('(',1); }
\{                              { if (!IN_COMMENT) RETURN('{',1); }
\}                              { if (!IN_COMMENT) RETURN('}',1); }
\,                              { if (!IN_COMMENT) RETURN(',',1); }

\=                              { if (!IN_COMMENT) RETURN('=',1); }
\+                              { if (!IN_COMMENT) RETURN('+',1); }
\-                              { if (!IN_COMMENT) RETURN('-',1); }
\*                              { if (!IN_COMMENT) RETURN('*',1); }
\/                              { if (!IN_COMMENT) RETURN('/',1); }
\+\+                            { if (!IN_COMMENT) RETURN(INCR_TOK,1); }
\-\-                            { if (!IN_COMMENT) RETURN(DECR_TOK,1); }

[/][/]                          { lineComment = 1; IN_COMMENT = true; }
[/][*]                          { IN_COMMENT = true; comments++; }
[*][/]                          { comments--; if(comments == 0) IN_COMMENT = false; }
[ \t]                           { }
[\n]                            { lineComment = 0; if(comments == 0) IN_COMMENT = false; }
.                               { }
%%
