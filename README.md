[![Entice Logo](https://github.com/vtorri/entice/raw/master/data/icons/entice.png)](https://github.com/vtorri/entice)

[![Github Build status](https://github.com/vtorri/entice/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/vtorri/entice/actions?query=workflow%3A%22GitHub+CI%22)
[![Travis Build Status](https://travis-ci.com/vtorri/entice.svg?branch=master)](https://travis-ci.com/github/vtorri/entice)
[![Coverity Scan](https://scan.coverity.com/projects/23000/badge.svg)](https://scan.coverity.com/projects/vtorri-entice)

### Entice
##### A simple cross-platform image viewer with basic features written with the EFL (Enlightement Fundation Libraries)

### License:

This application is released under the BSD 2-Clause License

### Requirements:

The EFL.

### Supported images

The following image formats (with the corresponding file extension) are
supported, if they are supported by the EFL:

Image format  |  File extension
------------  |  --------------
 avif         | .avif, .avifs, .avis
 bmp          | .bmp
 dds          | .dds
 gif          | .gif
 heif         | .heif
 ico          | .ico, .cur
 jp2k         | .jp2k, .jp2,.jpx, .jpf
 jpeg         | .jpg, .jpeg, .jfif
 pmaps        | .pbm, .pgm, .ppm, .pnm
 png          | .png
 psd          | .psd
 raw          | a lot..
 svg          | .svg, .svg.gz
 tga          | .tga
 tgv          | .tgv
 tiff         | .tiff
 wbmp         | .wbmp
 webp         | .webp
 xcf          | .xcf
 xpm          | .xpm

### Animated images

Animated images, like gif or avif, can also be played.

This feature is enabled by default and can be disabled in settings

### Shortcuts

Action | Shortcut
------ | --------
Quit   | Ctrl-q
Toggle fullscreen | F11 or f
Next image | Space or right arrow
Previous image|  BackSpace or left arrow
First image | Alt-home
Last image | Alt-end
rotation clockwise |  Ctrl-r
rotation counter-clockwise | Ctrl-Alt-r
Original size | Ctrl-0
Best fit | b
Zoom in | +
Zoom out | -
Manage Settings | s
Show EXIF data | e
Copy file in clipboard | Ctrl-c
Copy file name in clipboard | Ctrl-Shift-c
Exit Settings or Exif panels | Esc

### Mouse

Action | Shortcut
------ | --------
fullscreen | double click
Open Settings | right click
Exit Settings | right click
Zoom in/out | Ctrl-mouse wheel
