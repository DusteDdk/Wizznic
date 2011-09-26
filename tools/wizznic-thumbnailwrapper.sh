#!/bin/bash

#The full path to your wizznic directory.
WIZZNICPATH="/home/dusted/Desktop/Desktop/dusted/ting/Wiz/WizDev/wizznic/"

LOG=/tmp/wizzthumb.log

cd $WIZZNICPATH
CMD="./wizznic -thumbnailer $1 $2"

echo $CMD >> $LOG

$CMD


#resize and convert to png
mv $2 $2.tga
convert $2.tga -crop 230x230+86+4 -resize $3x$3 $2.png
rm $2.tga
cp $2.png $2
