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
- 更新QT版本至5.15.0,并使用新版QT重新编译.
- 更新waifu2x-ncnn-vulkan,srmd-ncnn-vulkan,realsr-ncnn-vulkan.
- 性能优化.
- 修复bug.
----------------------------
Detailed change log:
v2.54.03-beta:
- 在启用[禁止连接码云]后,自动在后台检测是否可以连接raw.githubusercontent.com.
- 更新FFmpeg&FFprobe.
v2.54.02-beta:
- 更新waifu2x-ncnn-vulkan,srmd-ncnn-vulkan,realsr-ncnn-vulkan.
- 添加tooltip.
v2.54.01-beta:
- 更新QT版本至5.15.0,并使用新版QT重新编译.
- 内置QT-5.15.0的许可协议文件.
- 删除无用变量.
- 使用QRandomGenerator代替qsrand和qrand.
- 改进检查更新的流程.
- 完善翻译文档.
- 修复bug:在任务栏图标的右键菜单中,点击退出,并点击弹窗中的[否]选项将导致弹窗重复弹出多次.
--------------------------------------------------------------------------
To do:
- pdf格式支持
- 移植到Linux
---------------------------------------------------------------------------
Integrated component:
- gifsicle version 1.92(release)
- SoX 14.4.2-win32(release)
- waifu2x-caffe 1.2.0.3
- Waifu2x-converter e7de04d
- waifu2x-ncnn-vulkan 54b2b3df1384a7c13bfec431b806cf152f227e91
- SRMD-ncnn-Vulkan 04f93f628b12db5f24d5e46b6433fac728093305
- realsr-ncnn-vulkan fcaae13be5a9edf1f7e49035074c89eb75ec242b
- ImageMagick 7.0.10-23 Q16 x64 2020-07-04
- Anime4KCPP v2.2.1
- FFmpeg&FFprobe 20200713-7772666-win64-static
---
Icons made by : Freepik (https://www.flaticon.com/authors/freepik) From Flaticon : https://www.flaticon.com/
*/
