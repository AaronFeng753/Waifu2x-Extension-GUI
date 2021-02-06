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

v2.71.21-beta:
- Fix bug: Modifying the duration of the video clip after pausing and then continuing processing will cause the video sequence to be disordered.

- 修复bug: 暂停后修改视频片段时长然后继续处理会造成视频顺序错乱.

v2.71.12:
- Significantly improve the efficiency, speed and stability of ALL engines when processing Video and GIF.
- The OLD version of waifu2x-ncnn-vulkan engine will no longer support the use of multiple graphics cards to process GIF and video.
- Fix bug: When custom resolution and video settings are enabled and the video stream bit rate is left 0, the video stream bit rate cannot be calculated normally.
- Fix bug: Scale ratio might be incorrect when using srmd-ncnn-vulkan.
- Fix bug: Unable to assemble gif because ImageMagick is not compatible with new AMD GPU driver.
- Update ImageMagick,FFmpeg&FFprobe.

- 重写大量代码,大幅提升所有引擎在处理视频和GIF时的效率,速度和稳定性.
- 旧版本的waifu2x-ncnn-vulkan引擎不再支持使用多显卡处理视频和GIF.
- 修复bug: 某些情况下无法正确计算视频的比特率.
- 修复bug: 使用srmd-ncnn-vulkan时放大倍率可能不准确.
- 修复bug: 因ImageMagick与新版AMD显卡驱动不兼容而无法组装GIF.
- 更新ImageMagick,FFmpeg&FFprobe.

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
