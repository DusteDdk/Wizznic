#include "strings.h"
#include <string.h>

void stripNewLine(char* str)
{
  int i;
  for(i=0; i<strlen(str); i++)
  {
    if( str[i] == '\r' || str[i] == '\n' )
      str[i]='\0';
  }
}

int splitVals(char ch,const char* buf, char* set, char* val)
{
  int strpos=0;
  set[0]='\0';
  val[0]='\0';
  if(buf[0]!='#')
  {
    for(strpos=0; strpos<strlen(buf);strpos++)
    {
      //Here's a setting!
      if(buf[strpos]==ch)
      {
        strcpy(set,buf);
        set[strpos]='\0';
        strcpy(val,buf+strpos+1);
        return(1);
      }
    }
  }
  return(0);
}

//Return position of last instance of c
//Return 0 if none found
int charrpos(const char* str, char c)
{
  int i;
  for(i=strlen(str); i != 0; i--)
  {
    if(str[i]==c)
    {
      return(i);
    }
  }
  return(0);
}
