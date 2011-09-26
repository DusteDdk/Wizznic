#!/bin/bash

if [ "$2" = "" ]
then
  echo "Usage: movlvl num newnum [-dryrun]"
  echo "Moves level num from num to newnum."
  echo "  -dryrun and no changes will be made"
  exit
fi

OLDPOS=$1
NEWPOS=$2
export DRYRUN=$3

function MV {
  if [ "$DRYRUN" != "" ]
  then
    DR="(Would rename)"
  else
    DR="Renaming"
    mv "$1" "$2"
  fi
  echo "$DR $1 to $2"
}


if [ "$OLDPOS" -gt "$NEWPOS" ]
then
  #if larger
  MV "level$OLDPOS.wzp" "level$OLDPOS-temp.wzp"

  #add one to all files in range
  CN=$(($OLDPOS-1))
  while [ "$CN" -gt "$(($NEWPOS-1))" ]
  do
    MV "level$CN.wzp" "level$(($CN+1)).wzp"
    CN=$(($CN-1))
  done

  MV "level$OLDPOS-temp.wzp" "level$NEWPOS.wzp"

else

  #if smaller
  MV "level$OLDPOS.wzp" "level$OLDPOS-temp.wzp"

  #subtract one from all files in range
  CN=$(($OLDPOS+1))
  while [ "$CN" -lt "$(($NEWPOS+1))" ]
  do
    MV "level$CN.wzp" "level$(($CN-1)).wzp"
    CN=$(($CN+1))
  done

  MV "level$OLDPOS-temp.wzp" "level$NEWPOS.wzp"


fi
