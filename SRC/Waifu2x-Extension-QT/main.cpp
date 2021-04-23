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

v3.31.21-beta:
- Performance optimization.
- Update FFmpeg, ImageMagick.
- Some other improvements.

--------------------------------------------------------------------------
To do:
- 添加对"图片文件夹"的支持,一次处理一个文件夹.
- 改进自动调整引擎设定.
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
- ImageMagick 7.0.11-8-portable-Q16-x64
- Anime4KCPP v2.5.0
- FFmpeg 2021-04-20-git-718e03e5f2-full_build
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
