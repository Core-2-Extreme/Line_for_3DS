**Line for 3DS**

![GitHub all releases](https://img.shields.io/github/downloads/Core-2-Extreme/Line_for_3DS/total?color=purple&style=flat-square)
![GitHub commits since latest release (by SemVer)](https://img.shields.io/github/commits-since/Core-2-Extreme/Line_for_3DS/latest?color=orange&style=flat-square)
![GitHub commit activity](https://img.shields.io/github/commit-activity/m/Core-2-Extreme/Line_for_3DS?color=darkgreen&style=flat-square)

# Index
- [Index](#index)
- [Summary](#summary)
- [How to setup and use](#how-to-setup-and-use)
- [Build](#build)
- [Links](#links)
- [Patch note](#patch-note)
  - [Ver 2.0.0](#ver-200)
- [License](#license)

# Summary

![menu](https://github.com/Core-2-Extreme/Line_for_3DS/blob/v2.0.0.1/screenshots/menu.png)
![chat](https://github.com/Core-2-Extreme/Line_for_3DS/blob/v2.0.0.1/screenshots/chat.png)
|        Feature        | Supported? | Note |
| --------------------- | ---------- | ---- |
| Send text             |   Yes✅   |      |
| Send stickers         |   Yes✅   | Only package ID ***11537***, ***11538*** and ***11539*** in [sticker definitions](https://developers.line.biz/en/docs/messaging-api/sticker-list/#sticker-definitions) are supported for now. |
| Send pictures         |   Yes✅   | Up to 37.5MB per picture. |
| Send videos           |   Yes✅   | Up to 37.5MB per video. |
| Send audio            |   Yes✅   | Up to 37.5MB per audio. |
| Send files            |   Yes✅   | Up to 37.5MB per file. |
| View old chat logs    |   Yes✅   | Up to 4000 chat logs. |
| Auto log update       |   No❌    | Temporary removed due to technical issues. |
| Receive text          |   Yes✅   |      |
| Receive stickers      |   Yes✅   | Only package ID ***11537***, ***11538*** and ***11539*** in [sticker definitions](https://developers.line.biz/en/docs/messaging-api/sticker-list/#sticker-definitions) are displayed for now. |
| Receive pictures      |   Yes✅   | Very large images may not be displayed especially on OLD3DS. |
| Receive audio         |   Yes✅   |      |
| Receive videos        |   Yes✅   | Some videos may not be played at full speed or at all especially on OLD3DS. |
| Direct message        |   Yes✅   |      |
| Group chat            |   Yes✅   | If `Allow bot to join group chat` feature in LINE developer page (see [Create your LINE BOT](https://github.com/Core-2-Extreme/Line_for_3DS/blob/v2.0.0.1/SETUP.md#create-your-line-bot) step 6) is enabled. |
| Open chat             |   No❌    |      |
| Voice call            |   No❌    |      |
| Night mode            |   Yes✅   |      |

# How to setup and use
See [Setup instructions](https://github.com/Core-2-Extreme/Line_for_3DS/blob/v2.0.0.1/SETUP.md).

# Build
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
gcc version 13.2.0 (devkitARM release 62)
```
Make sure you have release 62. \
(later version may work but sometimes later version has incompatibility) \
If you have older devkitpro, update it or compilation will fail.

* Clone this repository
  * On windows run `build.bat`
  * On other system, type `make` (`make -j` for faster build)

# Links
* [Discord channel](https://discord.gg/EqK3Kpb)
* [GBAtemp](https://gbatemp.net/threads/line-for-3ds.539530)

# Patch note
## Ver 2.0.0
* Line
  * Significant changes for internal code, so you need to setup your Google Apps Script again.
  * [Setup instructions](https://github.com/Core-2-Extreme/Line_for_3DS/blob/v2.0.0.1/SETUP.md) are updated.
  * 3DS
    * Chat log wrapping feature has been improved a lot.
    * Auto update feature (for chat logs) are removed temporary.
    * Clear cache feature (for downloaded files) are removed temporary.
  * Google Apps Script
    * Self tests are added so that it is much easier to trouble shoot.
* Camera
  * Supported QR code scan (currently, only for Google Apps Script setup).
* Video player
  * Updated to v1.5.3 (see [Video player for 3DS patch note](https://github.com/Core-2-Extreme/Video_player_for_3DS/blob/v1.5.3#v153) for details).
* Overall
  * Application folder has been changed from `/Line/` to `/3ds/Line/` (old folder won't be deleted automatically, but you can delete it if you don't need it).
  * Sub application icon has been added.

# License
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
