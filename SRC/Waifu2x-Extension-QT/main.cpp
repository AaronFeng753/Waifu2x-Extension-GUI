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
    MainWindow w;
    w.show();
    return a.exec();
}

/*
Change log:
- 新特性:进度条左侧显示[完成数量/总数量].
- 新特性:增加设置选项,控制是否在启动时自动检查更新.(不建议关闭自动更新)
- 新特性:文件列表下方显示总文件数量.
- 新特性:保存/读取文件列表
- 新特性:自动关机前自动保存文件列表,下次开机后可手动加载文件列表查看文件处理情况
- 新特性:自动关机前弹窗提示
- 完善简体中文语言支持
- 根据当前语言来选择要打开的说明文档
- 在开始处理一个文件前,先检查其是否存在
- 修复bug:保存设置后,无法正常读取曾保存的[图像风格]设置
- 修复bug:移除单一条目后,其余所有条目的自定义分辨率设置会被清除
- 修复bug:当文件列表内有文件名称相同而扩展名不同的文件时, 如果启用多线程会造成读写冲突.
*/

/*
Integrated component(In releases):
- waifu2x-ncnn-vulkan version Nov 4, 2019
- Anime4K Java v0.9 Beta
- ffmpeg version 4.2.1
- gifsicle version 1.92
- Waifu2x-converter version: 2015-11-30T02:17:24
- ImageMagick 7.0.9-12-portable-Q16-x64
*/
