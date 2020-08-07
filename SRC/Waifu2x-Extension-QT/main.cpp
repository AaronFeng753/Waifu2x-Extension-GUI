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
Brief change log:
- Anime4k引擎在启用HDN后可支持调节降噪等级.
- 为Anime4k引擎添加自动检测Alpha通道的支持.(新版Anime4k引擎支持保留Alpha通道)
- 为[文件列表]添加右键菜单选项:[打开选中文件],[从列表移除所选文件]和[打开选中文件的文件夹]
- 为[输出路径]添加右键菜单以打开输出路径文件夹.
- 更新FFmpeg&FFprobe.
- 更新&适配新版Anime4KCPP(v2.3.0).
- 修复多个bug.
- 其他改进.
----------------------------
Detailed change log:
v2.56.01-beta:
- Anime4k引擎在启用HDN后可支持调节降噪等级.
- 为Anime4k引擎添加自动检测Alpha通道的支持.(新版Anime4k引擎支持保留Alpha通道)
- 更新FFmpeg&FFprobe.
- 更新&适配新版Anime4KCPP(v2.3.0).
- 为[文件列表]添加右键菜单选项:[从列表移除所选文件]
- 改进文件列表的右键菜单.
- 修复bug:修复文件列表右键菜单的逻辑bug.
v2.55.32-beta:
- 更新FFmpeg&FFprobe.
- 补全多语言翻译.
- 为文件列表的右键菜单项添加错误提示.
- 删除多余的代码.
v2.55.31-beta:
- 为[文件列表]添加右键菜单选项:[打开选中文件]和[打开选中文件的文件夹]
- 为[输出路径]添加右键菜单以打开输出路径文件夹.
- 修复bug:如果将程序放在外部存储设备中,会无法显示编码器列表.
- 修复bug:特殊情况下无法判断文件夹是否存在.
--------------------------------------------------------------------------
To do:
- 移植到Linux.
---------------------------------------------------------------------------
Integrated component:
- gifsicle version 1.92(release)
- SoX 14.4.2-win32(release)
- waifu2x-caffe 1.2.0.3
- Waifu2x-converter e7de04d
- waifu2x-ncnn-vulkan 8b5c1cdfcde4b6da5127d60b11a1242ddc65bbb8
- SRMD-ncnn-Vulkan 04f93f628b12db5f24d5e46b6433fac728093305
- realsr-ncnn-vulkan fcaae13be5a9edf1f7e49035074c89eb75ec242b
- ImageMagick 7.0.10-25-portable-Q16-x64
- Anime4KCPP v2.2.1
- FFmpeg&FFprobe 20200807-fab00b0-win64
---
Icons made by : Freepik (https://www.flaticon.com/authors/freepik) From Flaticon : https://www.flaticon.com/
*/
