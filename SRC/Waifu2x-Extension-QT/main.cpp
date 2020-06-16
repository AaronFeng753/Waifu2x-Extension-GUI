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
v2.46.02-beta:
- Add overwrite switch.
- Add tooltip.
- Performance optimization.
- Use isChecked() to replace checkState().
- Fix bug: Potential multi-thread bug when writing files.
v2.46.01-beta:
- 更新FFmpeg.
- 添加tooltip.
- 更改默认设定.
- 删除冗余代码.
--------------------------------------------------------------------------
To do:
- 最小化到托盘
- 多gpu支持
- 升级更新时保存设置
- pdf格式支持
---------------------------------------------------------------------------
Integrated component:
- gifsicle version 1.92(release)
- SoX 14.4.2-win32(release)
- waifu2x-caffe 1.2.0.2(release)
- Waifu2x-converter e7de04d
- waifu2x-ncnn-vulkan 20200606(release)
- SRMD-ncnn-Vulkan 20200606(release)
- realsr-ncnn-vulkan 20200606(release)
- ImageMagick 7.0.10-19 Q16 x64(release)
- Anime4KCPP v2.1.0(release)
- FFmpeg version 20200615-9d80f3e
---
Icons made by : Freepik (https://www.flaticon.com/authors/freepik) From Flaticon : https://www.flaticon.com/
*/
