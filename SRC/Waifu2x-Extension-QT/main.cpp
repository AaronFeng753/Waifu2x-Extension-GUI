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
v2.45.18:
- 新特性:为Anime4k引擎加入HDN模式支持,可改善画质.(启用ACNet后才能启用HDN)
- 新特性:保留原文件名.(可选,默认禁用)
- 更新Anime4K,ImageMagick,FFmpeg.
- 为提示音添加淡出效果.(由 @Michael18751 编辑)
- 性能优化.
- 修复多处Bug.
- 以及其他多项改进.
--------------------------------------------------------------------------
To do:
- 最小化到托盘
- 多gpu支持
- 升级更新时保存设置
- pdf格式支持
---------------------------------------------------------------------------
Integrated component:
- gifsicle version 1.92(release)
- SoX 14.4.2-win32(release)
- waifu2x-caffe 1.2.0.2(release)
- Waifu2x-converter e7de04d
- waifu2x-ncnn-vulkan 20200606(release)
- SRMD-ncnn-Vulkan 20200606(release)
- realsr-ncnn-vulkan 20200606(release)
- ImageMagick 7.0.10-18 Q16 x64(release)
- Anime4KCPP v2.1.0(release)
- ffmpeg version 20200612-38737b3
---
Icons made by : Freepik (https://www.flaticon.com/authors/freepik) From Flaticon : https://www.flaticon.com/
*/
