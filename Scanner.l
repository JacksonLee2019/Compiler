/* Author:      Jackson Lee
   Created:     03/04/19
   Resources:   Dr. Steven Senger
*/

Letter	[A-Za-z]
Digit   [0-9]

%{
#include "ScanTokens.h"
#include "IOMngr.h"

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
#define RETURN(tokenNumber,offset) \
  { if (shouldLog) fprintf(stderr,"Token#=%d, Length=%zu, Text=\"%s\"\n", \
                          tokenNumber,(unsigned long)yyleng,yytext); \
    tokenStartPosition = GetLastPosition() - yyleng + offset; \
    return(tokenNumber); \
  }

// var for comments
bool IN_COMMENT;
int lineComment = 0;
int comments = 0;

%}


%%
\@INIT                          { if (!IN_COMMENT) RETURN(INIT_TOK,1); }
\@DUMP                          { if (!IN_COMMENT) RETURN(DUMP_TOK,1); }
{Letter}({Letter}|{Digit})*			{ if (!IN_COMMENT) RETURN(IDENT_TOK,0); }
-?{Digit}+                      { if (!IN_COMMENT) RETURN(INT_TOK,0); }
-?{Digit}*\.{Digit}+            { if (!IN_COMMENT) RETURN(FLOAT_TOK,0); }
[=]                             { if (!IN_COMMENT) RETURN(EQUAL_TOK, 1); }
[$]                             { if (!IN_COMMENT) RETURN(DOLLAR_TOK, 1); }
[/][/]                          { lineComment = 1; IN_COMMENT = true; }
[/][*]                          { IN_COMMENT = true; comments++; }
[*][/]                          { comments--; if(comments == 0) IN_COMMENT = false; }
[ \t]                           { }
[\n]                            { lineComment = 0; if(comments == 0) IN_COMMENT = false; }
.                               { }
%%
