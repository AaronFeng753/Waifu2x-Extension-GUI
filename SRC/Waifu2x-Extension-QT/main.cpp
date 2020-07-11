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
- 更新QT版本至5.15.0,并使用新版QT重新编译.
- 性能优化.
- 修复bug.
----------------------------
Detailed change log:
v2.54.01-beta:
- 更新QT版本至5.15.0,并使用新版QT重新编译.
- 内置QT-5.15.0的许可协议文件.
- 删除无用变量.
- 使用QRandomGenerator代替qsrand和qrand.
- 改进检查更新的流程.
- 完善翻译文档.
- 修复bug:在任务栏图标的右键菜单中,点击退出,并点击弹窗中的[否]选项将导致弹窗重复弹出多次.
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
- waifu2x-ncnn-vulkan 3172fc3f73be8e0a7d1c73cc7e6c8ec92783f27d
- SRMD-ncnn-Vulkan 20200606(release)
- realsr-ncnn-vulkan cdda5aa4c5c3f6c9628c043bab4fb863b3ec9ec4
- ImageMagick 7.0.10-23 Q16 x64 2020-07-04
- Anime4KCPP v2.2.1
- FFmpeg&FFprobe 20200628-4cfcfb3-win64-static
---
Icons made by : Freepik (https://www.flaticon.com/authors/freepik) From Flaticon : https://www.flaticon.com/
*/
