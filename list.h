#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED

struct listItemStruct
{
  void* data;
  struct listItemStruct* next;
};

typedef struct listItemStruct listItem;

void listAddData(listItem* start, void* data);
void listInsertData(listItem* start, void* data, int p); //Inserts into the list at pos p. 0 = first

listItem* listRemoveItem(listItem* start, listItem* item); //Removes item from list, returns item just before removed item.
listItem* initList();
void freeList(listItem* start);
int listSize(listItem* start);

void* listGetItemData(listItem* start, int index);


#endif // LIST_H_INCLUDED
