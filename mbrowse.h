#include "list.h"

struct fileListItem_s
{
  char* fullName; //Full path
  char* name; //Shown in list
  int dir;
};

typedef struct fileListItem_s fileListItem_t;

void fileListFree();
void fileListMake(const char* path);
listItem* fileList();
