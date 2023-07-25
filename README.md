# Line for 3DS
![GitHub all releases](https://img.shields.io/github/downloads/Core-2-Extreme/Line_for_3DS/total?color=purple&style=flat-square)
![GitHub commits since latest release (by SemVer)](https://img.shields.io/github/commits-since/Core-2-Extreme/Line_for_3DS/latest?color=orange&style=flat-square)
![GitHub commit activity](https://img.shields.io/github/commit-activity/m/Core-2-Extreme/Line_for_3DS?color=darkgreen&style=flat-square)

## Index
* [Summary](https://github.com/Core-2-Extreme/Line_for_3DS#summary)
* [How to setup and use](https://github.com/Core-2-Extreme/Line_for_3DS#how-to-setup-and-use)
* [Build](https://github.com/Core-2-Extreme/Line_for_3DS#build)
* [Links](https://github.com/Core-2-Extreme/Line_for_3DS#links)
* [Patch note](https://github.com/Core-2-Extreme/Line_for_3DS#patch-note)
* [License](https://github.com/Core-2-Extreme/Line_for_3DS#license)

## Summary
<img src="https://user-images.githubusercontent.com/45873899/85299815-b9bbfb80-b4e0-11ea-8ee1-7551cedd83d4.jpg" width="400" height="480"> \
<img src="https://user-images.githubusercontent.com/45873899/77538658-676c3e00-6ee3-11ea-8a9a-9c37ee7ba69b.png"> \
[![Video](https://img.youtube.com/vi/4FMCwcwEEi8/0.jpg)](https://www.youtube.com/watch?v=4FMCwcwEEi8)

* Send text ✅
* Send image ✅ (v1.4.0)
* Send video ✅(*2) (v1.4.0)
* Send sound ✅(*2) (v1.4.0)
* Send sticker ✅(*1) (v1.3.0)
* Receive text ✅
* Receive (View) image ✅ (v1.2.0)
* Receive (Play) video ✅ (v1.7.0)
* Receive (Play) sound ✅ (v1.7.0)
* Receive (View) sticker ✅(*1) (v1.3.0)
* View old log ✅ (～v1.5.2 Max 300 logs) (v1.6.0~ Max 4000 logs)
* Auto update ✅ (v0.2.0)
* Group chat ✅
* Night mode ✅ (v0.2.0)
* Password ✅ (v0.3.0)
* Save log to SD card ✅ (v0.3.0)

### *1 The stickers must be included this list.
https://developers.line.biz/media/messaging-api/sticker_list.pdf
### *2 Google drive URL will be sent(not embed).

## How to setup and use
Please follow the guide on [GBAtemp](https://gbatemp.net/threads/line-for-3ds.539530).

## Build
You need : 
* [devkitpro](https://devkitpro.org/wiki/Getting_Started)

If you want to build .cia, then you also need : 
* [bannertool](https://github.com/Steveice10/bannertool/releases) and [makerom](https://github.com/3DSGuy/Project_CTR/releases) (Copy them in your path e.g. in `{devkitPro_install_dir}\tools\bin`).

If you already have devkitpro, type `{devkitPro_install_dir}\devkitARM\bin\arm-none-eabi-gcc -v`. \
You should see something like : 
```
.....
.....
.....
Thread model: posix
Supported LTO compression algorithms: zlib zstd
gcc version 12.2.0 (devkitARM release 60)
```
Make sure you have release 60 or later. \
If you have older devkitpro, update it or compilation will fail.

* Clone this repository
  * On windows run `build.bat`
  * On other system, type `make` (`make -j` for faster build)

## Links
* [Discord channel](https://discord.gg/EqK3Kpb)

## Patch note
### Ver 1.7.2
【Line】 Fixed some bugs. \
【Vid】 Added debug infomation. \
【Vid】 Changed initial image size and position \
【App】 Some minor update to better user experience.

### Ver 1.7.1
【Line】 Added- Supported download all files. \
【Mup/Vid】 Added- Additional formats support(.ogg). \
【App】 Some minor update to better user experience.

### Ver 1.7.0
【App】 Fixed- Fonts was improved. \
【App】 Added- Video player. \
【Line】 Added- Now, you can play audio and video sent by user. \
【Cam】 Fixed- Framerate was improved. \
【Mup】 Added- Additional formats (like aac) are supported. \
【App】 Some minor update to better user experience.

## License
This software is licensed as GNU General Public License v3.0.

Third party libraries are licensed as :

| Library | License |
| ------- | ------- |
| [Base64](https://github.com/ReneNyffenegger/cpp-base64/blob/master/LICENSE) | No specific license name               |
| [citro2d](https://github.com/devkitPro/citro2d/blob/master/LICENSE)         | zlib License                           |
| [citro3d](https://github.com/devkitPro/citro3d/blob/master/LICENSE)         | zlib License                           |
| [curl](https://github.com/curl/curl/blob/master/COPYING)                    | No specific license name               |
| [dav1d](https://github.com/videolan/dav1d/blob/master/COPYING)              | BSD 2-Clause                           |
| [ffmpeg](https://github.com/FFmpeg/FFmpeg/blob/master/COPYING.GPLv2)        | GNU General Public License v2.0        |
| [libctru](https://github.com/devkitPro/libctru#license)                     | zlib License                           |
| [mbedtls](https://github.com/Mbed-TLS/mbedtls/blob/development/LICENSE)     | Apache License 2.0                     |
| [mp3lame](https://github.com/gypified/libmp3lame/blob/master/COPYING)       | GNU Lesser General Public License v2.0 |
| [stb_image](https://github.com/nothings/stb/blob/master/LICENSE)            | Public Domain                          |
| [x264](https://github.com/mirror/x264/blob/master/COPYING)                  | GNU General Public License v2.0        |
| [zlib](https://github.com/madler/zlib/blob/master/LICENSE)                  | zlib License                           |
