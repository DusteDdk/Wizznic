#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED

/************************************************************************
 * This file is part of Wizznic.                                        *
 * Copyright 2009-2012 Jimmy Christensen <dusted@dusted.dk>             *
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

void listAddData(listItem* start, void* data);
void listInsertData(listItem* start, void* data, int p); //Inserts into the list at pos p. 0 = first

listItem* listRemoveItem(listItem* start, listItem* item); //Removes item from list, returns item just before removed item.
listItem* initList();
void freeList(listItem* start);
int listSize(listItem* start);

void* listGetItemData(listItem* start, int index);


#endif // LIST_H_INCLUDED
