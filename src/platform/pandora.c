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

#include <math.h>
#include "eglport.h"

#include "pandora.h"
#include "../settings.h"

static GLuint texture;
static GLuint dlist;

static SDL_Surface* oglWindow;


SDL_Surface* platformInitAccel( int sdlVideoModeFlags )
{

  int w = 800;
  int h = 480;


  SDL_Surface* screen = SDL_CreateRGBSurface(SDL_SWSURFACE, 320,240,24, 0x00ff0000,0x0000ff00,0x000000ff,0xff000000);
  oglWindow = SDL_SetVideoMode(w,h,0, SDL_SWSURFACE | SDL_FULLSCREEN );

  if (EGL_Open (w, h)) {
        printf("EGL_Open(); Failed, sorry.\n");
        return(0);
  }

  int vW = (GLint)h*(320.0f/240.0f);

  glViewport(w/2-vW/2, 0, vW, (GLint)h);

  glClearColor(1,0,0,1);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();



  glOrthof( 0, (float)SCREENW, (float)SCREENH, 0, 0,1);
  glColor4f(1,1,1,1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glDisable(GL_DEPTH_TEST);
  glDisable( GL_CULL_FACE );
  glDisable(GL_LIGHTING);
  glDisable(GL_BLEND);


  glColor4f(1,1,1,1);


  glEnable(GL_TEXTURE_2D);
  glGenTextures( 1, &texture );
  glBindTexture( GL_TEXTURE_2D, texture );

  if( setting()->glFilter )
  {
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  } else {
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  }


  //Set scaling
  setting()->scaleFactor= (float)oglWindow->h/240.0;
  return(screen);
}

void platformExit()
{
  EGL_Close();
}

void platformDrawScaled( SDL_Surface* src )
{
  glTexImage2D( GL_TEXTURE_2D, 0, src->format->BytesPerPixel, src->w, src->h, 0, GL_RGB, GL_UNSIGNED_BYTE, src->pixels );

  GLfloat box[] = {0,0,-1,  320,0,-1,     320,240,-1,   0,240,-1};
  GLfloat tex[] = {0,0, 1,0, 1,1, 0,1};

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  glVertexPointer(3, GL_FLOAT, 0,box);
  glTexCoordPointer(2, GL_FLOAT, 0, tex);


  glDrawArrays(GL_TRIANGLE_FAN,0,4);

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);


  EGL_SwapBuffers ();
}
