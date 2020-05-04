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
v2.01.1:
- 新特性:重新设计用户界面.
- 新特性:添加[繁體中文]支持(感謝 @uimee 協助翻譯).
- 新特性:Anime4K引擎支持指定GPU.
- 新特性:存储与读取所有引擎的GPU设置.
- 新特性:改进兼容性测试,增加检测项目,支持存储测试结果.
- 更新Anime4K(v1.6.0).
- 更新ImageMagick(7.0.10-10-Q16-x64).
- 改进多线程管理.
- 添加一些提示.
- 性能优化.
- 修复bug.
--------------------------------------------------------------------------
To do:
- 多gpu支持
- 用python写一个视频的特殊字符路径检测(路径输入到python ext 让他看能不能判断exist)
---------------------------------------------------------------------------
Integrated component(In releases):
- waifu2x-ncnn-vulkan version 20200223
- Anime4KCPP v1.6.0
- ffmpeg version 4.2.2
- gifsicle version 1.92
- Waifu2x-converter version: v5.3.3
- ImageMagick 7.0.10-6-Q16-x64
- SoX 14.4.2-win32
---
Icons made by : Freepik (https://www.flaticon.com/authors/freepik) From Flaticon : https://www.flaticon.com/
*/
