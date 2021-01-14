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

v2.59.03-beta:
- Automatically disable [Save Files List] button when files list is empty.
- Other improvements to the GUI.
- Edit tool tip and Chinese translation.

v2.59.02-beta:
- NEW Feature: Reset Files List scale.
- NEW Feature: File count for each kind of file.
- New icons.
- Performance optimization.
- Other improvements to the GUI.
- Complete translation.
- Fix bug: [Add Files] button is not disabled when loading Files list.
- Fix bug: Horizontal header data of the Files List will lost after using the right-click menu of [Remove item].

v2.59.01-beta:
- NEW Feature: Now you can hide components easier by drag the handle.
- Improve GUI design.

v2.58.14-beta:
- NEW Feature: Now you can adjust the horizontal width of the file list.
- Move [Move to Recycle Bin] option to Right-click menu of [Delete original files]
- Other improvements to the GUI.
- Notify users GPU ID list is empty when they try to enable Multi-GPU.
- Optimize performance.

--------------------------------------------------------------------------
To do:
- 在SRMD和REALSR正式加入cpu支持后, 在GPU ID tooltip上添加[★ "-1" is CPU,others are GPUs ★]
- 在Anime4kCPP更新release后,删除强制启用opencl的语句( -M opencl )
---------------------------------------------------------------------------
Integrated component:
- gifsicle version 1.92
- SoX 14.4.2-win32
- waifu2x-caffe 1.2.0.4
- Waifu2x-converter 9e0284ae23d43c990efb6320253ff0f1e3776854
- waifu2x-ncnn-vulkan 20210102
- SRMD-ncnn-Vulkan 20200818
- realsr-ncnn-vulkan 20200818
- ImageMagick 7.0.10-56-portable-Q16-x64
- Anime4KCPP b81d3fc2fd17b86c31ea3e9c24b9edce519cf974
- FFmpeg&FFprobe 2021-01-05-git-66deab3a26-full_build
- NirCmd v2.86
- Ghostscript 9.53.3
---------------------------------------------------------------------------
Icons made by :
Freepik (https://www.flaticon.com/authors/freepik) From Flaticon : https://www.flaticon.com/
Roundicons (https://www.flaticon.com/authors/roundicons) From Flaticon : https://www.flaticon.com/
Icongeek26 (https://www.flaticon.com/authors/Icongeek26) From Flaticon : https://www.flaticon.com/
*/
