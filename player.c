#include "player.h"
#include "settings.h"

static player_t playerInfo;

player_t* player()
{
  return(&playerInfo);
}

void initPlayer()
{
  memset(&playerInfo,0, sizeof(player_t));
  playerInfo.lives=3;
}

