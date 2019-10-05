/* Author:      Jackson Lee
   Created:     05/15/19
   Resources:   Dr. Steven Senger
                Kailey Linn
*/

/* Semantics.c
 Support and semantic action routines.

 */

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "Scanner.h"
#include "YStructs.h"
#include "YSemantics.h"

// Shared Data

struct SymTab * IdentifierTable;
struct SymTab * StringTable;
// corresponds to enum Operators
char * Ops[] = { "add", "sub", "mul", "div"};
// enum Comparisons { LtCmp, LteCmp, GtCmp, GteCmp, EqCmp, NeqCmp };
// comparison set instructions, in order with enum Comparisons
char * CmpSetReg[] = { "slt", "sle", "sgt", "sge", "seq", "sne" };

void
InitSemantics() {
  IdentifierTable = CreateSymTab(100,"global",NULL);
  StringTable = CreateSymTab(100, "global", NULL);
}

bool
selectPrimType(struct SymEntry * entry) {
  struct Attr * attr = GetAttr(entry);
  if(!attr->typeDesc) {
    return false;
  }
  return attr->typeDesc->declType == PrimType;
}

void
processStrings(void * dataCode) {
  struct SymEntry ** p = GetEntries(StringTable,false,NULL);
  while (*p) {
    const struct SymEntry * entry = *p;
    struct StringLit * attr = GetAttr(entry);
    AppendSeq(dataCode, attr->codeSeq);
    FreeStringLit(attr);
    p++;
  }
}

void
processGlobalIdentifier(void * dataCode) {
  struct SymEntry ** p = GetEntries(IdentifierTable,false,selectPrimType);
  while (*p) {
    const struct SymEntry * entry = *p;
    struct Attr * attr = GetAttr(entry);
    // gen code here
    int initValue = attr->typeDesc->primDesc.initialValue;
    AppendSeq(dataCode, GenLabelOp1(attr->reference, ".word", strdup(IntLit(initValue))));
    p++;
  }
}

bool
selectFuncType(struct SymEntry * entry) {
  struct Attr * attr = GetAttr(entry);
  if(!attr->typeDesc) {
    return false;
  }
  return attr->typeDesc->declType == FuncType;
}

void
processFunctions(void * textCode) {
  // need to implement
  struct SymEntry ** p = GetEntries(IdentifierTable,false,selectFuncType);
  while (*p) {
    const struct SymEntry * entry = *p;
    struct Attr * attr = GetAttr(entry);
    AppendSeq(textCode, attr->typeDesc->funcDesc.funcCode);
    p++;
  }
}

// Semantics Actions

void
FinishSemantics() {
  // build text segment
  struct InstrSeq * textCode = GenOpX(".text");

  // form module preamble
  struct SymEntry * mainEntry = LookupName(IdentifierTable,"main");
  CondPostMessageAndExit(!mainEntry,MakeEOFSpan(),"no main function for module");

  // should make sure main takes no arguments
  struct Attr * mainAttr = GetAttr(mainEntry);

  // boilerplate for spim
  AppendSeq(textCode,GenOp1X(".globl","__start"));
  AppendSeq(textCode,GenLabelX("__start"));

  AppendSeq(textCode,GenOp1X("jal",mainAttr->reference));
  AppendSeq(textCode,GenOp2X("li","$v0","10"));
  AppendSeq(textCode,GenOpX("syscall"));

  // append code for all Functions
  processFunctions(textCode);

  // build data segment
  struct InstrSeq * dataCode = GenOpX(".data");
  processGlobalIdentifier(dataCode);
  processStrings(dataCode);
  // combine and write
  struct InstrSeq * moduleCode = AppendSeq(textCode,dataCode);
  WriteSeq(moduleCode);

  // free code
  FreeSeq(moduleCode);
  CloseCodeGen();

  CloseSource();

  DisplaySymbolTable(IdentifierTable);
}

