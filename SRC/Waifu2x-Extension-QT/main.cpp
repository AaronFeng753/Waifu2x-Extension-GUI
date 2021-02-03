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

v2.71.02-beta:
- Improve the efficiency, speed and stability of Waifu2x-NCNN-Vulkan when processing Video and GIF.
- The old version of waifu2x-ncnn-vulkan engine will no longer support the use of multiple graphics cards to process GIF and video.
- Fix bug: When custom resolution and video settings are enabled and the video stream bit rate is left 0, the video stream bit rate cannot be calculated normally.
- Fix bug: Cannot process video and gif properly when there is only one frame.
- Fix bug: Scale ratio might be incorrect when using srmd-ncnn-vulkan.

v2.71.01-beta:
- Improve the efficiency, speed and stability of Anime4k, Waifu2x-Caffe and Waifu2x-Converter when processing Video and GIF.
- GUI fix.

--------------------------------------------------------------------------
To do:
- vulkan整个文件夹一起处理.

-t tile-size         tile size (>=32/0=auto, default=0) can be 0,0,0 for multi-gpu
  -g gpu-id            gpu device to use (default=0) can be 0,1,2 for multi-gpu
  -j load:proc:save    thread count for load/proc/save (default=1:2:2) can be 1:2,2,2:2 for multi-gpu

获取基础倍率
按照放大倍数循环
根据线程和显卡分配参数
开跑
跑完了,复制scale到split
#w2xVulkan only# 判断当前用的版本,如果为老旧的则读取split文件名列表,把带有两个"."的文件都改名成basename
继续循环
循环结束

视频和gif里添加判断当前是否用的vulkan,如果是则判断当前放大倍数是否需要修改分辨率

- 在SRMD和REALSR正式加入cpu支持后, 在GPU ID tooltip上添加[★ "-1" is CPU,others are GPUs ★]
- 将文件列表的状态替换为预设的QString
---------------------------------------------------------------------------
Integrated component:
- gifsicle version 1.92
- SoX 14.4.2-win32
- waifu2x-caffe 1.2.0.4
- Waifu2x-converter 9e0284ae23d43c990efb6320253ff0f1e3776854
- waifu2x-ncnn-vulkan 20210102
- SRMD-ncnn-Vulkan 20200818
- realsr-ncnn-vulkan 20200818
- ImageMagick 7.0.10-59-portable-Q16-x64
- Anime4KCPP v2.5.0
- FFmpeg&FFprobe 2021-01-24-git-1775688292-full_build
- NirCmd v2.86
- Ghostscript 9.53.3
- GNU Wget 1.20.3
---------------------------------------------------------------------------
Icons made by :
Freepik (https://www.flaticon.com/authors/freepik) From Flaticon : https://www.flaticon.com/
Roundicons (https://www.flaticon.com/authors/roundicons) From Flaticon : https://www.flaticon.com/
Icongeek26 (https://www.flaticon.com/authors/Icongeek26) From Flaticon : https://www.flaticon.com/
*/
