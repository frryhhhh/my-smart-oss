#!/usr/bin/env bash
# Генерация кириллических шрифтов LVGL. Вызывается из CI.
set -euo pipefail
DEST="${1:-MiniOS}"
TMP="build/fonts"
mkdir -p "$TMP" "$DEST"

# DejaVu Sans: свободная лицензия, есть кириллица
if [ ! -f "$TMP/DejaVuSans.ttf" ]; then
  curl -fsSL -o "$TMP/dejavu.zip" \
    https://github.com/dejavu-fonts/dejavu-fonts/releases/download/version_2_37/dejavu-fonts-ttf-2.37.zip
  unzip -o -j "$TMP/dejavu.zip" \
    "dejavu-fonts-ttf-2.37/ttf/DejaVuSans.ttf" \
    "dejavu-fonts-ttf-2.37/ttf/DejaVuSans-Bold.ttf" -d "$TMP"
fi

npm install -g lv_font_conv@1.5.3

SYM="°–—•…←↑→↓✓⌫⇧№"
RANGES=(-r 0x20-0x7F -r 0x400-0x4FF)   # латиница + кириллица

gen () { # size  ttf  имя
  lv_font_conv --bpp 4 --size "$1" --font "$TMP/$2" \
    "${RANGES[@]}" --symbols "$SYM" \
    --format lvgl --no-compress -o "$DEST/$3.c"
  echo "готов шрифт $3"
}

gen 14 DejaVuSans.ttf      ui_font_14
gen 18 DejaVuSans.ttf      ui_font_18
gen 24 DejaVuSans.ttf      ui_font_24
gen 28 DejaVuSans-Bold.ttf ui_font_28b
gen 44 DejaVuSans-Bold.ttf ui_font_44b

ls -l "$DEST"/ui_font_*.c