void
ProcDecl(struct IdList * idList, enum BaseTypes baseType, int initialValue) {
  // walk idList item entries
  // get entry attributes
  // check required conditions
  // make and assign primitive type descriptor
  // make and assign id reference string

  while(idList) {
    struct Attr * attr = GetAttr(idList->entry);
    attr->typeDesc = MakePrimDesc(baseType, initialValue);
    attr->reference = AppendStr("_", GetName(idList->entry));
    idList = idList->next;
  }
}

void
ProcDeclFunc(struct IdList * idList, enum BaseTypes type) {
  // walk idList item entries
  // get entry attributes
  // check required conditions
  // make and assign function descriptor
  // make and assign id reference string

  while(idList) {
    struct Attr * attr = GetAttr(idList->entry);
    attr->typeDesc = MakeFuncDesc(type);
    attr->reference = AppendStr("_", GetName(idList->entry));
    idList = idList->next;
  }
}

void
ProcFuncBody(struct IdList * idItem, struct InstrSeq * codeBlock) {
  // check single item
  // check has typeDesc, if not create one indicating void return
  // wrap codeBlock in function entry label and jump return, assign to typeDesc

  struct Attr * attr = GetAttr(idItem->entry);

  if(!attr->typeDesc) {
    attr->typeDesc = MakeFuncDesc(VoidBaseType);
  }

  attr->reference = AppendStr("_", GetName(idItem->entry));
  struct InstrSeq * instr = GenLabelC(attr->reference, "func entry");

  AppendSeq(instr, GenOp3C("subu", "$sp", "$sp", "4", "ProcFuncBody"));
  AppendSeq(instr, GenOp2C("sw", "$ra", "0($sp)", "ProcFuncBody"));
  AppendSeq(instr, codeBlock);
  AppendSeq(instr, GenLabel(AppendStr(GetName(idItem->entry), "_ret")));
  AppendSeq(instr, GenOp2C("lw", "$ra", "0($sp)", "ProcFuncBody"));
  AppendSeq(instr, GenOp3C("addu", "$sp", "$sp", "4", "ProcFuncBody"));
  AppendSeq(instr, GenOp1C("jr", "$ra", "func return"));
  attr->typeDesc->funcDesc.funcCode = instr;
}

struct IdList *
AppendIdList(struct IdList * item, struct IdList * list) {
  // chain item to list, return item

  item->next = list;

  return item;
}

struct IdList *
ProcName(char * tokenText, struct Span span) {
  // look up token text - LookupName()? where do we get the SymTab? GlobalIdentifierTable
  // check conditions
  // enter token text - EnterName(GlobalIdentifierTable, tokenText);
  // create struct IdList * item = MakeIdList(entry, span);
  // create and init attribute struct -

  struct SymEntry * entry = LookupName(IdentifierTable, tokenText);

  if(!entry) {
    entry = EnterName(IdentifierTable, tokenText);
  }

  struct Attr * attr = MakeAttr(NULL, NULL, span);
  SetAttr(entry, STRUCT_KIND, attr);

  return MakeIdList(entry, span);
}

struct InstrSeq *
Assign(char * id, struct ExprResult * result) {
  struct InstrSeq * codeSeq = result->codeSeq;
  AppendSeq(codeSeq, GenOp2("sw", TmpRegName(result->regNum), AppendStr("_", id)));
  ReleaseTmpReg(result->regNum);
  free(id);
  return codeSeq;
}

struct ExprResult *
Increment(struct ExprResult * expr) {
  struct ExprResult * result = MakeExprResult(NULL, AvailTmpReg(), IntBaseType);
  struct InstrSeq * instr = expr->codeSeq;
  AppendSeq(instr, GenOp3("addi", TmpRegName(result->regNum), TmpRegName(expr->regNum), "1"));
  result->codeSeq = instr;

  ReleaseTmpReg(expr->regNum);
  return result;
}

