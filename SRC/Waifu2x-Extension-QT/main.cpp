/*
    Copyright (C) 2020  Aaron Feng

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

v2.57.22-beta:
- Update Anime4KCPP,FFmpeg&FFprobe,ImageMagick.
- Improve performance and compatibility.
- Modify and rebuild Anime4KCPP(2aecb53) to fix compatibility issues.
- Fix bug: Image with alpha channel will still be reprocessed even the current engine doesn't support processing alpha channel.

--------------------------------------------------------------------------
To do:
- 在Anime4kCCP更新release后,删除强制启用opencl的语句( -M opencl )
- 移植到Linux.
---------------------------------------------------------------------------
Integrated component:
- gifsicle version 1.92
- SoX 14.4.2-win32
- waifu2x-caffe 1.2.0.4
- Waifu2x-converter e7de04d
- waifu2x-ncnn-vulkan 20200818
- SRMD-ncnn-Vulkan 20200818
- realsr-ncnn-vulkan 20200818
- ImageMagick 7.0.10-43-portable-Q16-x64
- Anime4KCPP 2aecb5348a423527e9e4016be9c83815fa3e5777
- FFmpeg&FFprobe 2020-11-25-git-3de3d2f5e2-full_build
- NirCmd v2.86
- Ghostscript 9.53.3
---------------------------------------------------------------------------
Icons made by :
Freepik (https://www.flaticon.com/authors/freepik) From Flaticon : https://www.flaticon.com/
Roundicons (https://www.flaticon.com/authors/roundicons) From Flaticon : https://www.flaticon.com/
Icongeek26 (https://www.flaticon.com/authors/Icongeek26) From Flaticon : https://www.flaticon.com/
*/
