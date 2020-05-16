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
v2.11.1:
- 更新ImageMagick(7.0.10-12-Q16-x64)
- 更改默认设置.
- 添加一些提示.
- 处理文件前检测是否拥有文件所在目录的写权限.
- 改进软件启动时的权限检测.
- 修复bug:特殊情况下,处理视频时可能会卡在获取时长那一步.
- 修复bug:无法正常检测是否拥有输出路径的写权限.
- 修复bug:当使用Anime4k引擎处理视频时,自定义分辨率设定无效.
--------------------------------------------------------------------------
To do:
- Anime4k引擎加入图片和GIF处理
- 初次启动运行见兼容性检测后，帮助用户自动修改引擎设定。
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
- ImageMagick 7.0.10-12-Q16-x64
- SoX 14.4.2-win32
---
Icons made by : Freepik (https://www.flaticon.com/authors/freepik) From Flaticon : https://www.flaticon.com/
*/
