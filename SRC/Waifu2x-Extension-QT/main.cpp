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
v1.61.3:
- 完善多语言翻译.
- 开始处理文件列表时禁止调整分段处理视频相关的设置.
- 当禁用分段处理视频时,同步禁用调整分段时长的spinbox.
- 默认启用分段处理视频.
- 修改捐赠弹窗的计数机制.
- 分段处理视频时显示时长进度.
- 完善tooltip.
v1.61.2:
- 给所有引擎适配了分段处理视频.
- 修复文本错误.
- 停止处理时不再强制终止ffmpeg进程,因为这会导致无法正常生成缓存文件,影响下次继续执行任务.
- 为组装视频片段的函数添加自定义音频编码器&音频比特率支持.
v1.61.1:
- 新特性:分段处理视频,大幅减少处理视频时占用的硬盘空间.(当前仅适配了Anime4k引擎,其他引擎将陆续适配)
v1.51.4:
- 删除冗余代码.
--------------------------------------------------------------------------
To do:
- 多gpu支持
- 改进剩余时间算法
- 用python写一个视频的特殊字符路径检测(路径输入到python ext 让他看能不能判断exist)
- 放大视频时, 判断磁盘空间够不够, 提取十个帧取平均值然后计算用量
---------------------------------------------------------------------------
完整更新日志:
- 新特性:分段处理视频,大幅减少处理视频时占用的硬盘空间.(可选,附加设置,默认启用)
- 完善多语言支持.
- 修复bug:停止处理进程时将强制终止ffmpeg进程,会导致无法正常生成缓存文件,影响下次读取任务进度.
- 其他小改进.
*/

/*
Integrated component(In releases):
- waifu2x-ncnn-vulkan version 20200223
- Anime4K Java v0.9 Beta
- ffmpeg version 4.2.2
- gifsicle version 1.92
- Waifu2x-converter version: v5.3.3
- ImageMagick 7.0.9-27-Q16-x64
- SoX 14.4.2-win32
*/

/*
Icons made by : Freepik (https://www.flaticon.com/authors/freepik) From Flaticon : https://www.flaticon.com/
*/
