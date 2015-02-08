#!/bin/bash
# This file is executed by contigratord when a build is triggered.
# You can execute it manually too by executing /home/contigrator/contigrator/tool/trigBuild.sh

#These variables are available:
# $WD = The workspace $BN = Zero padded build Number (%07i) and $BN_NUM for unpadded.
# $OUT = This empty directory is created for you to store the output of the build.
set -e;set -x

#Add your build instructions below:

echo "Wizznic build script."

VERSION_LONG=build_$BN_NUM
VERSION=1.0-dev

make -f Makefile.win clean
make -f Makefile.wiz clean

# The 64 bit linux version
make -j 4 -f Makefile.linux CC='ccache gcc' BUILD_NUMBER=$BN_NUM
mv wizznic wizznic_linux_x64_precompiled
make -j 4 -f Makefile.linux clean

# The chrooted 32 bit linux version.
sudo -E -P USER=$USER USERNAME=$USER LOGNAME=$LOGNAME -- /usr/sbin/chroot /var/local/32bitDeb/ su -p -l $USER -c "cd /home/contigrator/jobs/Wizznic/workspace/ && make -j 4 -f Makefile.linux CC='ccache gcc' BUILD_NUMBER=$BN_NUM"
mv wizznic wizznic_linux_x86_precompiled

#Copy files around and package
DST=Wizznic_lin_build_$BN_NUM
mkdir $DST
cp -a data packs doc wizznic_linux_x64_precompiled wizznic_linux_x86_precompiled $DST/
rm -f wizznic_linux_x86_precompiled wizznic_linux_x64_precompiled
rm $DST/data/border.png
tar jcf $OUT/$DST.tar.bz2 $DST
rm -R $DST

make -f Makefile.linux clean

#32 Bit Windows
make -j 4 -f Makefile.win CC='ccache i586-mingw32msvc-gcc' BUILD_NUMBER=$BN_NUM

DST=Wizznic_win_build_$BN_NUM
mkdir $DST
cp -a data packs doc wizznic.exe tools/releaser/data/win/win-dlls/* tools/releaser/data/win/curl tools/releaser/data/win/Wizznic_* $DST
rm $DST/data/border.png


#Convert docs to Windows format
cd $DST/doc/
unix2dos *.txt
cd ../..

zip -9 -r $OUT/$DST.zip $DST > /dev/null
rm -R $DST

make -f Makefile.win clean


#Gp2X Wiz
export TOOLCHAIN=/opt/arm-openwiz-linux-gnu
make -j 4 -f Makefile.wiz CC='ccache /opt/arm-openwiz-linux-gnu/bin/arm-openwiz-linux-gnu-gcc' BUILD_NUMBER=$BN_NUM

DST=Wizznic_wiz_build_$BN_NUM

mkdir -p $DST
cp -a data packs doc wizznic-wiz tools/releaser/data/wiz/* $DST
rm $DST/data/border.png
cd $DST
mv wiz-launch-ini.ini $DST.ini
sed -i "s/%VERSION_LONG%/$VERSION_LONG/g; s/%VERSION%/$VERSION/g" $DST.ini
cd ..
mkdir game
mv $DST/$DST.ini game/
mv $DST game/
zip -9 -r $OUT/$DST.zip game > /dev/null
rm -R game

make -f Makefile.wiz clean

#GCW-Zero

echo "Making GCW-Zero package"
make -f Makefile.gcw0  CC='ccache /opt/gcw0-toolchain/usr/bin/mipsel-linux-gcc' BUILD_NUMBER=$BN_NUM
make -f Makefile.gcw0 install
mv *.opk $OUT/Wizznic_gcw0_build_"$BN_NUM".opk
rm -f *.opk
make -f Makefile.gcw0 clean

echo "Making source..."

SRCFN="Wizznic_src_build_$BN_NUM.tar.bz2"
git checkout-index -f -a --prefix ./Wizznic_src_build_$BN_NUM/
sed -i 's/#define VERSION_STRING CODE_VERSION$/#define VERSION_STRING CODE_VERSION"(S'$BN_NUM')"/g' ./Wizznic_src_build_$BN_NUM/src/strings.h
tar -jcf $OUT/$SRCFN ./Wizznic_src_build_$BN_NUM
rm -Rf ./Wizznic_src_build_$BN_NUM
SRCMD5=`md5sum $OUT/$SRCFN | cut -d ' ' -f 1`


echo "Making Arch Linux PKGBUILD..."
cp PKGBUILD.template $OUT/PKGBUILD

SRCURL="http:\/\/contigrator.wizznic.org\/jobs\/Wizznic\/builds\/$BN\/output\/$SRCFN"

sed -i "s/<BUILDNUMBER>/$BN_NUM/g; s/<URL>/$SRCURL/g; s/<SRCMD5>/$SRCMD5/g; s/<SRCDIRNAME>/Wizznic_src_build_$BN_NUM/g;" $OUT/PKGBUILD
