#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

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

//Macro for iterating
#define LISTFWD(LIST, IT) (IT=IT->next) != &LIST->end
#define LISTBCK(LIST, IT) (IT=IT->prev) != &LIST->begin

struct listItemStruct
{
  void* data;
  struct listItemStruct* next;
  struct listItemStruct* prev;
};

typedef struct listItemStruct listItem;

typedef void (*listFreeFunc)(void*);

typedef struct
{
    struct listItemStruct begin;
    struct listItemStruct end;
    int count;
    listFreeFunc freeFunc;
} list_t;


//Show a list (for debugging purposes)
void listDebugShow(list_t* list, uint_fast8_t all );

//Add item to beginning of list, return pointer to new item
listItem* listPrependData(list_t* list, void* data);

//Add item to end of list, return pointer to new item
listItem* listAppendData(list_t* list, void* data);

//Add item at position p into the list, return pointer to new item
listItem* listInsertAtIdx(list_t* list, void* data, int p);

//Add item in after item (As fast as append and prepend)
listItem* listInsertAfterItem(list_t* list, listItem* item, void* data);

//Removes item from list.
//If direction is LIST_PREV it will return the item just BEFORE the removed one, or end if empty (for use in forward iterations).
//If direction is LIST_NEXT it will return the item just AFTER the removed one, or begin if empty (for use in reverse iterations).
//If the item was not found, it will return a null pointer.
listItem* listRemoveItem(list_t* list, listItem* item, uint_fast8_t direction);

//Initialize a new empty list
list_t* listInit(listFreeFunc freeFunc);

//Frees memory used by list - If the freeFunc pointer is not null, this is called with the pointer for each data element.
void listFree(list_t* list );

//Returns pointer to item at index or NULL if the list is empty or index invalid.
listItem* listGetItemAt(list_t* list, int index);

//Add data from list to array, the caller must allocate the array
void** listAddToArray(void** array, list_t* list);

//Add data from array to list
list_t* listAddFromArray(list_t* list, void** data, int count );

#define LIST_DEBUG_SHOW_FORWARD 1
#define LIST_DEBUG_SHOW_BACKWARD 2
#define LIST_DEBUG_SHOW_SHORT 0

#define LIST_NEXT 0
#define LIST_PREV 1


#endif // LIST_H_INCLUDED
