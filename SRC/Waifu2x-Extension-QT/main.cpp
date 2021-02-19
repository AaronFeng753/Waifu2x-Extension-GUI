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

Waifu2x-Extension-GUI V3 Update:
- New Feature: Video Frame Interpolation using Machine Learning. You can choose to interpolate the video frames after enlarge the video, or just interpolate the video frames.
- New built-in Frame Interpolation Engines: RIFE-NCNN-Vulkan, CAIN-NCNN-Vulkan.
- New GUI layout design.
- Fix multiple bugs.

Waifu2x-Extension-GUI V3 更新:
- 新特性: 借助机器学习对视频进行插帧(补帧). 您可以选择在对进行视频放大后对视频进行插帧, 或者仅对视频进行插帧.
- 新增内置插帧引擎: RIFE-NCNN-Vulkan, CAIN-NCNN-Vulkan.
- 新的用户界面布局设计.
- 多项错误修复.

v3.11.19:
- Optimize performance.
- Update GUI.
- Fix bug: Cannot interpolate frames of video with "%" in the absolute path.
- Fix bug: [Hide Text Browser] doesn't work properly.

v3.11.08-beta:
- Fix bug: Failure detection for NCNN-Vulkan engines not working normally.
- Fix bug: Extra commands are always loaded when [Process video by segment] is enabled.
- Fix bug: Compatibility test may provide incorrect results.
- Fix bug: Available GPU ID detection for NCNN-Vulkan engines may provide incorrect results.

v3.11.07-beta:
- Update GUI layout.

v3.11.06-beta:
- New Feature: Automatically enable [UHD Mode] when Ultra High Definition input is detected.
- Much more compact gui.
- Optimize performance.
- Fix bug: Extra commands for video are not loaded when [Process video by segment] is enabled.
- Fix bug: [Automatic file list scrolling] not working normally after user rearrange the files list.
- Fix bug: [Verify] button is enabled when [Multi GPU] is disabled.

v3.11.05-beta:
- Fix bug: Unable to use [Process video by segment] when Frame Interpolation is disabled.

v3.11.04-beta:
- Add a shortcut for enable Frame Interpolation in home tab.
- [Verify] button for Frame Interpolation.
- Complete translation.
- Other improvements.

v3.11.03-beta:
- New Feature: Now you can skip the super-resolution, only interpolate video frames by enable [Frame Interpolation only(Video)].
- Add pause support for frame interpolation.
- Add auto adjust for number of threads of frame interpolation.
- Add failure detection for frame interpolation.
- Remove temporary code for debug.
- Fix bug: If frame interpolation failed in the middle of processing video by segment, clip with wrong frame rate gonna be generated.
- Fix bug: Cannot read video configuration correctly.

v3.11.02-beta:
- New Feature: Video frame interpolation using cain-ncnn-vulkan(@nihui).
- Add tool tips.
- Fix bug: Video frame interpolation engines won't work when there are special characters in the installation path.
- Fix bug: Video frame interpolation setting is not saved in the video configuration file.

v3.11.01-beta:
- New Feature: Video frame interpolation using rife-ncnn-vulkan(@nihui).
- Update Waifu2x-NCNN-Vulkan,SRMD-NCNN-Vulkan,RealSR-NCNN-Vulkan.
- Fix bug: Failure detection for NCNN-Vulkan engines doesn't work as intended when processing GIF and Video.

--------------------------------------------------------------------------
To do:
- 添加一个快速剪切视频的小工具
- 在SRMD和REALSR正式加入cpu支持后, 在GPU ID tooltip上添加[★ "-1" is CPU,others are GPUs ★]
- 将文件列表的状态替换为预设的QString
---------------------------------------------------------------------------
Integrated component:
- gifsicle version 1.92
- SoX 14.4.2-win32
- waifu2x-caffe 1.2.0.4
- Waifu2x-converter 9e0284ae23d43c990efb6320253ff0f1e3776854
- Waifu2x-NCNN-Vulkan 20210210
- SRMD-NCNN-Vulkan 20210210
- RealSR-NCNN-Vulkan 20210210
- ImageMagick 7.0.10-61-portable-Q16-x64
- Anime4KCPP v2.5.0
- FFmpeg&FFprobe 2021-02-02-git-2367affc2c-full_build
- NirCmd v2.86
- Ghostscript 9.53.3
- GNU Wget 1.20.3
- rife-ncnn-vulkan 20210210
- cain-ncnn-vulkan 20210210
---------------------------------------------------------------------------
Icons made by :
Freepik (https://www.flaticon.com/authors/freepik) From Flaticon : https://www.flaticon.com/
Roundicons (https://www.flaticon.com/authors/roundicons) From Flaticon : https://www.flaticon.com/
Icongeek26 (https://www.flaticon.com/authors/Icongeek26) From Flaticon : https://www.flaticon.com/
*/
