#!/bin/sh

PLATFORM="$1"
ANIMATION_DIR="$(dirname "$(readlink -f "$0")")"
FRAMES_DIR="$PLATFORM"
RESULT_FILE="xxx~$PLATFORM"

cd "$ANIMATION_DIR"

convert -loop 1 -dispose previous -background none \
  -delay 10 "$FRAMES_DIR/01.svg" \
  -delay 10 "$FRAMES_DIR/02.svg" \
  -delay 10 "$FRAMES_DIR/03.svg" \
  -delay 10 "$FRAMES_DIR/04.svg" \
  -delay 10 "$FRAMES_DIR/05.svg" \
  -delay 100 "$FRAMES_DIR/06.svg" \
  -delay 10 "$FRAMES_DIR/05.svg" \
  -delay 10 "$FRAMES_DIR/04.svg" \
  -delay 10 "$FRAMES_DIR/03.svg" \
  -delay 10 "$FRAMES_DIR/02.svg" \
  -delay 10 "$FRAMES_DIR/01.svg" \
  "$RESULT_FILE.gif"

gif2apng -z0 "$RESULT_FILE.gif" "$RESULT_FILE.apng" > /dev/null

rm "$RESULT_FILE.gif"

cd - > /dev/null
