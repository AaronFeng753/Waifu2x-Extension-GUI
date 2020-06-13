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
v2.45.06-beta:
- 修复bug:在Windows 7平台上无法正常拆分视频.
- 改进兼容性测试.
v2.45.05-beta:
- New notification sound with fadeout.(Edited by @Michael18751)
- 更新ImageMagick.
- 更新Anime4K.
- 性能优化.
- 改用ImageMagick处理GIF以改善兼容性.
- 添加提示.
v2.45.04-beta:
- 新特性:为Anime4k引擎加入HDN模式支持,可改善画质.(启用ACNet后才能启用HDN)
- 更新Anime4k引擎.
- 改进稳定性.
v2.45.03-beta:
- 修复bug:无法处理某些特殊的GIF.
v2.45.02-beta:
- 新特性:保留源文件名.(可选,默认禁用)
- 修复bug:当安装路径内有空格时,无法打开本地存储的FFmpeg编码器列表.
v2.45.01-beta:
- 改进GUI.
- 更新ImageMagick.
--------------------------------------------------------------------------
To do:
- 保留原文件名.(1.向移动文件到输出目录的函数传输原文件地址+名称,2.判断是否要保留源文件名,3.在函数内提取源文件名并和输出目录组装代替原来的输出路径.)
- 最小化到托盘
- 多gpu支持
- 升级更新时保存设置
- pdf格式支持
---------------------------------------------------------------------------
Integrated component(In releases):
- gifsicle version 1.92(release)
- SoX 14.4.2-win32(release)
- waifu2x-caffe 1.2.0.2(release)
- Waifu2x-converter e7de04d
- waifu2x-ncnn-vulkan 20200606(release)
- SRMD-ncnn-Vulkan 20200606(release)
- realsr-ncnn-vulkan 20200606(release)
- ImageMagick 7.0.10-18 Q16 x64(release)
- Anime4KCPP f1692754c1b6efaf3ccdb9d4370587751a5bdd3f
- ffmpeg version 4.2.3(release)
---
Icons made by : Freepik (https://www.flaticon.com/authors/freepik) From Flaticon : https://www.flaticon.com/
*/
