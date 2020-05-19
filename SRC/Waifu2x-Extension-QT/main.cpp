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
v2.21.01-beta:
- 新特性:处理视频或GIF时,单独显示当前文件的进度(进度条,消耗时间,剩余时间,ETA).
- 性能优化.
- 其他改进.
- 修复bug.
--------------------------------------------------------------------------
To do:
- 发更新后把捐赠页面的 禁用弹窗 教程删掉(因为新版没弹窗了)
- 多gpu支持
- 升级更新时保存设置
---------------------------------------------------------------------------
Integrated component(In releases):
- Anime4KCPP v1.8.1
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
