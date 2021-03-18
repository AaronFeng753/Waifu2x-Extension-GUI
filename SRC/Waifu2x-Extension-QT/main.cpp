/*
    Copyright (C) 2021  Aaron Feng

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    My Github homepage: https://github.com/AaronFeng753
*/

#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);//高分辨率屏幕支持
    QApplication a(argc,argv);
    a.setQuitOnLastWindowClosed(false);//隐藏无窗口时保持运行
    MainWindow *w = new MainWindow;
    w->show();
    return a.exec();
}

/*
Change log:
******************** !!!补全翻译!!! ***********************
v3.21.13:
- Complete multi-language translation.
- Fix bug: Turn off screen button doesn't work.
- Fix bug: compatibility test progress bar not working normally.

v3.21.02-beta:
- New Feature: [Multiple of FPS] adjustment is now available for all video frame interpolation engines.
- Improve GUI.
- Fix bug: Potential video cache conflict.
- Fix bug: Unable to generate a video when multiple of fps is higher then 2.

v3.21.01-beta:
- New Frame Interpolation engine: DAIN-NCNN-Vulkan.
- New Feature: Multiple of frames.(Only available when using DAIN-NCNN-Vulkan.)
- Improve GUI.
- Fix bug: Unable to generate a video when frame interpolation failed.

v3.11.33-beta:
- New Feature: Now you can choose when you wanna interpolate the frames, before or after upscaled the frames.(Thanks to @CAFUCSteven for suggestion)
- New options: [Start], [Pause] and [Donate] for system tray menu.
- Add icons for system tray menu.
- Fix bug: The [Background mode] in system tray menu didn't work when frame interpolation is enabled.
- Fix bug: Video cache will be abandoned when frame interpolation setting is changed, even though [Process video by segment] is disabled.
- Fix bug: False error report when pause.

v3.11.32-beta:
- New options: [About] and [Send feedback] for system tray menu.
- Better naming for cache files.
- Optimize performance.
- Update FFmpeg&FFprobe,ImageMagick.
- Fix typo.

v3.11.31-beta:
- New Feature: Show summary after processing all files.
- New Feature: Now you can save multiple files lists.
- No longer need to fix the suffix of mp4 files.
- Optimize performance.
- Remove 日本語 translation.

--------------------------------------------------------------------------
To do:
- 在SRMD和REALSR正式加入cpu支持后, 在GPU ID tooltip上添加[★ "-1" is CPU,others are GPUs ★]
---------------------------------------------------------------------------
Integrated component:
- gifsicle version 1.92
- SoX 14.4.2-win32
- waifu2x-caffe 1.2.0.4
- Waifu2x-converter 9e0284ae23d43c990efb6320253ff0f1e3776854
- Waifu2x-NCNN-Vulkan 20210210
- SRMD-NCNN-Vulkan 20210210
- RealSR-NCNN-Vulkan 20210210
- ImageMagick 7.0.11-3-portable-Q16-x64
- Anime4KCPP v2.5.0
- FFmpeg&FFprobe 2021-03-09-git-c35e456f54
- NirCmd v2.86
- Ghostscript 9.53.3
- GNU Wget 1.20.3
- rife-ncnn-vulkan 20210227
- cain-ncnn-vulkan 20210210
---------------------------------------------------------------------------
Icons made by :
Freepik (https://www.flaticon.com/authors/freepik) From Flaticon : https://www.flaticon.com/
Roundicons (https://www.flaticon.com/authors/roundicons) From Flaticon : https://www.flaticon.com/
Icongeek26 (https://www.flaticon.com/authors/Icongeek26) From Flaticon : https://www.flaticon.com/
*/
