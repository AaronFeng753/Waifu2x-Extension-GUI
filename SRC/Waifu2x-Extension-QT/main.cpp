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
- NEW: Customize the resolution of image, GIF and video output.
- NEW: Multi-language.(Add Simplified Chinese.)
- New: Switch the display status of setting options. (Hide / Show)
- New: Prompt when misoperation.
- New: Automatically scroll to the bottom after changing the font size of Textbrowser.
- Improve tableview selection logic. When one tableview is selected, the other two are automatically unselected.
- Remove redundant code
- Fix bug: [Remove item] will cause the program to crash when the file is finished and [Re-process finished files] is not enabled.
- Optimize the tableview display logic. After inputting the file, only change the display state of the corresponding tableview.
- Fix bug: [video_gif_info.exe] Chinese characters cannot be read normally

- 新增: 自定义图片, GIF和视频输出时的分辨率.
- 新增: 多语言.(加入简体中文)
- 新增: 切换设置选项显示状态.(隐藏/显示)
- 新增: 用户误操作时弹出提示.
- 新增: 更改Textbrowser的字体大小后自动滑动到底端.
- 改进tableview选中逻辑. 当其中一个tableview被选中时, 其余两个自动取消选中.
- 去除冗余代码
- 修复bug: 当[Re-process finished files]未启用时, 尝试移除已经处理完成的文件会导致程序崩溃.
- 优化tableview显示逻辑, 在输入文件后, 仅改变对应tableview的显示状态.
- 修复bug: [video_gif_info.exe] 无法正常读取中文字符
*/
