/* Main of Parser */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Scanner.h"
#include "IOMngr.h"
#include "Grammar.h"

int
main(int argc, char * argv[])
{
  if (argc != 2) {
    fprintf(stderr,"usage: Parse filename\n");
    return 1;
  }

  if (!OpenSource(argv[1])) {
    fprintf(stderr,"Source File Not Found.\n");
    return 1;
  }

  if (yyparse() == 0) {
    PostMessage(MakeEOFSpan(),"Parse Successful");
  }
  else {
    PostMessage(MakeEOFSpan(),"Parse Unsuccessful");
  }

  CloseSource();

  return 0;
}
