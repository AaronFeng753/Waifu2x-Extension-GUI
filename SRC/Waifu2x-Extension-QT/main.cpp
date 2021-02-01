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

v2.62.13:
- Improve the performance of SRMD-NCNN-Vulkan.
- Fix bug: Incorrect scale ratio when using SRMD-NCNN-Vulkan.
- Recent changes:
- Speed up the processing of video and gif.
- Abandon python extension to improve performance.
- Improve download speed, video processing flow, multithreading performance and other performance improvements.
- More detailed tool tips, fix typo.
- Fix bug: Cannot process variable frame rate video properly.
- Fix bug: After the compatibility test is over, the waifu2x-caffe process will stay in the background.

- 改进SRMD-NCNN-Vulkan的性能.
- 修复错误: 使用SRMD-NCNN-Vulkan时的放大倍率错误.
- 近期更新:
- 加快视频和gif的处理速度。
- 替换掉python扩展以提高性能。
- 改进下载速度, 视频处理流程, 多线程性能以及其他性能改进。
- 更详尽的工具提示，修正错字。
- 修复错误：无法正确处理可变帧率视频。
- 修复错误：兼容性测试结束后，waifu2x-caffe进程可能滞留在后台。


--------------------------------------------------------------------------
To do:
- 当单显卡时整个文件夹一起处理.
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
