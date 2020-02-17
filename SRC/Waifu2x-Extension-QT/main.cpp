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
- 新特性:自定义".jpg"文件压缩后的图像质量
- 完善浏览文件对话框的文件扩展名过滤
- 更新ImageMagick(7.0.9-22-Q16-x64)
- 提高稳定性
- 完善简体中文支持
- 其他改进
- 修复bug:程序关闭后滞留后台
--------------------------------------------------------------------------
To do:
- 完善浏览文件对话框
- gif看看能不能组装的时候调整大小
- 视频看看能不能组装时统一调整大小
- 移动到回收站
- 潜在的:stop后有的qprocess无法退出
- 性能优化(重写函数加入return)
- 提高稳定性
- 改进剩余时间算法
- 检测当前所在路径是否要启动管理员权限
- 写 user guide
- 写注释
- 用python写一个视频的特殊字符路径检测(路径输入到python ext 让他看能不能判断exist)
- 放大视频时, 判断磁盘空间够不够, 提取十个帧取平均值然后计算用量
- 分辨率自定义增加apply to all和cancel all
- tableview看能不能自动适配大小的同时, 还让用户自主调节
- 扫描子文件夹
*/

/*
Integrated component(In releases):
- waifu2x-ncnn-vulkan version Nov 4, 2019
- Anime4K Java v0.9 Beta
- ffmpeg version 4.2.2
- gifsicle version 1.92
- Waifu2x-converter version: 2015-11-30T02:17:24
- ImageMagick 7.0.9-12-portable-Q16-x64
*/

/*
Icons made by : Freepik (https://www.flaticon.com/authors/freepik) From Flaticon : https://www.flaticon.com/
*/
