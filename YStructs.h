/* Author:      Jackson Lee
   Created:     05/15/19
   Resources:   Dr. Steven Senger
                Kailey Linn
*/

/* YStructs.h
   Semantics struct definitions and utility functions
 */

#ifndef YSTRUCTS_H
#define YSTRUCTS_H

#include "SymTab.h"
#include "IOMngr.h"

static char * BaseTypeNames[5] = { "int", "chr", "bool", "void", "String"};
static char * TypeNames[3] = { "unkn", "prim", "func"};
static char * Branches[6] = {"bge", "bg", "ble", "bl", "bne", "beq"};

enum AttrKinds { VOID_KIND = -1, INT_KIND, STRING_KIND, STRUCT_KIND };
enum Operators { Add, Sub, Div, Mult };
enum BaseTypes { IntBaseType, ChrBaseType, BoolBaseType, VoidBaseType, StringBaseType };
enum BoolOps   { GreaterEqual, GreaterThan, LessEqual, LessThan, NotEqual, EqualTo };

struct StringLit {
  struct InstrSeq * codeSeq;
  char * text;
};

struct CondResult {
  struct InstrSeq * codeSeq;
  char * label;
};

struct ExprResult {
  struct InstrSeq * codeSeq;
  int regNum;
  enum BaseTypes type;
};

/* Declaration of semantic record data types used in grammar.y %union */
struct IdList {
  struct SymEntry * entry;
  struct Span span;
  struct IdList * next;
};

struct FuncDesc {
  enum BaseTypes returnType;
  struct InstrSeq * funcCode;
};

struct PrimDesc {
  enum BaseTypes type;
  int initialValue;           // only meaningful with desc of identifier
};

enum DeclTypes { UnknownType, PrimType, FuncType };
struct TypeDesc {
  enum DeclTypes declType;
  union {
    struct PrimDesc primDesc;
    struct FuncDesc funcDesc;
  };
};

 struct LiteralDesc {
   enum BaseTypes baseType;
   int value;
 };

 // Symbol Table Structures
 struct Attr {
   struct TypeDesc * typeDesc;
   char * reference;
   struct Span declSpan;
};

// Make and Free Structs
struct StringLit *     MakeStringLit(struct InstrSeq * codeSeq, char * text);
struct CondResult *    MakeCondResult(struct InstrSeq * codeSeq, char * label);
struct ExprResult *    MakeExprResult(struct InstrSeq * codeSeq, int regNum, enum BaseTypes type);
struct IdList *        MakeIdList(struct SymEntry * entry, struct Span span);
struct TypeDesc *      MakePrimDesc(enum BaseTypes type, int initialValue);
struct TypeDesc *      MakeFuncDesc(enum BaseTypes returnType);
struct LiteralDesc *   MakeLiteralDesc(char * text, enum BaseTypes type);
struct Attr *          MakeAttr(struct TypeDesc * typeDesc, char * reference, struct Span span);

void                   FreeStringLit(struct StringLit * s);
void                   FreeCondResult(struct CondResult * result);
void                   FreeExprResult(struct ExprResult * result);
void                   FreeIdList(struct IdList * item);
void                   FreeTypeDesc(struct TypeDesc * typeDesc);
void                   FreeLiteralDesc(struct LiteralDesc * literalDesc);
void                   FreeAttr(struct Attr * attr);

// Supporting Functions
char *                AppendStr(const char * str1, const char * str2);

// Message Posting Function
void PostMessageAndExit(struct Span span, char * message);
void CondPostMessageAndExit(bool cond, struct Span span, char * message);

struct Span LastTokenSpan();

// Symbol Table Functions
void DisplaySymbolTable(struct SymTab * table);

#endif
