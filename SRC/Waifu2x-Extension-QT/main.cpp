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
v1.73.1:
- 新特性:为Anime4K引擎加入GPU加速支持.
- 新特性:安装软件后,首次启动时弹窗让用户选择自己所使用的语言.
- 更新Anime4K(v1.5.0)
- 不再允许禁用自动检查更新，但是可以禁用自动更新弹窗，禁用弹窗后将在文本框内显示更新提示.
- 兼容测试结束后弹窗提示用户查看测试结果.
- 完善兼容性测试.
--------------------------------------------------------------------------
To do:
- 多gpu支持
- 改进剩余时间算法
- 用python写一个视频的特殊字符路径检测(路径输入到python ext 让他看能不能判断exist)
- 放大视频时, 判断磁盘空间够不够, 提取十个帧取平均值然后计算用量
---------------------------------------------------------------------------
完整更新日志:

*/

/*
Integrated component(In releases):
- waifu2x-ncnn-vulkan version 20200223
- Anime4KCPP v1.3.1
- ffmpeg version 4.2.2
- gifsicle version 1.92
- Waifu2x-converter version: v5.3.3
- ImageMagick 7.0.10-6-Q16-x64
- SoX 14.4.2-win32
*/

/*
Icons made by : Freepik (https://www.flaticon.com/authors/freepik) From Flaticon : https://www.flaticon.com/
*/
