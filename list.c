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

#include "list.h"
#include <stdlib.h>

void listAddData(listItem* start, void* data)
{
  //Recurse till next=0
  listItem* t=start, *tt;
  tt=malloc(sizeof(listItem));
  tt->data=data;
  tt->next=0;

  while(1)
  {
    if(!t->next)
    {
      t->next=tt;
      return;
    }
    t=t->next;
  }

}

//Inserts into the list at pos p. 0 = first
void listInsertData(listItem* start, void* data, int p)
{
  int pos=0;
  listItem* it=start, *tt;
  tt=malloc(sizeof(listItem));
  tt->data=data;

  do
  {
    if(p==pos)
    {
      tt->next=it->next;
      it->next=tt;
      break;
    }
    pos++;
  } while( (it=it->next) );

}

//Removes item from list, returns item just before removed item.
listItem* listRemoveItem(listItem* start, listItem* item)
{
  //Recurse till we find item
  listItem* l=start;
  listItem* prev=start;

  while( (l=l->next) )
  {
    if(l==item)
    {
      //Anyone after this?
      if(l->next)
      {
        prev->next=l->next;
      } else {
        prev->next=0;
      }
      return(prev);
    }
    prev=l;
  }
  return(0);
}

listItem* initList()
{
  listItem* ptr = malloc(sizeof(listItem));
  ptr->next=0;
  ptr->data=0;
  return(ptr);
}

void freeList(listItem* start)
{
  //Recurse through list:
  listItem* next=start;
  listItem* t;
  while(1)
  {
    if(next->next)
    {
      t=next;
      next=next->next;
      free(t);
    } else {
      free(next);
      return;
    }
  }
}

int listSize(listItem* start)
{
  listItem* t=start;
  int items=0;
  while( (t=t->next) )
  {
    items++;
  }
  return(items);
}

void* listGetItemData(listItem* start, int index)
{
  listItem* t = start;
  int i=0;
  while( (t=t->next) )
  {
    if(i==index)
    {
      return(t->data);
    }
    i++;
  }
  return(0);
}
