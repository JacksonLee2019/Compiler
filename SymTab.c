/* Author:      Jackson Lee
   Created:     02/18/19
   Resources:   Dr. Steven Senger
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

#include "SymTab.h"

/* The symbol table entry structure proper.
*/
struct SymEntry {
  struct SymTab * table;
  char * name;
  int attrKind;
  void * attributes;
  struct SymEntry * next;
};

/* The symbol table structure proper. The hash table array Contents
   is dynamically allocated according to size
*/
struct SymTab {
  struct SymTab * parent;
  char * scopeName;
  int size;
  int count;
  struct SymEntry ** contents;
};

struct SymTab *
CreateSymTab(int size, char * scopeName, struct SymTab * parentTable) {
  struct SymTab * table = malloc(sizeof(struct SymTab));

  if(parentTable) {
    table->parent = parentTable;
  } else {
    table->parent = NULL;
  }

  if(!scopeName) {
    table->scopeName = NULL;
  } else {
    table->scopeName = strdup(scopeName);
  }

  table->size = size;
  table->count = 0;
  table->contents = malloc(sizeof(struct SymEntry *) * size);

  for(int i =0; i < size; i++) {
    table->contents[i] = NULL;
  }

  return table;
}

struct SymTab *
DestroySymTab(struct SymTab *aTable) {
  for(int i = 0; i < aTable->size; i++) {
    while(aTable->contents[i]) {
      free(aTable->contents[i]->name);
      struct SymEntry * nextEntry = aTable->contents[i]->next;
      free(aTable->contents[i]);
      aTable->contents[i] = nextEntry;
    }
  }

  free(aTable->contents);
  free(aTable->scopeName);
  struct SymTab * table = aTable->parent;
  free(aTable);

  return table;
}

unsigned int
HashName(int size, const char *name) {
  int hashValue = 0;

  for(int i = 0; i < strlen(name); i++) {
    hashValue += name[i];
  }

  hashValue = hashValue%size;

  return hashValue;
}

struct SymEntry *
FindHashedName(struct SymTab *aTable, int hashValue, const char *name) {
  struct SymTab * table = aTable;
  struct SymEntry * entry = table->contents[hashValue];

  while(entry) {
    if(strcmp(entry->name, name) == 0) {
      return entry;
    }

    entry = entry->next;
  }

  return NULL;
}

struct SymEntry *
LookupName(struct SymTab *aTable, const char * name) {
  struct SymTab * table = aTable;
  int hashValue;
  struct SymEntry * entry;

  while(table) {
    hashValue = HashName(table->size, name);
    entry = FindHashedName(table, hashValue, name);

    if(entry) {
      return entry;
    }

    table = table->parent;
  }

  return NULL;
}

struct SymEntry *
EnterName(struct SymTab *aTable, const char *name) {
  int hashValue = HashName(aTable->size, name);
  struct SymEntry * entry = FindHashedName(aTable, hashValue, name);

  if(entry) {
    return entry;
  }

  entry = malloc(sizeof(struct SymEntry));
  entry->table = aTable;

  if(name) {
    entry->name = strdup(name);
  } else {
    entry->name = NULL;
  }

  entry->attrKind = 0;
  entry->attributes = NULL;

  if(aTable->contents[hashValue]) {
    entry->next = aTable->contents[hashValue];
  } else {
    entry->next = NULL;
  }

  aTable->contents[hashValue] = entry;
  aTable->count++;

  return entry;
}

void
SetAttr(struct SymEntry *anEntry, int kind, void *attributes) {
  anEntry->attrKind = kind;
  anEntry->attributes = attributes;
}

int
GetAttrKind(const struct SymEntry *anEntry) {
  if(!anEntry) {
    return NULL;
  } else {
    return anEntry->attrKind;
  }
}

void *
GetAttr(const struct SymEntry *anEntry) {
  if(!anEntry) {
    return NULL;
  } else {
    return anEntry->attributes;
  }
}

