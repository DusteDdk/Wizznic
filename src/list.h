#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED

/************************************************************************
 * This file is part of Wizznic.                                        *
 * Copyright 2009-2013 Jimmy Christensen <dusted@dusted.dk>             *
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

struct listItemStruct
{
  void* data;
  struct listItemStruct* next;
};

typedef struct listItemStruct listItem;

//Add data to the end of a list
void listAddData(listItem* start, void* data);

//Fast, appends a new item containing data to a listitem.
listItem* listAppendData(listItem* item, void* data);


//Inserts into the list at pos p. 0 = first
void listInsertData(listItem* start, void* data, int p);

listItem* listRemoveItem(listItem* start, listItem* item); //Removes item from list, returns item just before removed item. (or 0)
listItem* cutItem(listItem* previous, listItem* removeMe ); //Given previous item, removes item, returns next item (or 0)

void listReverse( listItem* old, listItem* new ); //Appends the elements of old in to new in reverse order


listItem* initList();
void freeList(listItem* start);
int listSize(listItem* start);

void* listGetItemData(listItem* start, int index);


#endif // LIST_H_INCLUDED
