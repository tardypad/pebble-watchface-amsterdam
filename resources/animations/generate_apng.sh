#!/bin/bash

function generate_animation_apng()
{
  cd "$1"

  for i in frames/*
  do
    rsvg-convert "$i" -o `echo "$i" | sed -e 's/svg$/png/'`
  done

  apngasm -o xxx.apng -f animation.xml -F > /dev/null

  rm -f frames/*.png

  cd - > /dev/null
}

for folder in */ ; do
    generate_animation_apng "$folder"
done
