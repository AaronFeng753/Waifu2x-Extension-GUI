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
v2.56.68-beta:
- 新特性:文件列表右键菜单支持应用与取消自定义分辨率设定.
- 删除会误导用户的提示.
- 为部分按钮添加图标.
- 改进UI布局.
- 性能优化
- 为一些部件添加了无效时自动禁用.
- 改进将文件移动到输出文件夹并遇到同名文件时添加后辍的方式.
v2.56.61-beta:
- 新特性:将文件输出到指定路径时可以选择保留上级文件夹.
- 优化代码结构.
- 性能优化.
- 修复bug:未自动禁用无效选项.
--------------------------------------------------------------------------
To do:
- 移植到Linux.
---------------------------------------------------------------------------
Integrated component:
- gifsicle version 1.92(release)
- SoX 14.4.2-win32(release)
- waifu2x-caffe 1.2.0.4
- Waifu2x-converter e7de04d
- waifu2x-ncnn-vulkan 20200818
- SRMD-ncnn-Vulkan 20200818
- realsr-ncnn-vulkan 20200818
- ImageMagick 7.0.10-30
- Anime4KCPP v2.3.0
- FFmpeg&FFprobe 2020-09-20-git-ef29e5bf42
- NirCmd v2.86
---
Icons made by :
Freepik (https://www.flaticon.com/authors/freepik) From Flaticon : https://www.flaticon.com/
Roundicons (https://www.flaticon.com/authors/roundicons) From Flaticon : https://www.flaticon.com/
*/
