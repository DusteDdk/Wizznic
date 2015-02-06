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

#include <stdio.h>
#include <stdint.h>

#include "../src/bundle.h"

int main(int argc, char** argv)
{
  printf("\nWizznic bundle tool -- Creates and extracts Wizznic! bundles.\n<WizznicBundle> Format version: 0x%X\n", BUNDLE_FILE_VERSION );
  int ret = BUNDLE_SUCCESS;
  if( argc == 4 || argc == 5)
  {
    if( argc==5 && argv[4][0] == 'f' )
    {
      printf("Note: Writing a bundle with reverse byte-order, this will cause conversion on this platform during debundling.\n");
      bundleSetFlip(1);
    }
    if( argv[3][0] == 'd')
    {
      ret=debundle( argv[1], argv[2] );
      if( ret == BUNDLE_SUCCESS )
      {
        printf("Debundle success.\n");
      } else {
        printf("Debundle error: %i.\n",ret);
      }
    } else if( argv[3][0] == 'b')
    {
      bundle( argv[1], argv[2] );
      printf("Bundle done.\n");
    }
  } else {
    printf("  Usage:\n    %s file dir action [f]\n      file - The bundle file.\n      dir - Directory to bundle or extract to\n      action - d = debundle(extract) b = bundle.\n      f (optional) - flip byte-order to test conversion works.\n\nWhen creating a bundle, relative paths: ./mkBundle wizznic.bin 000_wizznic b\n\n",argv[0]);
  }
  return( ((ret==BUNDLE_SUCCESS)?0:1) );
}
