#!/bin/bash

for i in frames/*
do
  rsvg-convert "$i" -o `echo "$i" | sed -e 's/svg$/png/'`
done

apngasm -o ../slide.apng -f animation.xml -F

rm -f frames/*.png