struct ExprResult *
Decrement(struct ExprResult * expr) {
  struct ExprResult * result = MakeExprResult(NULL, AvailTmpReg(), IntBaseType);
  struct InstrSeq * instr = expr->codeSeq;
  AppendSeq(instr, GenOp3("addi", TmpRegName(result->regNum), TmpRegName(expr->regNum), "-1"));
  result->codeSeq = instr;

  ReleaseTmpReg(expr->regNum);
  return result;
}

struct ExprResult *
AddOp(struct ExprResult * expr1, struct ExprResult * expr2) {
  struct ExprResult * result = MakeExprResult(NULL, AvailTmpReg(), IntBaseType);
  struct InstrSeq * instr = AppendSeq(expr1->codeSeq, expr2->codeSeq);
  AppendSeq(instr, GenOp3("add", TmpRegName(result->regNum), TmpRegName(expr1->regNum), TmpRegName(expr2->regNum)));
  result->codeSeq = instr;

  ReleaseTmpReg(expr1->regNum);
  ReleaseTmpReg(expr2->regNum);
  return result;
}

struct ExprResult *
SubOp(struct ExprResult * expr1, struct ExprResult * expr2) {
  struct ExprResult * result = MakeExprResult(NULL, AvailTmpReg(), IntBaseType);
  struct InstrSeq * instr = AppendSeq(expr1->codeSeq, expr2->codeSeq);
  AppendSeq(instr, GenOp3("sub", TmpRegName(result->regNum), TmpRegName(expr1->regNum), TmpRegName(expr2->regNum)));
  result->codeSeq = instr;

  ReleaseTmpReg(expr1->regNum);
  ReleaseTmpReg(expr2->regNum);
  return result;
}

struct ExprResult *
MultOp(struct ExprResult * expr1, struct ExprResult * expr2) {
  struct ExprResult * result = MakeExprResult(NULL, AvailTmpReg(), IntBaseType);
  struct InstrSeq * instr = AppendSeq(expr1->codeSeq, expr2->codeSeq);
  AppendSeq(instr, GenOp3("mul", TmpRegName(result->regNum), TmpRegName(expr1->regNum), TmpRegName(expr2->regNum)));
  result->codeSeq = instr;

  ReleaseTmpReg(expr1->regNum);
  ReleaseTmpReg(expr2->regNum);
  return result;
}

struct ExprResult *
DivOp(struct ExprResult * expr1, struct ExprResult * expr2) {
  struct ExprResult * result = MakeExprResult(NULL, AvailTmpReg(), IntBaseType);
  struct InstrSeq * instr = AppendSeq(expr1->codeSeq, expr2->codeSeq);
  AppendSeq(instr, GenOp3("div", TmpRegName(result->regNum), TmpRegName(expr1->regNum), TmpRegName(expr2->regNum)));
  result->codeSeq = instr;

  ReleaseTmpReg(expr1->regNum);
  ReleaseTmpReg(expr2->regNum);
  return result;
}

struct ExprResult *
NegOp(struct ExprResult * expr) {
  int reg1 = AvailTmpReg();
  int reg2 = AvailTmpReg();

  AppendSeq(expr->codeSeq, GenOp2("li", TmpRegName(reg1), "-1"));
  AppendSeq(expr->codeSeq, GenOp2("mult", TmpRegName(reg2), TmpRegName(expr->regNum)));

  expr->regNum = reg2;
  ReleaseTmpReg(reg1);

  return expr;
}

