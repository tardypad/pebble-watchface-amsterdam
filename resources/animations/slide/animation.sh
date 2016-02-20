#!/bin/sh

PLATFORM="$1"
ANIMATION_DIR="$(dirname "$(readlink -f "$0")")"
FRAMES_DIR="$ANIMATION_DIR/$PLATFORM"
RESULT_FILE="$ANIMATION_DIR/xxx~$PLATFORM"

convert -loop 1 -dispose previous -background none \
  -delay 10 "$FRAMES_DIR/01.svg" \
  -delay 10 "$FRAMES_DIR/02.svg" \
  -delay 10 "$FRAMES_DIR/03.svg" \
  -delay 10 "$FRAMES_DIR/04.svg" \
  -delay 100 "$FRAMES_DIR/05.svg" \
  -delay 10 "$FRAMES_DIR/04.svg" \
  -delay 10 "$FRAMES_DIR/03.svg" \
  -delay 10 "$FRAMES_DIR/02.svg" \
  -delay 10 "$FRAMES_DIR/01.svg" \
  "$RESULT_FILE.gif"

gif2apng "$RESULT_FILE.gif" > /dev/null

mv "$RESULT_FILE.png" "$RESULT_FILE.apng"
rm "$RESULT_FILE.gif"
