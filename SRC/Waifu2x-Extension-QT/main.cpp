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
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    MainWindow *w = new MainWindow;
    w->show();
    return a.exec();
}

/*
Change log:
- 默认启用旧版本的waifu2x-ncnn-vulkan引擎.(因为新版本的waifu2x-ncnn-vulkan存在稳定性问题)
`如果您需要使用waifu2x-ncnn-vulkan引擎的TTA的特性,切换到新版本即可.`
- 提高稳定性.
- 修复bug:一次性添加大量文件时会造成程序无响应.
--------------------------------------------------------------------------
To do:
- 多线程接收输入文件以避免无响应 (改造 FileList_Add)

- 浏览文件记住上一次文件夹
- 利用ffprobe获取视频信息
- 优化计算自定义分辨率所需放大倍数的函数
- 改进剩余时间算法
- 用python写一个视频的特殊字符路径检测(路径输入到python ext 让他看能不能判断exist)
- 放大视频时, 判断磁盘空间够不够, 提取十个帧取平均值然后计算用量
- 尝试减少放大视频所需磁盘容量
- 小数位放大支持
*/

/*
Integrated component(In releases):
- waifu2x-ncnn-vulkan version 20200223
- Anime4K Java v0.9 Beta
- ffmpeg version 4.2.2
- gifsicle version 1.92
- Waifu2x-converter version: v5.3.3
- ImageMagick 7.0.9-22-Q16-x64
*/

/*
Icons made by : Freepik (https://www.flaticon.com/authors/freepik) From Flaticon : https://www.flaticon.com/
*/