struct InstrSeq *
Put(struct ExprResult * expr) {
  struct InstrSeq * instr = expr->codeSeq;

  if(expr->type == IntBaseType) {
    AppendSeq(instr, GenOp2("li", "$v0", "1"));
  } else if(expr->type == ChrBaseType) {
    AppendSeq(instr, GenOp2("li", "$v0", "11"));
  } else if(expr->type == BoolBaseType) {
    AppendSeq(instr, GenOp2("li", "$v0", "11"));
    AppendSeq(instr, GenOp3C("mul", TmpRegName(expr->regNum), TmpRegName(expr->regNum), "14", "PutExpr"));
    AppendSeq(instr, GenOp3C("add", TmpRegName(expr->regNum), TmpRegName(expr->regNum), "70", "PutExpr"));
  } else {
    AppendSeq(instr, GenOp2("li", "$v0", "1"));
  }

  AppendSeq(instr, GenOp2("move", "$a0", TmpRegName(expr->regNum)));
  AppendSeq(instr, GenOp("syscall"));

  ReleaseTmpReg(expr->regNum);
  return instr;
}

struct InstrSeq *
PutStr(char * text) {
  struct InstrSeq * instr = GenOp2("li", "$v0", "4");
  AppendSeq(instr, GenOp2("la", "$a0", text));
  AppendSeq(instr, GenOp("syscall"));

  return instr;
}

struct ExprResult *
Get(int type) {
  struct ExprResult * result = MakeExprResult(NULL, AvailTmpReg(), type);

  if(type == IntBaseType) {
    result->codeSeq = GenOp2C("li", "$v0", "5", "read int syscall");
  } else if(type == ChrBaseType) {
    result->codeSeq = GenOp2("li", "$v0", "12");
  }

  AppendSeq(result->codeSeq, GenOp("syscall"));
  AppendSeq(result->codeSeq, GenOp2("move", TmpRegName(result->regNum), "$v0"));

  return result;
}

struct ExprResult *
LoadLiteral(struct LiteralDesc * lit) {
  struct ExprResult * result = MakeExprResult(NULL, AvailTmpReg(), lit->baseType);
  result->codeSeq = GenOp2("li", TmpRegName(result->regNum), strdup(IntLit(lit->value)));

  return result;
}

struct ExprResult *
LoadId(char * text) {
  struct ExprResult * result = MakeExprResult(NULL, AvailTmpReg(), VoidBaseType);
  struct SymEntry * entry = LookupName(IdentifierTable, text);

  if(!entry) {
    return result;
  }

  struct Attr * attr = GetAttr(entry);

  result->codeSeq = GenOp2("lw", TmpRegName(result->regNum), AppendStr("_", text));

  return result;
}

char *
LoadString(char * text) {
  struct StringLit * s;
  struct SymEntry * entry = LookupName(StringTable, text);

  if(!entry) {
    entry = EnterName(StringTable, text);
    s = MakeStringLit(NULL, NewLabel());
    s->codeSeq = GenLabelOp1(s->text, ".asciiz", text);
    SetAttr(entry, StringBaseType, s);
  } else {
    s = GetAttr(entry);
  }

  return s->text;
}

struct InstrSeq *
If(struct CondResult * result, struct InstrSeq * codeSeq) {
  struct InstrSeq * instr = result->codeSeq;
  AppendSeq(instr, codeSeq);
  AppendSeq(instr, GenLabel(result->label));
  return instr;
}

struct InstrSeq *
Else(struct CondResult * result, struct InstrSeq * instr1, struct InstrSeq * instr2) {
  struct InstrSeq * instr = result->codeSeq;
  AppendSeq(instr, instr1);
  char * ifLabel = NewLabel();
  AppendSeq(instr, GenOp1C("b", ifLabel, "ProcIf"));
  AppendSeq(instr, GenLabelC(result->label, "ProcIf"));
  AppendSeq(instr, instr2);
  AppendSeq(instr, GenLabelC(ifLabel, "ProcIf"));
  free(ifLabel);
  FreeCondResult(result);
  return instr;
}

