#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "list.h"


typedef struct {
  int num;
  char* name;
} item;

item* mkItem(int nu, const char* na)
{
  item* i = malloc(sizeof(item));
  i->num = nu;
  i->name = malloc(strlen(na)+1);
  strcpy(i->name, na);
  return(i);
}

void showItem(item* i )
{
  printf("    Item( %p ) -> num = %i -> name = %s\n", i, i->num, i->name );
}

void freeItem( void* data )
{
  item* i = (item*)data;
  printf("Freeing:\n");
  showItem(i);
  free(i->name);
  free(i);
}

void showItems(list_t* list)
{
  listItem* it=&list->begin;
  while( LISTFWD(list,it) )
  {
    showItem((item*)it->data);
  }
}

int main()
{
  int i;
  printf("Test: New list.\n");
  list_t* list = listInit(freeItem);
  listDebugShow(list,LIST_DEBUG_SHOW_BACKWARD|LIST_DEBUG_SHOW_FORWARD);
  listItem* it;

  printf("\nTest: Appending 4 items to list.\n");
  listAppendData(list, mkItem(1, "Fisk") );
  listAppendData(list, mkItem(2, "Hund") );
  it=listAppendData(list, mkItem(3, "Hest") );
  listAppendData(list, mkItem(4, "Rose") );
  listDebugShow(list,LIST_DEBUG_SHOW_SHORT);
  showItems(list);


  printf("\nTest: Prepending 4 items.\n");
  listPrependData(list, mkItem(5, "Laks") );
  listPrependData(list, mkItem(6, "Ko") );
  listPrependData(list, mkItem(7, "Tiger") );
  listPrependData(list, mkItem(8, "Zebra") );
  listDebugShow(list,LIST_DEBUG_SHOW_SHORT);
  showItems(list);


  printf("\nTest: Removing item (Hest) from list.\n");
  listRemoveItem(list,it, LIST_PREV);
  listDebugShow(list,LIST_DEBUG_SHOW_SHORT);
  showItems(list);


  printf("\nTest: Inserting 4 items\n");
  listInsertAtIdx(list, mkItem(1, "New Pos 0"), 0 );
  it=listInsertAtIdx(list, mkItem(1, "New Pos 2"), 2 );
  listInsertAtIdx(list, mkItem(1, "New Pos 8"), 8 );
  listInsertAtIdx(list, mkItem(list->count, "New Last Pos"), list->count );
  listDebugShow(list,LIST_DEBUG_SHOW_SHORT);
  showItems(list);


  printf("\nTest: Insert after item %p (New Pos 2)\n");
  listInsertAfterItem(list, it, mkItem(2, "New Pos 3") );
  listDebugShow(list,LIST_DEBUG_SHOW_SHORT);
  showItems(list);



  printf("\nTest: Getting data.\n");
  for(i=0; i < list->count; i++)
  {
    printf("  Idx %i Got data: %p\n", i, listGetItemAt(list, i )->data );
  }

  printf("\nTest: Filling list into array.\n");
  item** arr = (item**)listAddToArray( malloc(sizeof(void*)*list->count), list );

  int idx;
  for(idx=0; idx < list->count; idx++)
  {
    printf("  Arr[%i] = %p\n",idx,arr[idx] );
    showItem(arr[idx]);
  }

  printf("\nTest: Appending array to list2.\n");
  list_t* list2 = listAddFromArray(listInit(NULL), (void*)arr, list->count);
  listDebugShow(list2,LIST_DEBUG_SHOW_SHORT);
  showItems(list2);

  list_t* list3 = listAddFromArray(listInit(NULL), (void*)arr, list->count);

  free(arr);

  printf("\nRemoving all list2 elements individually in a forward loop.");
  it=&list2->begin;
  while( LISTFWD(list2,it) )
  {
    it=listRemoveItem(list2,it, LIST_PREV);
  }
  listDebugShow(list2, LIST_DEBUG_SHOW_BACKWARD|LIST_DEBUG_SHOW_FORWARD);

  printf("\nRemoving all list3 elements individually in a reverse loop.");
  it=&list3->end;
  while( LISTBCK(list3, it) )
  {
    it=listRemoveItem(list3,it,LIST_NEXT);
  }
  listDebugShow(list3, LIST_DEBUG_SHOW_BACKWARD|LIST_DEBUG_SHOW_FORWARD);



  printf("\nTest: Freeing lists.\n");
  listFree(list);
  listFree(list2);

  printf("\nTests done, it's up to you to figure out if they passed, <nelson>hahaaa!</nelson>\n");

  return(0);
}
