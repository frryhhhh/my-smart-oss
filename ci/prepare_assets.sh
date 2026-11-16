#!/usr/bin/env bash
# Видео -> MJPEG (склейка JPEG-кадров) + обои и их размытие.
set -euo pipefail
FPS="${FPS:-15}"
Q="${JPEG_Q:-8}"
CROP_H="${CROP_H:-crop=320:240:0:ih*0.38}"
CROP_V="${CROP_V:-crop=240:320:0:ih-320}"
SIGMA="${BLUR_SIGMA:-14}"
SRC="assets/intro.mp4"
OUT="build/assets"
mkdir -p "$OUT"

if [ -f "$SRC" ]; then
  IN=(-i "$SRC")
else
  echo "ВНИМАНИЕ: $SRC не найден, делаю временную заглуку-тест"
  IN=(-f lavfi -i "testsrc2=size=1080x1920:rate=${FPS}:duration=6")
fi

clip () { # фильтр  выход
  ffmpeg -y -hide_banner -loglevel error "${IN[@]}" -an \
    -vf "fps=${FPS},${1}" -c:v mjpeg -q:v "$Q" -f image2pipe "$OUT/$2"
}
poster () { # фильтр выход
  ffmpeg -y -hide_banner -loglevel error "${IN[@]}" -an \
    -vf "fps=${FPS},${1}" -frames:v 1 -q:v 5 "$OUT/$2"
  ffmpeg -y -hide_banner -loglevel error -i "$OUT/$2" \
    -vf "gblur=sigma=${SIGMA}" -q:v 6 "$OUT/${3}"
}

clip   "scale=320:-2,${CROP_H}" intro_h.mjpeg
clip   "scale=240:-2,${CROP_V}" intro_v.mjpeg
poster "scale=320:-2,${CROP_H}" wall_h.jpg wall_h_blur.jpg
poster "scale=240:-2,${CROP_V}" wall_v.jpg wall_v_blur.jpg

ls -l "$OUT"
