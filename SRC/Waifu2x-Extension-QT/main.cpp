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
v2.44.01-beta:
- 新特性:为Anime4K引擎加入ACNet支持.
- 更新Anime4K.
- 支持将重试次数设定为0(即不重试).
- 修复bug:重试次数比显示的少一次.
- 修复bug:即使没有进行重试依旧在文本框内弹出重试提示.
- 修复bug:处理图片失败时,没有删除预处理图片时生成的缓存.
--------------------------------------------------------------------------
To do:
- 最小化到托盘
- 多gpu支持
- 升级更新时保存设置
- pdf格式支持
- 等caffe跟着稳定走了一段时间确定稳定后, 把自动调整引擎设置的策略改成先caffe gpu 后 vulkan
---------------------------------------------------------------------------
Integrated component(In releases):
- gifsicle version 1.92
- SoX 14.4.2-win32
- waifu2x-caffe 1.2.0.2
- Waifu2x-converter e7de04d
- waifu2x-ncnn-vulkan 20200530(release)
- SRMD-ncnn-Vulkan 20200530(release)
- realsr-ncnn-vulkan 20200530(release)
- ImageMagick 7.0.10-15-Q16-x64
- Anime4KCPP v2.0.0
- ffmpeg version 4.2.3
---
Icons made by : Freepik (https://www.flaticon.com/authors/freepik) From Flaticon : https://www.flaticon.com/
*/
