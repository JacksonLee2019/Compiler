/* Author:      Jackson Lee
   Created:     05/15/19
   Resources:   Dr. Steven Senger
                Kailey Linn
*/

/* YStructs.c
Semantics struct definitions and utility functions
*/

#include <stdlib.h>
#include <string.h>
#include "IOMngr.h"
#include "Scanner.h"
#include "YCodeGen.h"
#include "YStructs.h"


// Make and Free Structs
struct StringLit *
MakeStringLit(struct InstrSeq * codeSeq, char * text) {
  struct StringLit * s = malloc(sizeof(struct StringLit));
  s->codeSeq = codeSeq;
  s->text = text;
  return s;
}

struct CondResult *
MakeCondResult(struct InstrSeq * codeSeq, char * label) {
  struct CondResult * result = malloc(sizeof(struct CondResult));
  result->codeSeq = codeSeq;
  result->label = label;
  return result;
}

struct ExprResult *
MakeExprResult(struct InstrSeq * codeSeq, int regNum, enum BaseTypes type) {
  struct ExprResult * result = malloc(sizeof(struct ExprResult));
  result->codeSeq = codeSeq;
  result->regNum = regNum;
  result->type = type;
  return result;
}

struct IdList *
MakeIdList(struct SymEntry * entry, struct Span span) {
  struct IdList * item = malloc(sizeof(struct IdList));
  item->entry = entry;
  item->span = span;
  item->next = NULL;
  return item;
}

struct TypeDesc *
MakePrimDesc(enum BaseTypes type, int initialValue) {
  struct TypeDesc * desc = malloc(sizeof(struct TypeDesc));
  desc->declType = PrimType;
  desc->primDesc.type = type;
  desc->primDesc.initialValue = initialValue;
  return desc;
}

struct TypeDesc *
MakeFuncDesc(enum BaseTypes returnType) {
  struct TypeDesc * desc = malloc(sizeof(struct TypeDesc));
  desc->declType = FuncType;
  desc->funcDesc.returnType = returnType;
  desc->funcDesc.funcCode = NULL;
  return desc;
}

struct LiteralDesc *
MakeLiteralDesc(char * text, enum BaseTypes type) {
  struct LiteralDesc * desc = malloc(sizeof(struct LiteralDesc));
  desc->baseType = type;
  switch (desc->baseType) {
    case IntBaseType:
      desc->value = atoi(text);
      break;
    case ChrBaseType:
      if(text[2] == 'n') {
        desc->value = 10;
      } else {
        desc->value = (int)text[1];
      }
      break;
    case BoolBaseType: {
      if(text[0] == 't') {
        desc->value = 1;
      } else {
        desc->value = 0;
      }
    } break;
    case VoidBaseType: {
    } break;
  }
  return desc;
}

struct Attr *
MakeAttr(struct TypeDesc * typeDesc, char * reference, struct Span span) {
  struct Attr * attr = malloc(sizeof(struct Attr));
  attr->typeDesc = typeDesc;
  attr->reference = reference;
  attr->declSpan = span;
  return attr;
}

void
FreeStringLit(struct StringLit * s) {
  free(s->text);
  free(s);
}

void
FreeCondResult(struct CondResult * result) {
  free(result->label);
  free(result);
}

void
FreeExprResult(struct ExprResult * result) {
    free(result);
}

void
FreeIdList(struct IdList * item) {
  free(item);
}

void
FreeTypeDesc(struct TypeDesc * typeDesc) {
  switch (typeDesc->declType) {
    case UnknownType: {
    } break;
    case PrimType: {
    } break;
    case FuncType: {
      if (typeDesc->funcDesc.funcCode) FreeSeq(typeDesc->funcDesc.funcCode);
    } break;
  }
  free(typeDesc);
}

void
FreeLiteralDesc(struct LiteralDesc * literalDesc) {
  free(literalDesc);
}

void
FreeAttr(struct Attr * attr) {
  FreeTypeDesc(attr->typeDesc);
  free(attr->reference);
  free(attr);
}

// Supporting Routines
char *
AppendStr(const char * str1, const char * str2) {
  char * str = malloc(strlen(str1) + strlen(str2) + 1);
  strcpy(str,str1);
  strcat(str,str2);
  return str;
}

void
PostMessageAndExit(struct Span span, char * message) {
  PostMessage(span,message);
  CloseSource();
  //ListIdentifierTable();
  exit(0);
}

void
CondPostMessageAndExit(bool cond, struct Span span, char * message) {
  if (!cond) return;
  PostMessageAndExit(span, message);
  CloseSource();
  //ListIdentifierTable();
  exit(0);
}

struct Span
LastTokenSpan() {
  struct Span span = MakeSpanFromLength(tokenStartPosition,yyleng);
  return span;
}

void
DisplayType(struct TypeDesc * desc) {
  switch (desc->declType) {
    case UnknownType: {
      printf("unkn");
    } break;
    case PrimType: {
      printf("%-4s",BaseTypeNames[desc->primDesc.type]);
      printf("  %8d",desc->primDesc.initialValue);
    } break;
    case FuncType: {
      printf("() -> %-4s",BaseTypeNames[desc->funcDesc.returnType]);
    } break;
  }
}

void
DisplayEntries(struct SymEntry ** entries) {
  struct SymEntry ** p = entries;
  int cnt = 0;
  while (*p) {
    struct SymEntry * entry = *p;
    char * scope = GetScopeNamePath(GetTable(entry));
    printf("%3d %-20s %-15s",cnt,scope,GetName(entry));
    free(scope);
    int attrKind = GetAttrKind(entry);
    struct Attr * attr = GetAttr(entry);
    switch (attrKind) {
      case VOID_KIND:
      case INT_KIND:
      case STRING_KIND:
        // ignore these cases
      break;
      case STRUCT_KIND: {
        if (attr) {
          printf("%-10s",attr->reference);
          printf("     ");
          struct TypeDesc * desc = attr->typeDesc;
          DisplayType(desc);
        }
        else {
          printf("empty");
        }
      } break;
    }
    printf("\n");
    p++;
    cnt++;
  }
}

void
DisplaySymbolTable(struct SymTab * table) {
  printf("\nSymbol Table: %s\n",GetScopeName(table));
  printf("Num Scope                Name           Ref            Type\n");
  DisplayEntries(GetEntries(table,true,NULL));
}
