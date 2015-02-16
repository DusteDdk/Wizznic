#!/bin/bash

#Thank you Farox!

#export SDL_AUDIODRIVER="alsa"
export SDL_VIDEODRIVER="omapdss"
export SDL_OMAP_VSYNC="0"
export SDL_OMAP_FORCE_DOUBLEBUF="0"
export SDL_OMAP_LAYER_SIZE="scaled"
export SDL_OMAP_LAYER_SIZE="800x480"

./wizznic $@
