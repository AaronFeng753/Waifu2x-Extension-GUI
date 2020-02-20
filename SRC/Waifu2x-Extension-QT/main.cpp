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
- 新特性: waifu2x-converter 支持更多降噪等级
- 新特性:新增 waifu2x-converter 引擎自定义设置
- 新特性:自定义字体
- 新特性:最小化窗体
- 新特性:隐藏文本框
- 更新 waifu2x-converter (v5.3.3) (感谢 NyanMisaka 反馈)
- 界面布局调整
- 提高稳定性
- 其他多项改进
- 修复bug:当文件扩展名大写时无法正确识别(感谢 zhehenkaifa 反馈)
- 修复bug:当视频扩展名为".MP4"而非".mp4"时, 会导致错误
- 修复bug:当文件名称内有多余的"."时会导致软件错误(感谢 我叫黄伟达 反馈)
- 修复bug:某些特殊情况下无法停止处理
--------------------------------------------------------------------------
To do:
- 无损改变图片分辨率
- gif看看能不能组装的时候调整大小
- 视频看看能不能组装时统一调整大小
- convert试试double的scale
- 移动到回收站
- 提高稳定性
- 改进剩余时间算法
- 用python写一个视频的特殊字符路径检测(路径输入到python ext 让他看能不能判断exist)
- 放大视频时, 判断磁盘空间够不够, 提取十个帧取平均值然后计算用量
- 分辨率自定义增加apply to all和cancel all
- tableview看能不能自动适配大小的同时, 还让用户自主调节
- 扫描子文件夹
- 隐藏到托盘
*/

/*
Integrated component(In releases):
- waifu2x-ncnn-vulkan version Nov 4, 2019
- Anime4K Java v0.9 Beta
- ffmpeg version 4.2.2
- gifsicle version 1.92
- Waifu2x-converter version: 2015-11-30T02:17:24
- ImageMagick 7.0.9-22-Q16-x64
*/

/*
Icons made by : Freepik (https://www.flaticon.com/authors/freepik) From Flaticon : https://www.flaticon.com/
*/
