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
    QApplication a(argc, argv);
    MainWindow *w = new MainWindow;
    w->show();
    return a.exec();
}

/*
Change log:
v2.02.3:
- 更新Anime4K(v1.7.0).
- 改进简体中文支持.
- 改进兼容性测试,减少资源占用.
- 添加一些提示.
- 修复bug:某些情况下,程序会在运行兼容性测试时卡住.
- 修复bug:某些组件未在处理文件时禁用,可能导致潜在错误.
--------------------------------------------------------------------------
To do:
- 出处理视频&GIF时单独显示进度条和剩余时间一类的.(Current Video(GIF),平时隐藏,处理时可见)
- 多gpu支持
---------------------------------------------------------------------------
Integrated component(In releases):
- Anime4KCPP v1.7.0
- ffmpeg version 4.2.2
- gifsicle version 1.92
- waifu2x-ncnn-vulkan version 20200223
- SRMD-ncnn-Vulkan version 20200224
- Waifu2x-converter version: v5.3.3
- ImageMagick 7.0.10-10-Q16-x64
- SoX 14.4.2-win32
---
Icons made by : Freepik (https://www.flaticon.com/authors/freepik) From Flaticon : https://www.flaticon.com/
*/