struct InstrSeq *
While(struct CondResult * result, struct InstrSeq * codeSeq) {
  char * label = NewLabel();
  struct InstrSeq * instr = GenLabelC(label, "ProcWhile");
  AppendSeq(instr, result->codeSeq);
  AppendSeq(instr, codeSeq);
  AppendSeq(instr, GenOp1C("b", label, "ProcWhile"));
  AppendSeq(instr, GenLabelC(result->label, "ProcWhile"));
  free(label);
  FreeCondResult(result);
  return instr;
}

struct CondResult *
BGE(struct ExprResult * expr1, enum BoolOps boolOp, struct ExprResult * expr2) {
  struct CondResult * result = MakeCondResult(NULL, NewLabel());
  result->codeSeq = AppendSeq(expr1->codeSeq, expr2->codeSeq);
  AppendSeq(result->codeSeq, GenOp3("blt", TmpRegName(expr1->regNum), TmpRegName(expr2->regNum), result->label));
  ReleaseTmpReg(expr1->regNum);
  ReleaseTmpReg(expr2->regNum);
  return result;
}

struct CondResult *
BG(struct ExprResult * expr1, enum BoolOps boolOp, struct ExprResult * expr2) {
  struct CondResult * result = MakeCondResult(NULL, NewLabel());
  result->codeSeq = AppendSeq(expr1->codeSeq, expr2->codeSeq);
  AppendSeq(result->codeSeq, GenOp3("ble", TmpRegName(expr1->regNum), TmpRegName(expr2->regNum), result->label));
  ReleaseTmpReg(expr1->regNum);
  ReleaseTmpReg(expr2->regNum);
  return result;
}

struct CondResult *
BLE(struct ExprResult * expr1, enum BoolOps boolOp, struct ExprResult * expr2) {
  struct CondResult * result = MakeCondResult(NULL, NewLabel());
  result->codeSeq = AppendSeq(expr1->codeSeq, expr2->codeSeq);
  AppendSeq(result->codeSeq, GenOp3("bgt", TmpRegName(expr1->regNum), TmpRegName(expr2->regNum), result->label));
  ReleaseTmpReg(expr1->regNum);
  ReleaseTmpReg(expr2->regNum);
  return result;
}

struct CondResult *
BL(struct ExprResult * expr1, enum BoolOps boolOp, struct ExprResult * expr2) {
  struct CondResult * result = MakeCondResult(NULL, NewLabel());
  result->codeSeq = AppendSeq(expr1->codeSeq, expr2->codeSeq);
  AppendSeq(result->codeSeq, GenOp3("bge", TmpRegName(expr1->regNum), TmpRegName(expr2->regNum), result->label));
  ReleaseTmpReg(expr1->regNum);
  ReleaseTmpReg(expr2->regNum);
  return result;
}

struct CondResult *
BNE(struct ExprResult * expr1, enum BoolOps boolOp, struct ExprResult * expr2) {
  struct CondResult * result = MakeCondResult(NULL, NewLabel());
  result->codeSeq = AppendSeq(expr1->codeSeq, expr2->codeSeq);
  AppendSeq(result->codeSeq, GenOp3("beq", TmpRegName(expr1->regNum), TmpRegName(expr2->regNum), result->label));
  ReleaseTmpReg(expr1->regNum);
  ReleaseTmpReg(expr2->regNum);
  return result;
}

struct CondResult *
BEQ(struct ExprResult * expr1, enum BoolOps boolOp, struct ExprResult * expr2) {
  struct CondResult * result = MakeCondResult(NULL, NewLabel());
  result->codeSeq = AppendSeq(expr1->codeSeq, expr2->codeSeq);
  AppendSeq(result->codeSeq, GenOp3("bne", TmpRegName(expr1->regNum), TmpRegName(expr2->regNum), result->label));
  ReleaseTmpReg(expr1->regNum);
  ReleaseTmpReg(expr2->regNum);
  return result;
}

struct InstrSeq *
CallFunc(char * f) {
  struct InstrSeq * instr = GenOp1X("jal", AppendStr("_",f));
  return instr;
}
