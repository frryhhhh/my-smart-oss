#!/usr/bin/env python3
# Пак MOPAK1: заголовок 16 байт + записи по 40 байт + данные (выровнены по 16).
import sys, struct, pathlib

src = pathlib.Path(sys.argv[1])
dst = pathlib.Path(sys.argv[2])
files = sorted(p for p in src.iterdir() if p.is_file())
if not files:
    sys.exit("Нет файлов для пака")

ENT = 40
head = 16 + ENT * len(files)
off = (head + 15) // 16 * 16
entries, blobs = [], []

for p in files:
    data = p.read_bytes()
    name = p.name.encode("utf-8")[:31]
    entries.append(struct.pack("<32sII", name, off, len(data)))
    pad = (-len(data)) % 16
    blobs.append(data + b"\xFF" * pad)
    print(f"{p.name:22} offset=0x{off:08X} size={len(data)}")
    off += len(data) + pad

with dst.open("wb") as f:
    f.write(struct.pack("<8sII", b"MOPAK1\0\0", len(files), 0))
    for e in entries:
        f.write(e)
    f.write(b"\xFF" * (((head + 15) // 16 * 16) - head))
    for b in blobs:
        f.write(b)

print(f"Пак готов: {dst} ({dst.stat().st_size} байт)")
