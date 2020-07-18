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
- 新特性:为所有引擎加入显示当前多显卡设定的按钮.
- 更新FFmpeg&FFprobe.
- 性能优化.
- 修复bug.
----------------------------
Detailed change log:
v2.55.01-beta:
- 新特性:为所有引擎加入显示当前多显卡设定的按钮.
- 更新FFmpeg&FFprobe.
- Anime4k引擎的[列出gpu]改为多线程执行,避免进入未响应状态.
--------------------------------------------------------------------------
To do:
- 把download2单独写成个函数.
- 加入一个tools标签页
- 借助ImageMagick convert,实现拆分大图片,导入文件列表,放大,然后组装.
- 移植到Linux.
---------------------------------------------------------------------------
Integrated component:
- gifsicle version 1.92(release)
- SoX 14.4.2-win32(release)
- waifu2x-caffe 1.2.0.3
- Waifu2x-converter e7de04d
- waifu2x-ncnn-vulkan 54b2b3df1384a7c13bfec431b806cf152f227e91
- SRMD-ncnn-Vulkan 04f93f628b12db5f24d5e46b6433fac728093305
- realsr-ncnn-vulkan fcaae13be5a9edf1f7e49035074c89eb75ec242b
- ImageMagick 7.0.10-23 Q16 x64 2020-07-04
- Anime4KCPP v2.2.1
- FFmpeg&FFprobe 20200716-d11cc74-win64-static
---
Icons made by : Freepik (https://www.flaticon.com/authors/freepik) From Flaticon : https://www.flaticon.com/
*/
