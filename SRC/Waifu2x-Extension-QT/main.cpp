/*
    Copyright (C) 2021  Aaron Feng

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
Change log:

v2.71.22-beta:
- Complete translation.
- Limit the number of threads for scaling images to 32.

- 补全翻译.
- 限制缩放图片的线程数量到32.

v2.71.21-beta:
- The maximum number of threads will no longer be limited by the number of CPU logical cores.
- Speed up the assembly of GIF.
- Fix bug: Modifying the duration of the video clip after pausing and then continuing processing will cause the video sequence to be disordered.

- 最大线程数量将不再受到CPU逻辑核心数量的限制.
- 加快组装GIF的速度.
- 修复bug: 暂停后修改视频片段时长然后继续处理会造成视频顺序错乱.

--------------------------------------------------------------------------
To do:
- 补全翻译
- 在SRMD和REALSR正式加入cpu支持后, 在GPU ID tooltip上添加[★ "-1" is CPU,others are GPUs ★]
- 将文件列表的状态替换为预设的QString
---------------------------------------------------------------------------
Integrated component:
- gifsicle version 1.92
- SoX 14.4.2-win32
- waifu2x-caffe 1.2.0.4
- Waifu2x-converter 9e0284ae23d43c990efb6320253ff0f1e3776854
- waifu2x-ncnn-vulkan 20210102
- SRMD-ncnn-Vulkan 20200818
- realsr-ncnn-vulkan 20200818
- ImageMagick 7.0.10-61-portable-Q16-x64
- Anime4KCPP v2.5.0
- FFmpeg&FFprobe 2021-02-02-git-2367affc2c-full_build
- NirCmd v2.86
- Ghostscript 9.53.3
- GNU Wget 1.20.3
---------------------------------------------------------------------------
Icons made by :
Freepik (https://www.flaticon.com/authors/freepik) From Flaticon : https://www.flaticon.com/
Roundicons (https://www.flaticon.com/authors/roundicons) From Flaticon : https://www.flaticon.com/
Icongeek26 (https://www.flaticon.com/authors/Icongeek26) From Flaticon : https://www.flaticon.com/
*/
