# gifdec.ldg

Library using the LDG system and the GIF decoder functions from GIFLIB.
Instead of generating TC24 bitmap, the result is 2D width x height sized byte-array of colors indexes (ie chunky format).

Used by:

* P2SM to handle large GIF pictures.
* MapEdit to load tiles from GIF (tile singleton or tiles containor).
* MAKE_MAP to slice large GIF into tiles and map array.

Other programs can use it, please read these sources and P2SM GFA source.

# installation for makefiles

- pre-requisite: different targets of libldg.a in /opt/cross-mint/m68k-atari-mint/lib/

- get /gifdec.ldg/ from [codecs_r5_src.zip](https://ptonthat.fr/files/gifdec/codecs_r5_src.zip) and unpack the contents to ./

- pngdec.ldg.xcodeproj is for Xcode 26.3, you may not need it if you use something else.
