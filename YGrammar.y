/* Author:      Jackson Lee
   Created:     05/15/19
   Resources:   Dr. Steven Senger
                Kailey Linn
*/

%{
  #include <stdio.h>
  #include <string.h>
  #include <stdlib.h>
  #include "IOMngr.h"
  #include "Scanner.h"
  #include "YSemantics.h"

  void yyerror(char *s);

  #define MAKE_SPAN(loc) MakeSpan(loc.first_column,loc.last_column)
%}

%locations

/* Union structure of data that can be attached to non-terminals   */
/* type union-field-name                                           */
%union     {
  int Integer;
  char * Text;
  enum BaseTypes BaseType;
  enum Operators Operator;
  struct IdList * IdList;
  struct LiteralDesc * LiteralDesc;
  struct InstrSeq * InstrSeq;
  struct ExprResult * ExprResult;
  struct CondResult * CondResult;
}

/* Type declaration for data attached to non-terminals. Allows     */
/* $# notation to be correctly type casted for function arguments. */
/* %type <union-field-name> non-terminal                           */
%type <Text> Id
%type <Text> String
%type <IdList> IdItem
%type <IdList> IdList
%type <BaseType> BaseType
%type <LiteralDesc> Literal
%type <BaseType> FuncDecl
%type <InstrSeq> CodeBlock
%type <InstrSeq> StmtSeq
%type <InstrSeq> Stmt
%type <InstrSeq> AssignStmt
%type <ExprResult> Expr
%type <ExprResult> Oprnd
%type <InstrSeq> PutStmt
%type <InstrSeq> IfStmt
%type <InstrSeq> ElseStmt
%type <InstrSeq> WhileStmt
%type <InstrSeq> FuncStmt
%type <CondResult> Cond

/* List of token name, corresponding numbers will be generated */
/* y.tab.h will be generated from these */
/* %token symbolic-name text-used-in-rules */
%token IDENT_TOK
%token INT_TOK    	   "int"
%token CHR_TOK    	   "chr"
%token BOOL_TOK        "bool"
%token VOID_TOK        "void"
%token INTLIT_TOK
%token CHRLIT_TOK
%token BOOLLIT_TOK
%token STRLIT_TOK
%token ARROW_TOK       "->"
%token DBLCOLON_TOK    "::"
%token GET_TOK         "get"
%token PUT_TOK         "put"
%token IF_TOK          "if"
%token ELSE_TOK        "else"
%token WHILE_TOK       "while"
%token BGE_TOK         ">="
%token BG_TOK          ">"
%token BLE_TOK         "<="
%token BL_TOK          "<"
%token BNE_TOK         "##"
%token BEQ_TOK         "=="
%token INCR_TOK        "++"
%token DECR_TOK        "--"

/* operator associativity and precedence in increasing order */
%left '+' '-'
%left '*' '/'
%left UMINUS

%%
// Step 1
Module        : DeclList                                    { };

DeclList      : Decl DeclList                               { };
DeclList      : Decl                                        { };

Decl          : IdList ':' BaseType                         { ProcDecl($1,$3,0); };
Decl          : IdList "::" Literal                         { ProcDecl($1,$3->baseType,$3->value); };
Decl          : IdList ':' FuncDecl                         { ProcDeclFunc($1,$3); };
Decl          : IdList "::" CodeBlock                       { ProcFuncBody($1,$3); };

IdList        : IdItem ',' IdList                           { $$ = AppendIdList($1,$3); };
IdList        : IdItem                                      { $$ = $1; };

IdItem        : Id                                          { $$ = ProcName($1,MAKE_SPAN(@1)); }

Id            : IDENT_TOK                                   { @$ = @1; $$ = strdup(yytext); };

BaseType      : "int"                                       { @$ = @1; $$ = IntBaseType; };
BaseType      : "chr"                                       { @$ = @1; $$ = ChrBaseType; };
BaseType      : "bool"                                      { @$ = @1; $$ = BoolBaseType; };
BaseType      : "void"                                      { @$ = @1; $$ = VoidBaseType; };

Literal       : INTLIT_TOK                                  { @$ = @1; $$ = MakeLiteralDesc(yytext,IntBaseType); };
Literal       : CHRLIT_TOK                                  { @$ = @1; $$ = MakeLiteralDesc(yytext,ChrBaseType); };
Literal       : BOOLLIT_TOK                                 { @$ = @1; $$ = MakeLiteralDesc(yytext,BoolBaseType); };

FuncDecl      : '(' ')' "->" BaseType                       { $$ = VoidBaseType; };

CodeBlock     : '{' StmtSeq '}'                             { $$ = $2; };

StmtSeq       : Stmt StmtSeq                                { $$ = AppendSeq($1,$2); };
StmtSeq       :                                             { $$ = NULL; };

Stmt          : AssignStmt                                  { $$ = $1; };
Stmt          : PutStmt                                     { $$ = $1; };
Stmt          : IfStmt                                      { $$ = $1; };
Stmt          : ElseStmt                                    { $$ = $1; };
Stmt          : WhileStmt                                   { $$ = $1; };
Stmt          : FuncStmt                                    { $$ = $1; };

FuncStmt      : Id '(' ')'                                  { $$ = CallFunc($1); };

AssignStmt    : Id '=' Expr                                 { $$ = Assign($1,$3); };

PutStmt       : PUT_TOK '(' Expr ')'                        { $$ = Put($3); };
PutStmt       : PUT_TOK '(' String ')'                      { $$ = PutStr($3); };

String        : STRLIT_TOK                                  { $$ = LoadString(yytext); };

IfStmt        : IF_TOK Cond CodeBlock                       { $$ = If($2,$3); };

ElseStmt      : IF_TOK Cond CodeBlock ELSE_TOK CodeBlock    { $$ = Else($2,$3,$5); };

WhileStmt     : WHILE_TOK Cond CodeBlock                    { $$ = While($2,$3); };

Cond          : Expr BGE_TOK Expr                           { $$ = BGE($1,0,$3); };
Cond          : Expr BG_TOK Expr                            { $$ = BG($1,1,$3); };
Cond          : Expr BLE_TOK Expr                           { $$ = BLE($1,2,$3); };
Cond          : Expr BL_TOK Expr                            { $$ = BL($1,3,$3); };
Cond          : Expr BNE_TOK Expr                           { $$ = BNE($1,4,$3); };
Cond          : Expr BEQ_TOK Expr                           { $$ = BEQ($1,5,$3); };

Expr          : Expr '+' Expr                               { $$ = AddOp($1,$3); };
Expr          : Expr '-' Expr                               { $$ = SubOp($1,$3); };
Expr          : Expr '*' Expr                               { $$ = MultOp($1,$3); };
Expr          : Expr '/' Expr                               { $$ = DivOp($1,$3); };
Expr          : '-' Expr                       %prec UMINUS { $$ = NegOp($2); };
Expr          : Expr "++"                                   { $$ = Increment($1); };
Expr          : Expr "--"                                   { $$ = Decrement($1); };
Expr          : '(' Expr ')'                                { $$ = $2; };
Expr          : Oprnd                                       { $$ = $1; };
Expr          : GET_TOK '(' BaseType ')'                    { $$ = Get($3); };

Oprnd         : Literal                                     { $$ = LoadLiteral($1); };
Oprnd         : Id                                          { $$ = LoadId($1); };

%%

void
yyerror( char *s)     {
  char * buf = malloc(strlen(s) + strlen(yytext) + 20 + 1);
  sprintf(buf,"ERROR: \"%s\" token: \"%s\"",s,yytext);
  PostMessage(LastTokenSpan(),buf);
  free(buf);
}
