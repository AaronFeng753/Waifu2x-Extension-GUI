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

v3.11.03-beta:
- Add failure detection for frame interpolation.
- Remove temporary code for debug.
- Fix bug: If frame interpolation failed in the middle of processing video by segment, clip with wrong frame rate gonna be generated.

v3.11.02-beta:
- New Feature: Video frame interpolation using cain-ncnn-vulkan(@nihui).
- Add tool tips.
- Fix bug: Video frame interpolation engines won't work when there are special characters in the installation path.
- Fix bug: Video frame interpolation setting is not saved in the video configuration file.

v3.11.01-beta:
- New Feature: Video frame interpolation using rife-ncnn-vulkan(@nihui).
- Update Waifu2x-NCNN-Vulkan,SRMD-NCNN-Vulkan,RealSR-NCNN-Vulkan.
- Fix bug: Failure detection for NCNN-Vulkan engines doesn't work as intended when processing GIF and Video.

- 新特性: 使用rife-ncnn-vulkan(@nihui)对视频进行插帧.
- 更新Waifu2x-NCNN-Vulkan,SRMD-NCNN-Vulkan,RealSR-NCNN-Vulkan.
- 修复bug: 處理GIF和視頻時，NCNN-Vulkan引擎的故障檢測無法正常工作。

--------------------------------------------------------------------------
To do:
- Frame Interpolation
- 在SRMD和REALSR正式加入cpu支持后, 在GPU ID tooltip上添加[★ "-1" is CPU,others are GPUs ★]
- 将文件列表的状态替换为预设的QString
---------------------------------------------------------------------------
Integrated component:
- gifsicle version 1.92
- SoX 14.4.2-win32
- waifu2x-caffe 1.2.0.4
- Waifu2x-converter 9e0284ae23d43c990efb6320253ff0f1e3776854
- Waifu2x-NCNN-Vulkan 20210210
- SRMD-NCNN-Vulkan 20210210
- RealSR-NCNN-Vulkan 20210210
- ImageMagick 7.0.10-61-portable-Q16-x64
- Anime4KCPP v2.5.0
- FFmpeg&FFprobe 2021-02-02-git-2367affc2c-full_build
- NirCmd v2.86
- Ghostscript 9.53.3
- GNU Wget 1.20.3
- rife-ncnn-vulkan 20210210
- cain-ncnn-vulkan 20210210
---------------------------------------------------------------------------
Icons made by :
Freepik (https://www.flaticon.com/authors/freepik) From Flaticon : https://www.flaticon.com/
Roundicons (https://www.flaticon.com/authors/roundicons) From Flaticon : https://www.flaticon.com/
Icongeek26 (https://www.flaticon.com/authors/Icongeek26) From Flaticon : https://www.flaticon.com/
*/
