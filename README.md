[![Entice Logo](https://github.com/vtorri/entice/raw/master/data/icons/entice.png)](https://github.com/vtorri/entice)

[![Github Build status](https://github.com/vtorri/entice/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/vtorri/entice/actions?query=workflow%3A%22GitHub+CI%22)
[![Travis Build Status](https://travis-ci.com/vtorri/entice.svg?branch=master)](https://travis-ci.com/github/vtorri/entice)
[![Coverity Scan](https://scan.coverity.com/projects/23000/badge.svg)](https://scan.coverity.com/projects/vtorri-entice)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/c2497520f8e444bca108086993b41e81)](https://www.codacy.com/gh/vtorri/entice/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=vtorri/entice&amp;utm_campaign=Badge_Grade)
[![Total alerts](https://img.shields.io/lgtm/alerts/g/vtorri/entice.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/vtorri/entice/alerts/)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/vtorri/entice.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/vtorri/entice/context:cpp)

### Entice
#### A simple cross-platform image viewer with basic features written with the EFL (Enlightement Foundation Libraries)

### License

This application is released under the BSD 2-Clause License

### Requirements

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
Quit   | Esc (when no panel is shown), or q, or Ctrl-q
Toggle fullscreen | F11 or f
Next image | space, or right arrow, or down arrow, or Next key
Previous image|  backspace, or left arrow, or up arrow, or Prior key
First image | home, or Alt-home
Last image | end, or Alt-end
rotation clockwise |  Ctrl-r
rotation counter-clockwise | Ctrl-Alt-r
Original size | Ctrl-0
Best fit | b
Zoom in | +, or =
Zoom out | -
Resize window to real image size (implies zoom 100%) | n
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

### TODO

- [ ]  help panel
- [ ]   about panel
- [ ] image list
- [ ] browser mode
