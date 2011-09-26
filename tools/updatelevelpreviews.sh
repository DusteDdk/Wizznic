#!/bin/bash

if [ "$1" == "" ]
then
  echo "Usage: updatelevelpreviews.sh PACKNAME"
  exit
fi
cd ..
DIR="packs/$1/levels/"

if [ ! -d "$DIR" ]
then
  echo "Hmm, can't find $DIR..."
  exit
fi

./wizznic -d $1

cd "$DIR"

L=`ls | grep .wzp.tga`

for F in $L
do
  echo "Cropping file: $F"
  NF=${F/.wzp.tga/.wzp.png}
  rm $NF &> /dev/null
  if [ "$2" != "" ]
  then
    convert $F -crop 230x230+86+4 -resize $2 $NF
  else
    convert $F -crop 230x230+86+4 -resize 110x110 $NF
  fi
  rm $F
done

echo "Done."
