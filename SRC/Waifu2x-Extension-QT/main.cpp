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
Brief change log:
- 新特性:为Anime4k&Waifu2x-caffe引擎添加验证显卡配置的按钮.
- 更新ImageMagick,FFmpeg&FFprobe,waifu2x-caffe,Anime4k.
- 内置cuDNN.
- 性能优化.
- 修复多处bug.
- 其他多项改进.
----------------------------
Detailed change log:
v2.52.05-beta:
- 更新Anime4k.
- 修改tooltip.
- 在文本框的启动信息中标识当前版本状态.
v2.52.04-beta:
- 更新FFmpeg&FFprobe.
- 优化读取语言文件的流程.
- 更换兼容性测试时使用的GIF和视频.
- 为Anime4k引擎适配强制重试.
- 为LineEdit添加清空按钮.
- 修改部分tooltip.
- 修复bug:错误的弹出捐赠提醒.
- 修复bug:测试时遗留下来的输出语句.
v2.52.03-beta:
- 更新ImageMagick,waifu2x-caffe.
- 内置cuDNN.
v2.52.02-beta:
- 更新FFmpeg&FFprobe.
- 改进Anime4k & waifu2x-caffe多显卡配置的验证.
- 修复英文书写错误.
v2.52.01-beta:
- 新特性:为Anime4k&Waifu2x-caffe引擎添加验证显卡配置的按钮.
- 优化SRMD-ncnn-vulkan处理图像的流程,适配原生3x,4x放大.
- 减少ncnn-vulkan系列引擎运行时可能产生的垃圾缓存.
- 添加tooltip.
- 更新ImageMagick,FFmpeg&FFprobe.
- 改进自动校正.
- 修复bug:Anime4k&Waifu2x-caffe引擎在启用多GPU后,因为未正常读取显卡设定而导致负载无法平均分配.
--------------------------------------------------------------------------
To do:
- pdf格式支持
- 移植到Linux
---------------------------------------------------------------------------
Integrated component:
- gifsicle version 1.92(release)
- SoX 14.4.2-win32(release)
- waifu2x-caffe 1.2.0.3
- Waifu2x-converter e7de04d
- waifu2x-ncnn-vulkan 785328838a7e3c3429d4d4587ba0d00f50cb098f
- SRMD-ncnn-Vulkan 20200606(release)
- realsr-ncnn-vulkan 20200606(release)
- ImageMagick 7.0.10-22-portable-Q16-x64
- Anime4KCPP v2.2.1
- FFmpeg&FFprobe 20200628-4cfcfb3-win64-static
---
Icons made by : Freepik (https://www.flaticon.com/authors/freepik) From Flaticon : https://www.flaticon.com/
*/
