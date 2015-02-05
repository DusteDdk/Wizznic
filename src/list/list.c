/************************************************************************
 * This file is part of Wizznic.                                        *
 * Copyright 2009-2015 Jimmy Christensen <dusted@dusted.dk>             *
 * Wizznic is free software: you can redistribute it and/or modify      *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation, either version 3 of the License, or    *
 * (at your option) any later version.                                  *
 *                                                                      *
 * Wizznic is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 * GNU General Public License for more details.                         *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with Wizznic.  If not, see <http://www.gnu.org/licenses/>.     *
 ************************************************************************/

#include "list.h"

void listDebugShow(list_t* list, uint_fast8_t all )
{
  listItem* it;
  printf("  List %p size %i.\n", list, list->count);
  printf("  List->begin( %p )->next = %p%s\n", &list->begin, list->begin.next, ((list->begin.next==&list->end)?" (end)":"") );
  printf("  List->begin( %p )->prev = %p%s\n", &list->begin, list->begin.prev, ((list->begin.prev==&list->end)?" (end)":"") );
  printf("  List->end( %p )->next = %p%s\n", &list->end, list->end.next, ((list->end.next==&list->begin)?" (begin)":"") );
  printf("  List->end( %p )->prev = %p%s\n", &list->end, list->end.prev, ((list->end.prev==&list->begin)?" (begin)":"") );

  //Check that list will iterate to the end
  it = &list->begin;
  int c=0;
  while( (it=it->next) != &list->end )
  {
    if(c++ > list->count)
    {
	printf("  == ERROR == More forward iterations than list elements; List is corrupt.\n");
	return;
    }
  }
  //Check that list will iterate to the beginning
  it = &list->end;
  c=0;
  while( (it=it->prev) != &list->begin )
  {
    if(c++ > list->count)
    {
	printf("  == ERROR == More backward iterations than list elements; List is corrupt.\n");
	return;
    }
  }

  if( all & LIST_DEBUG_SHOW_FORWARD )
  {
    printf("  Forward iteration:\n");
    it = &list->begin;

    while( (it=it->next) != &list->end )
    {
      printf("    listItem( %p )->data = %p ( prev = %p  - next = %p )\n", it, it->data, it->prev, it->next);
    }
  }
  if( all & LIST_DEBUG_SHOW_BACKWARD )
  {
    printf("  Reverse iteration:\n");
    it = &list->end;
    c=0;
    while( (it=it->prev) != &list->begin )
    {
      printf("    listItem( %p )->data = %p ( prev = %p  - next = %p )\n", it, it->data, it->prev, it->next);
    }
  }

}



// Add item to start of list, return pointer to new item
listItem* listPrependData(list_t* list, void* data)
{
  //Increment number of items in list
  list->count++;

  //Allocate new item structure
  listItem* t = malloc(sizeof(listItem));

  //Assign data pointer
  t->data=data;

  //The next element of this list is the current beginnings next
  t->next=list->begin.next;

  //The previous element of this list is the beginning
  t->prev=&list->begin;

  //The current beginnings next previous should be set to the new item
  list->begin.next->prev=t;

  //The current beginning is this item
  list->begin.next=t;

  return(t);
}

// Add item to end of list, return pointer to new item
listItem* listAppendData(list_t* list, void* data)
{
  //Increment number of items in list
  list->count++;

  //Allocate new item structure
  listItem* t=malloc(sizeof(listItem));

  //Assign data pointer
  t->data=data;

  //Next element of a new element will always be a pointer to the end
  t->next=&list->end;

  //The previous element of the new element will be the previous end.
  t->prev=list->end.prev;

  //Set previous element next to point to the new element
  list->end.prev->next=t;

  //Set the lists end to point to the new element
  list->end.prev=t;

  return(t);
}

//Inserts item at pos p, returns a pointer to item or 0 if p is out of bounds.
//Worst case it takes n/2 iterations to find position to insert into
listItem* listInsertAtIdx(list_t* list, void* data, int p)
{
  listItem* t = malloc(sizeof(listItem));
  listItem* it;
  t->data=data;
  int pos;

  if(p<0 || p > list->count)
  {
    printf("listInsertData Error: Position %i unacceptable for list %p with %i items.\n",p,list,list->count);
    return(0);
  }

  if( p <= list->count/2)
  {
    //Go from beginning
    it = list->begin.next;
    pos=0;
    while( pos != p )
    {
      it = it->next;
      pos++;
    }
  } else {
    //Go from end
    it = &list->end;
    pos=list->count;
    while( pos != p )
    {
      it = it->prev;
      pos--;
    }
  }

  list->count++;
  t->next=it;
  t->prev=it->prev;
  it->prev->next=t;
  it->prev=t;

  return(t);
}

//Add item in after item (As fast as append and prepend)
listItem* listInsertAfterItem(list_t* list, listItem* item, void* data)
{
  list->count++;
  listItem* t = malloc(sizeof(listItem));
  t->data = data;
  t->prev = item;
  t->next = item->next;

  item->next->prev = t;
  item->next = t;

  return(t);
}


//Removes item from list, returns 0 if not found in list, or either NEXT or PREV depending on direction.
//If freeFunc is not 0 then it is called with the item data as parameter.
listItem* listRemoveItem(list_t* list, listItem* item, uint_fast8_t direction)
{
  listItem* it=&list->begin;
  listItem* ret;
  while( (it=it->next) != &list->end )
  {
    if(it==item)
    {
      list->count--;
      it->prev->next = it->next;
      it->next->prev = it->prev;
      if(direction==LIST_PREV)
      {
        ret = it->prev;
      } else {
        ret = it->prev->next;
      }
      if(list->freeFunc)
      {
        list->freeFunc(it->data);
      }
      free(it);
      return(ret);
    }
  }

  return(NULL);
}


list_t* listInit(listFreeFunc freeFunc)
{
  list_t* list = memset(malloc(sizeof(list_t)), 0, sizeof(list_t));
  list->end.next=&list->begin;
  list->end.prev=&list->begin;
  list->begin.next=&list->end;
  list->begin.prev=&list->end;
  list->freeFunc=freeFunc;
  return(list);
}


void listFree(list_t* list)
{
  //Recurse through list:
  listItem* it=list->begin.next;
  listItem* next;

  while( it != &list->end )
  {
    if( list->freeFunc )
    {
	list->freeFunc(it->data);
    }
    next=it->next;
    free(it);
    it=next;
  }
  free(list);
}



//Worst case is listsize/2 iterations to reach index
listItem* listGetItemAt(list_t* list, int index)
{
  int idx;
  listItem* it;
  if(index > list->count-1 || index < 0 )
  {
      printf("\n----\nlistGetItemData Error: Requested data for index %i in list %p of size %i\n----\n", index,list,list->count );
  } else {
    if( index <= list->count/2)
    {
      it=list->begin.next;
      idx=0;
      while( idx < list->count )
      {
        if(idx == index )
        {
          return(it);
        }
        idx++;
        it=it->next;
      }
    } else {
      idx=list->count-1;
      it=list->end.prev;
      while( idx > -1 )
      {
        if( idx == index )
        {
          return(it);
        }
        idx--;
        it=it->prev;
      }
    }
  }

  return(NULL);
}


void** listAddToArray(void** array, list_t* list)
{
  listItem* it = &list->begin;
  int idx=0;
  while( (it=it->next) != &list->end )
  {
    array[idx] = it->data;
    idx++;
  }

  return(array);
}

list_t* listAddFromArray(list_t* list, void** data, int count )
{
  int i;
  for(i=0; i < count; i++)
  {
    listAppendData(list, data[i]);
  }
  return(list);
}
