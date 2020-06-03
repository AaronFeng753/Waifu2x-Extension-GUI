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
v2.43.06-beta:
- 在线更新捐赠二维码
- 更新python扩展,精简体积,提高稳定性.
v2.43.05-beta:
- 修复bug:某些情况下无法正常检查更新.
v2.43.04-beta:
- 改进自动检查更新流程.
- 更新ImageMagick.
v2.43.03-beta:
- 优化多线程.
- 更新FFmpeg.
- 添加tooltip.
v2.43.02-beta:
- 优化多线程调度机制.
v2.43.01-beta:
- 新特性:支持预处理图片以兼容更多图片格式.(附加设置,可选,默认禁用)
- 更新ImageMagick.
- 更新srmd-ncnn-vulkan.
- 更新realsr-ncnn-vulkan.
- 性能优化.
v2.42.01-beta:
- 更新waifu2x-ncnn-vulkan.
- 当启用视频自定义设置后,可以将单个设置项留空.
- 当启用Anime4K引擎处理图片时,弹出警告.
- 为每个引擎添加单独介绍.
- 修改Tooltip.
- 修正兼容测试结果输出时的文字错误.
- 性能优化.
- 修复bug.
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
- Anime4KCPP v1.8.1
- ffmpeg version 4.2.3
---
Icons made by : Freepik (https://www.flaticon.com/authors/freepik) From Flaticon : https://www.flaticon.com/
*/
