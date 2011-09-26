#!/bin/bash

#Tries to build releases for each major platform (only Wiz and Linux for now)

if [ "$2" = "" ]
then
  echo "Usage: release VERSION SOMEWHEREYOUWANTTHERELEASES"
  exit
fi

if ! ls release.sh &> /dev/null
then
  echo "Must execute from tools/releaser/"
  exit
fi

cd ../..
SRCDIR=`pwd`

VERSION=$1
DEST=$2
SRCREL="wizznic-$VERSION-src"
WIZREL="wizznic-$VERSION-wiz"
GFXREL="wizznic-$VERSION-srcgfx"

#No trailing /
WD="/tmp/wizznic"

echo "Using workdir $WD"

#clear previous dir
rm -R $WD

#create dirs
mkdir $WD
mkdir $WD/$GFXREL

#export into both
cd $SRCDIR
svn export . $WD/$SRCREL
svn export . $WD/$WIZREL

#move the srcgfx
mv $WD/$SRCREL/srcgfx $WD/$GFXREL

#package the srcgfx
cd $WD
tar -jcf "$GFXREL.tar.bz2" $GFXREL
mv "$GFXREL.tar.bz2" "$DEST/"

#The src one needs minimal work..
cd $WD/$SRCREL
make -f Makefile.linux
chmod 777 wizznic
mv wizznic wizznic-precompiled-x86
make -f Makefile.linux clean

cd ..
#Package
tar -jcf "$SRCREL.tar.bz2" $SRCREL
mv "$SRCREL.tar.bz2" "$DEST/"

#The wiz version needs a bit more
mkdir game
mkdir game/$WIZREL
cd $WIZREL
make -f Makefile.wiz
cp wizznic-wiz ../game/$WIZREL/
cp -a data ../game/$WIZREL/
cp -a doc ../game/$WIZREL/
cp -a packs ../game/$WIZREL/

cp tools/releaser/data/wiz/wizznic.gpe ../game/$WIZREL/
cp tools/releaser/data/wiz/wiz-launch-icon.png ../game/$WIZREL/

sed s/%VERSION%/$VERSION/g tools/releaser/data/wiz/wiz-launch-ini.ini > ../game/wizznic-$VERSION.ini
cd ..
rm -R $WIZREL
zip -r -T -9 "$WIZREL.zip" game
mv "$WIZREL.zip" "$DEST/"
cd ..
rm -R $WD
echo "Done..."
