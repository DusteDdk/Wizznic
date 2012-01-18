#include "pc.h"
#include "../settings.h"

#if defined(WITH_OPENGL)
static GLuint texture;
static GLuint dlist;

static SDL_Surface* oglWindow;

SDL_Surface* platformInitAccel( int sdlVideoModeFlags )
{
      const SDL_VideoInfo* vidinfo = SDL_GetVideoInfo();
      int w=setting()->glWidth,h=setting()->glHeight;
      if( (sdlVideoModeFlags&SDL_FULLSCREEN) )
      {
        w = vidinfo->current_w;
        h = vidinfo->current_h;
      } else {
        //Find largest resolution within screen
        if(w==-1||h==-1)
        {
          int factor=(int)floor( (float)(vidinfo->current_h-1)/240.0 );
          w=320*factor;
          h=240*factor;
        }
      }

      SDL_Surface* screen = SDL_CreateRGBSurface(SDL_SWSURFACE, 320,240,24, 0x00ff0000,0x0000ff00,0x000000ff,0xff000000);
      oglWindow = SDL_SetVideoMode(w,h,32, SDL_OPENGL | sdlVideoModeFlags );

      int vW = (GLint)h*(320.0f/240.0f);

      glViewport(w/2-vW/2, 0, vW, (GLint)h);

      glClearColor(1,0,0,1);

      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glOrtho( 0, SCREENW, SCREENH, 0, 0,1);
      glColor4f(1,1,1,1);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();

      glDisable(GL_DEPTH_TEST);
      glDisable( GL_CULL_FACE );
      glDisable(GL_LIGHTING);

      glEnable(GL_BLEND);
      glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      }


      dlist = glGenLists (1);
      glNewList(dlist, GL_COMPILE);
      glBegin( GL_QUADS );
        glTexCoord2f(0,0);
        glVertex2i(0,0);
        glTexCoord2f(1,0);
        glVertex2i(320,0);
        glTexCoord2f(1,1);
        glVertex2i(320,240);
        glTexCoord2f(0,1);
        glVertex2i(0,240);
      glEnd();
      glEndList();

      //Set scaling
      setting()->scaleFactor= (float)oglWindow->h/240.0;

      return(screen);
}

void platformDrawScaled( SDL_Surface* src )
{
  glTexImage2D( GL_TEXTURE_2D, 0, src->format->BytesPerPixel, src->w, src->h, 0, GL_BGR, GL_UNSIGNED_BYTE, src->pixels );
  glCallList(dlist);
  SDL_GL_SwapBuffers();
}
#endif
