/* Author:      Jackson Lee
   Created:     02/27/19
   Resources:   Dr. Steven Senger
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

#include "IOMngr.h"

int sourceFD;
char * source;
size_t sourceByteCnt;
char * sourceLastChar;
char * nextChar;        // address of next char to return
int curLine;
int nextMessageNumber;
int nextEOFSpan;

struct Message {
  struct Span span;
  int seqNumber;
  char * message;
  struct Message * next;
};

struct Message * Messages;

struct Message *
MakeMessage(struct Span span, int seq, const char * message) {
  struct Message * msg = malloc(sizeof(struct Message));
  msg->span = span;
  msg->seqNumber = seq;
  msg->message = strdup(message);
  msg->next = NULL;
  return msg;
}

void
FreeHeadMessage() {
  struct Message * head = Messages;

  if(!head) return;

  Messages = head->next;
  free(head->message);
  free(head);

}

bool
isLineBreak(char * c) {
  return *c == '\n' || *c == '\r' || c == sourceLastChar;
}

// ansi escape sequences for colors
char * colorSeqs[] = {"\033[91m","\033[92m","\033[93m","\033[94m","\033[95m","\033[96m"};

void
OutputMarkStart(struct Message * msg) {
  fprintf(stdout,"%s",colorSeqs[(msg->seqNumber % 6)]);
}

void
OutputMarkStop() {
  fprintf(stdout,"\033[0m");
}

void
OutputInterval(char * start, char * stop) {
  if (stop < start) return;
  fwrite(start,stop - start + 1,1,stdout);
}

void
OutputMessagesBefore(struct Message * curMsg) {
  while(Messages != curMsg) {
    printf("%8s", "");
    OutputMarkStart(Messages);
    printf("%s", "**");
    OutputMarkStop();
    printf("%s", " ");
    printf("%s\n", Messages->message);
    FreeHeadMessage();
  }
}

void
OutputSource() {
  struct Message * msg = Messages;

  if(!msg) {
    return;
  }

  int highlighting = 0;
  nextChar = source;
  curLine = 1;
  printf("%6d: ", curLine);

  while(nextChar <= sourceLastChar) {
    if(msg->span.first == GetLastPosition() + 1) {
      OutputMarkStart(msg);
      highlighting = 1;
    }

    if(msg->span.last == GetLastPosition()) {
      OutputMarkStop();
      highlighting = 0;

      if(msg->next) {
        msg = msg->next;

        if(msg->span.first == GetLastPosition() + 1) {
          OutputMarkStart(msg);
          highlighting = 1;
        }
      }
    }

    if(isLineBreak(nextChar) && highlighting == 1) {
      OutputMarkStop();
    }

    OutputInterval(nextChar, nextChar);

    if(isLineBreak(nextChar)) {
      OutputMessagesBefore(msg);

      if(nextChar != sourceLastChar) {
        curLine++;
        printf("%6d: ", curLine);
      }

      if(highlighting == 1) {
        OutputMarkStart(msg);
      }
    }

    nextChar++;
  }

  while(Messages) {
    printf("%8s", "");
    OutputMarkStart(Messages);
    printf("%s", "**");
    OutputMarkStop();
    printf("%s", " ");
    printf("%s\n", Messages->message);
    FreeHeadMessage();
  }
}

bool
OpenSource(const char * aFilename) {
  sourceFD = open(aFilename,O_RDONLY);
  if (sourceFD < 0) return false;
  struct stat buf;
  if (fstat(sourceFD,&buf)) return false;
  sourceByteCnt = buf.st_size;
  source = mmap(NULL,sourceByteCnt,PROT_READ,MAP_PRIVATE,sourceFD,0);
  nextChar = source;
  sourceLastChar = source + sourceByteCnt - 1;
  nextEOFSpan = sourceByteCnt + 1;
  return true;
}

void
CloseSource() {
  OutputSource();
  munmap(source,sourceByteCnt);
}

int
GetSourceChar() {
  int c;

  if(nextChar <= sourceLastChar) {
    c = *nextChar;
    *nextChar++;
  } else {
    c = -1;
  }

  return c;
}

bool
PostMessage(struct Span span, const char * aMessage) {
  int seqNum = 0;

  for(int i = 0; i < strlen(aMessage); i++) {
    seqNum += aMessage[i];
  }

  struct Message * newMessage = MakeMessage(span, seqNum, aMessage);

  if(!Messages) {
    Messages = newMessage;
    return true;
  }

  struct Message * msg = Messages;
  int start = span.first;
  int finish = span.last;

  if(finish < msg->span.first) {
    newMessage->next = msg;
    Messages = newMessage;

    return true;
  }

  while(msg) {
    if(msg->next) {
      struct Message * nextMsg = msg->next;
      if(msg->span.last < start && finish < nextMsg->span.first) {
        newMessage->next = nextMsg;
        msg->next = newMessage;
        return true;
      }
    } else {
      if(msg->span.last < start) {
        msg->next = newMessage;
        return true;
      }
    }

    msg = msg->next;
  }

  return false;
}

int
GetLastPosition() {
  return nextChar - 1 - source;
}

struct Span
MakeSpan(int first, int last) {
  struct Span span = {first, last};
  return span;
}

struct Span
MakeSpanFromLength(int start, int length) {
  struct Span span = {start,start + length - 1};
  return span;
}

struct Span
MakeEOFSpan() {
  struct Span span = {nextEOFSpan,nextEOFSpan};
  nextEOFSpan++;
  return span;
}
