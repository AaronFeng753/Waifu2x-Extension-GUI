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
- 新特性:[添加文件]按钮支持选中并添加多个文件.
- 新特性:加入[禁止Gitee]选项,启用本选项将禁止本软件从码云下载文件.
- 更新ImageMagick,Python扩展.
- 性能优化.
- 修复多个bug.
----------------------------
Detailed change log:
v2.53.02-beta:
- 新特性:加入[禁止Gitee]选项,启用本选项将禁止本软件从码云下载文件.(可能影响自动检查更新)
- 性能优化,减少对python扩展的多余调用.
- 在文本框内展示当前正在进行的联网活动.
- 更新Python扩展,优化性能.
- 将[找不到已保存的文件列表]的提示改为弹窗展示.
- 更改檢查更新按鈕的網頁打開順序,以及不再跳轉到latest release頁面而是release頁面.
- 為所有插件和引擎添加對應的許可協議文件.
- 添加隐私政策的本地文件
v2.53.01-beta:
- 新特性:[添加文件]按钮支持选中并添加多个文件.
- 性能优化.
- 更新ImageMagick.
- 修复bug:因未按照格式添加扩展名而引起的潜在bug.
- 修复bug:双击文件列表内文件导致选中状态错误.
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
- ImageMagick 7.0.10-23 Q16 x64 2020-07-04
- Anime4KCPP v2.2.1
- FFmpeg&FFprobe 20200628-4cfcfb3-win64-static
---
Icons made by : Freepik (https://www.flaticon.com/authors/freepik) From Flaticon : https://www.flaticon.com/
*/
