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
v1.62.4:
- 修复bug:因为没有精确读取帧率,所以输出的长视频会音画不同步.
--------------------------------------------------------------------------
To do:
- 多gpu支持
- 改进剩余时间算法
- 用python写一个视频的特殊字符路径检测(路径输入到python ext 让他看能不能判断exist)
- 放大视频时, 判断磁盘空间够不够, 提取十个帧取平均值然后计算用量
---------------------------------------------------------------------------
完整更新日志:
- 新特性:waifu2x-ncnn-vulkan新增了fp16p支持,改善了对一些老驱动的兼容性.
- 更新waifu2x-ncnn-vulkan(version 20200414).
- 更新ImageMagick(7.0.10-6-Q16-x64).
- 将默认的自定义分辨率的纵横比策略更改为保持纵横比.
- 修复bug:某些情况下的界面显示错误.
- 修复bug:当源视频文件名称内包含中文时,分段处理视频后无法正确的组装视频.

*/

/*
Integrated component(In releases):
- waifu2x-ncnn-vulkan version 20200223
- Anime4K Java v0.9 Beta
- ffmpeg version 4.2.2
- gifsicle version 1.92
- Waifu2x-converter version: v5.3.3
- ImageMagick 7.0.10-6-Q16-x64
- SoX 14.4.2-win32
*/

/*
Icons made by : Freepik (https://www.flaticon.com/authors/freepik) From Flaticon : https://www.flaticon.com/
*/
