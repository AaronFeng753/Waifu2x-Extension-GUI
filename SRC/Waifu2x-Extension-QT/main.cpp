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
v2.41.01-beta:
- 新特性:新增realsr-ncnn-vulkan引擎,大幅提升对3D写实风格图像的处理效果(感谢 @CAFUCSteven 协助测试软件).
- 新特性:除Anime4k外的引擎均支持自动识别并处理Alpha通道图片.
- 更新waifu2x-ncnn-vulkan引擎.
- 更新srmd-ncnn-vulkan引擎.
- 调整对带有Alpha通道的图片的处理流程.
- 性能优化.
- 添加一些提示,修改一些文本,修正错字.
- 调整默认设置.
- 修复若干bug.
- 其他改进.
v2.32.02-beta:
- 调整界面.
- 添加提示.
- 修正文本错误.
v2.32.01-beta:
- 更新ImageMagick(7.0.10-14-Q16-x64).
- 优化检查更新的流程.
- 性能优化.
--------------------------------------------------------------------------
To do:
- 多gpu支持
- 升级更新时保存设置
- pdf格式支持
- 等caffe跟着稳定走了一段时间确定稳定后, 把自动调整引擎设置的策略改成先caffe gpu 后 vulkan
---------------------------------------------------------------------------
Integrated component(In releases):
- Anime4KCPP v1.8.1
- ffmpeg version 4.2.2
- gifsicle version 1.92
- waifu2x-ncnn-vulkan version 20200223
- SRMD-ncnn-Vulkan version 20200224
- Waifu2x-converter version: v5.3.3
- ImageMagick 7.0.10-14-Q16-x64
- SoX 14.4.2-win32
- waifu2x-caffe 1.2.0.2
---
Icons made by : Freepik (https://www.flaticon.com/authors/freepik) From Flaticon : https://www.flaticon.com/
*/
