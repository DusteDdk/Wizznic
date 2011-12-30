#!/bin/bash

echo "Very dangerous renamer.. iterates through a directory containing sorted directories with single files, pulls the files out of that and renames it to the number that its dir corrosponds to"
cd $1

if [ "$1" == "" ]
then
  echo "Whats the dir with the dirs?"
  exit
fi

find -name *.wzp.png -delete

echo "In `pwd`"

N="0"

for D in `ls`
do
  echo "Dir: $D Num: $N"
  cd "$D"

  if [ `ls -l | wc -l` -eq 2 ]
  then
    OF=`ls`
    FN=`printf "level%03i.wzp" "$N"`
    echo "Old filename $OF New filename: $FN"
    mv $OF ../../$FN
  else
    echo "Invalid, more than one file."
  fi
  cd ".."
  let N++
done

