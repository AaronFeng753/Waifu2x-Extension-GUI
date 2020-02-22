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
    QApplication a(argc, argv);
    MainWindow *w = new MainWindow;
    w->show();
    return a.exec();
}

/*
Change log:
- 新特性:支持更多视频相关的自定义参数
- 新特性:初次启动时显示提示
- 更新 waifu2x-ncnn-vulkan (20200222)
- 界面布局调整
- 增强视频画质
- 修复bug:停止处理时没有删除软件生成的临时mp4视频
- 修复bug:启用waifu2x-ncnn-vulkan时,无法正确计算目标放大倍数 (感谢 slimekingwin8cn 反馈)
--------------------------------------------------------------------------
To do:
- 优化计算自定义分辨率所需放大倍数的函数
- 移动到回收站
- 改进剩余时间算法
- 用python写一个视频的特殊字符路径检测(路径输入到python ext 让他看能不能判断exist)
- 放大视频时, 判断磁盘空间够不够, 提取十个帧取平均值然后计算用量
- tableview看能不能自动适配大小的同时, 还让用户自主调节
- 扫描子文件夹
- 隐藏到托盘
*/

/*
Integrated component(In releases):
- waifu2x-ncnn-vulkan version 20200222
- Anime4K Java v0.9 Beta
- ffmpeg version 4.2.2
- gifsicle version 1.92
- Waifu2x-converter version: v5.3.3
- ImageMagick 7.0.9-22-Q16-x64
*/

/*
Icons made by : Freepik (https://www.flaticon.com/authors/freepik) From Flaticon : https://www.flaticon.com/
*/