const char *
GetName(const struct SymEntry *anEntry) {
  if(!anEntry) {
    return NULL;
  } else {
    return anEntry->name;
  }
}

struct SymTab *
GetTable(const struct SymEntry *anEntry) {
  if(!anEntry) {
    return NULL;
  } else {
    return anEntry->table;
  }
}

const char *
GetScopeName(const struct SymTab *aTable) {
  if(!aTable) {
    return NULL;
  } else {
    return aTable->scopeName;
  }
}

char *
GetScopeNamePath(const struct SymTab *aTable) {
  if(!aTable) {
    return strdup("");
  }

  char * path = GetScopeNamePath(aTable->parent);
  char * result;
  if(!aTable->scopeName) {
    result = malloc(strlen(path) + 2);
    strcpy(result, path);

    if(aTable->parent) {
      char * separator = ">";
      result = strcat(result, separator);
    }

    result = strcat(result, "");
  } else {
    result = malloc(strlen(aTable->scopeName) + strlen(path) + 2);
    strcpy(result, path);

    if(aTable->parent) {
      char * separator = ">";
      result = strcat(result, separator);
    }

    result = strcat(result, aTable->scopeName);
  }

  free(path);
  return result;
}

struct SymTab *
GetParentTable(const struct SymTab *aTable) {
  if(!aTable) {
    return NULL;
  } else {
    return aTable->parent;
  }
}

struct SymEntry ** entryArray = NULL;
int entryArraySize = 0;

void ProvisionArray(struct SymTab * aTable, bool includeParents) {
  struct SymTab * table = aTable;
  int reqSize = 0;

  if(!includeParents) {
    reqSize = aTable->count;
  } else {
    while(table) {
      reqSize += table->count;
      table = table->parent;
    }
  }
  reqSize++;
  if (entryArraySize < reqSize) {
    entryArray = realloc(entryArray,reqSize * sizeof(struct SymEntry *));
    entryArraySize = reqSize;
  }
}

void
CollectEntries(struct SymTab * aTable, bool includeParents, entryTestFunc testFunc) {
  struct SymTab * table = aTable;
  struct SymEntry * entry;
  int count = 0;

  while(table) {
    for(int i = 0; i < table->size; i++) {
      entry = table->contents[i];

      while(entry) {
        if(!testFunc) {
          entryArray[count] = entry;
          count++;
        } else {
          if(testFunc(entry)) {
            entryArray[count] = entry;
            count++;
          }
        }

        entry = entry->next;
      }
    }

    if(includeParents) {
      table = table->parent;
    } else {
      entryArray[count] = NULL;
      return;
    }
  }

  entryArray[count] = NULL;
}

struct SymEntry **
GetEntries(struct SymTab * aTable, bool includeParents, entryTestFunc testFunc) {
  ProvisionArray(aTable, includeParents);
  CollectEntries(aTable, includeParents, testFunc);

  return entryArray;
}

struct Stats *
Statistics(struct SymTab *aTable) {
  struct Stats * stats = malloc(sizeof(struct Stats));
  struct SymTab * table = aTable;
  struct SymEntry * entry;
  int size = table->size;
  int * chainLens = malloc(sizeof(int) * size);
  int max = INT_MIN;
  int min = INT_MAX;
  int total = 0;
  int count = 0;
  int tempCount = 0;

  for(int i = 0; i < size; i++) {
    entry = table->contents[i];

    while(entry) {
      count++;
      tempCount++;
      entry = entry->next;
    }

    chainLens[i] = tempCount;
    total += tempCount;
    if(tempCount > max) {
      max = tempCount;
    }

    if(tempCount < min) {
      min = tempCount;
    }

    tempCount = 0;
  }

  stats->minLen = min;
  stats->maxLen = max;
  stats->avgLen = total/size;
  stats->entryCnt = count;

  return stats;
}
