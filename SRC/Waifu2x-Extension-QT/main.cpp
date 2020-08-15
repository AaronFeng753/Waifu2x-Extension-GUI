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
- 添加更多处理完文件后可执行的动作(关机,睡眠,休眠,重启).(感谢 @Michael18751 的建议)
- 性能优化.
- 修复多个bug.
----------------------------
Detailed change log:
v2.56.31-beta:
- 添加更多处理完文件后可执行的动作(关机,睡眠,休眠,重启).(感谢 @Michael18751 的建议)
- 整理代码结构.
v2.56.21-beta:
- 适配新版FFmpeg.
- 修正注释中的错误.
- 性能优化.
- 更新FFmpeg&FFprobe,ImageMagick.
- 修复bug:当未检测到更新时会重复下载更新文件.
- 修复bug:极少数情况下无法正确修正放大倍数的bug.
- 修复bug:即便未勾选保存为jpg,在检测到透明通道后依旧会提示用户将强制保存为png.
--------------------------------------------------------------------------
To do:
- 移植到Linux.
---------------------------------------------------------------------------
Integrated component:
- gifsicle version 1.92(release)
- SoX 14.4.2-win32(release)
- waifu2x-caffe 1.2.0.3
- Waifu2x-converter e7de04d
- waifu2x-ncnn-vulkan 8b5c1cdfcde4b6da5127d60b11a1242ddc65bbb8
- SRMD-ncnn-Vulkan 04f93f628b12db5f24d5e46b6433fac728093305
- realsr-ncnn-vulkan fcaae13be5a9edf1f7e49035074c89eb75ec242b
- ImageMagick 7.0.10-27 Q16 x64 2020-08-10
- Anime4KCPP v2.3.0
- FFmpeg&FFprobe 20200814-a762fd2
- NirCmd v2.86
---
Icons made by : Freepik (https://www.flaticon.com/authors/freepik) From Flaticon : https://www.flaticon.com/
*/
