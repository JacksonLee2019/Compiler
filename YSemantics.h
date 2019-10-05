/* Author:      Jackson Lee
   Created:     05/15/19
   Resources:   Dr. Steven Senger
                Kailey Linn
*/

/* Semantics.h
 The action and supporting routines for performing semantics processing.
 */

#include "SymTab.h"
#include "IOMngr.h"
#include "YCodeGen.h"
#include "YStructs.h"

// Supporting Routines

void InitSemantics();
void FinishSemantics();

// Semantics Actions

// Step 1
void                    ProcDecl(struct IdList * idList, enum BaseTypes baseType, int initialValue);
void                    ProcDeclFunc(struct IdList * idList, enum BaseTypes type);
void                    ProcFuncBody(struct IdList * idItem, struct InstrSeq * codeBlock);
struct IdList *         AppendIdList(struct IdList * item, struct IdList * list);
struct IdList *         ProcName(char * tokenText, struct Span span);
struct InstrSeq *       Assign(char * id, struct ExprResult * codeSeq);
struct ExprResult *     AddOp(struct ExprResult * expr1, struct ExprResult * expr2);
struct ExprResult *     SubOp(struct ExprResult * expr1, struct ExprResult * expr2);
struct ExprResult *     MultOp(struct ExprResult * expr1, struct ExprResult * expr2);
struct ExprResult *     DivOp(struct ExprResult * expr1, struct ExprResult * expr2);
struct ExprResult *     NegOp(struct ExprResult * expr);
struct ExprResult *     Increment(struct ExprResult * expr);
struct ExprResult *     Decrement(struct ExprResult * expr);
struct InstrSeq *       Put(struct ExprResult * expr);
struct InstrSeq *       PutStr(char * text);
struct ExprResult *     Get(int type);
struct ExprResult *     LoadLiteral(struct LiteralDesc * lit);
struct ExprResult *     LoadId(char * text);
char *                  LoadString(char * text);
struct InstrSeq *       CallFunc(char * f);
struct InstrSeq *       If(struct CondResult * result, struct InstrSeq * codeSeq);
struct InstrSeq *       Else(struct CondResult * result, struct InstrSeq * instr1, struct InstrSeq * instr2);
struct InstrSeq *       While(struct CondResult * result, struct InstrSeq * codeSeq);
struct CondResult *     BGE(struct ExprResult * expr1, enum BoolOps boolOp, struct ExprResult * expr2);
struct CondResult *     BG(struct ExprResult * expr1, enum BoolOps boolOp, struct ExprResult * expr2);
struct CondResult *     BLE(struct ExprResult * expr1, enum BoolOps boolOp, struct ExprResult * expr2);
struct CondResult *     BL(struct ExprResult * expr1, enum BoolOps boolOp, struct ExprResult * expr2);
struct CondResult *     BNE(struct ExprResult * expr1, enum BoolOps boolOp, struct ExprResult * expr2);
struct CondResult *     BEQ(struct ExprResult * expr1, enum BoolOps boolOp, struct ExprResult * expr2);
