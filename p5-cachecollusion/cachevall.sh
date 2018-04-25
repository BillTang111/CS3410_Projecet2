#!/bin/bash

if [ $# -lt 2 ]; then
  echo "usage: cachevall [options] <bot_name> <dirname>" 1>&2
  echo "   runs <bot_name> against everything else in the <dirname> directory" 1>&2
  exit 1
fi

dir=${@:$#:1}
bot=${@:$(($#-1)):1}
options=${@:1:$(($#-2))}

echo "Playing ${bot} against everything else in ${dir} ..."

# path to sim is same as to this script, with cachevall replaced by cachecollusion
simulator=${0%cachevall.sh}cachecollusion

for otherbot in ${dir}/*; do
  if [ ! -x $otherbot ]; then
    # skip non-executable files
    continue
  fi
  if [ "$otherbot" = "$bot" ]; then
    # skip the bot itself
    continue
  fi
  echo "Playing $bot vs. $otherbot:"
  $simulator $options $bot $otherbot

  echo

  echo "Playing $otherbot vs. $bot:"
  $simulator $options $otherbot $bot

  echo
done
