/* This file was copied with little or no modification from the WizPort of OpenJazz */
/* There's also some bits from libcastor in here (Copyright (c) 2009 Adan Scotney) */

#ifdef WIZ

#include "../settings.h"

#include "wiz.h"


#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <fcntl.h>
#include <unistd.h>
#include "../ticks.h"

//For clock code
#include <sys/mman.h>


static int volume = 100;
static int volume_direction;

void WIZ_SetVolume( int volumePercent )
{
  if(volume==volumePercent) return;

  volume=volumePercent;
  WIZ_AdjustVolume( VOLUME_NOCHG );
}

int WIZ_GetVolume()
{
  return(volume);
}

static int countDown=0;
static int lastVol = 0;
void WIZ_ShowVolume(SDL_Surface* screen)
{
  char buf[11]; //vol: xxx%\0 = 10.

  if(countDown > 0)
  {
    countDown-=getTicks();
    sprintf(buf, "vol: %i", setting()->wizVol);
    txtWriteCenter(screen, FONTSMALL, buf, 160, 200);
  }
}


void WIZ_AdjustVolume( int direction )
{
	if( direction != VOLUME_NOCHG )
	{
	  countDown=1500;
		if( volume <= 10 )
		{
			if( direction == VOLUME_UP )   volume += VOLUME_CHANGE_RATE/2;
			if( direction == VOLUME_DOWN ) volume -= VOLUME_CHANGE_RATE/2;
		}
		else
		{
			if( direction == VOLUME_UP )   volume += VOLUME_CHANGE_RATE;
			if( direction == VOLUME_DOWN ) volume -= VOLUME_CHANGE_RATE;
		}

		if( volume < VOLUME_MIN ) volume = VOLUME_MIN;
		if( volume > VOLUME_MAX ) volume = VOLUME_MAX;
	}
	unsigned long soundDev = open("/dev/mixer", O_RDWR);
	if(soundDev)
	{
		int vol = ((volume << 8) | volume);
		ioctl(soundDev, SOUND_MIXER_WRITE_PCM, &vol);
		close(soundDev);
	}
	setting()->wizVol=volume;
	saveSettings();
}

#define PLLSETREG0		(memregs32[0xF004>>2])
#define PWRMODE			(memregs32[0xF07C>>2])
#define SYS_CLK_FREQ 27
static int omhz=533;
int WIZ_SetClock(int mhz)
{
  if(mhz==omhz) return(0);
  omhz=mhz;
  /* Open CPU reg */
  volatile uint32_t *memregs32;
  int memfd;
  memfd = open("/dev/mem", O_RDWR);
	if(memfd < 0) return(0);

	memregs32 = (volatile uint32_t*)mmap(0, 0x20000, PROT_READ|PROT_WRITE, MAP_SHARED, memfd, 0xC0000000);
	if(memregs32 == (volatile uint32_t*)0xFFFFFFFF) return(0);

  /* Set CPU Freq */
	unsigned  long v;
	unsigned mdiv, pdiv=9, sdiv=0;

	mdiv= (mhz * pdiv) / SYS_CLK_FREQ;
	mdiv &= 0x3FF;
	v= pdiv<<18 | mdiv<<8 | sdiv;

	PLLSETREG0 = v;
	PWRMODE |= 0x8000;

  /* Close CPU reg */

	memregs32 = NULL;
	close(memfd);
  return(1);
}

void platformExit()
{
    WIZ_SetClock(533);
}
#endif
