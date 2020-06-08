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
v2.44.07-beta:
- 缩短生成预处理图片时标记字符串的长度.
- 保存字体设定后,弹窗提示用户重启软件以生效.
- 性能优化.
- 补全翻译.
- 修复bug:无法查询[waifu2x-ncnn-vulkan,SRMD-ncnn-Vulkan,realsr-ncnn-vulkan]引擎的GPU ID.
- 修复bug:当启用预处理图片后,自定义分辨率设定无效.
- 修复bug:没有存储是否将自定义分辨率设定应用到全部的设置值.
v2.44.06-beta:
- 更新 ImageMagick.
- 添加tooltip.
- 将编码器列表移动到本地.
- 优化GUI.
- 修复bug:当处理视频和GIF时,当前文件进度的显示存在若干问题.
v2.44.05-beta:
- 当检测到Alpha通道时在文本框内输出提示.
- 补全翻译.
v2.44.04-beta:
- 更新 waifu2x-ncnn-vulkan.
- 更新 SRMD-ncnn-Vulkan.
- 更新 realsr-ncnn-vulkan.
v2.44.03-beta:
- 更新 waifu2x-ncnn-vulkan.
- 更新 SRMD-ncnn-Vulkan.
- 更新 realsr-ncnn-vulkan.
- 更改兼容性测试的输出文件格式以适配新版引擎.
v2.44.02-beta:
- 修改协助用户自动调整引擎设置时的策略.
- 性能优化.
- 修复bug:当使用waifu2x-caffe引擎处理视频和GIF时,自定义分辨率设定无法正常生效.
v2.44.01-beta:
- 新特性:为Anime4K引擎加入ACNet支持.
- 更新Anime4K.
- 支持将重试次数设定为0(即不重试).
- 修复bug:重试次数比显示的少一次.
- 修复bug:即使没有进行重试依旧在文本框内弹出重试提示.
- 修复bug:处理图片失败时,没有删除预处理图片时生成的缓存.
--------------------------------------------------------------------------
To do:
- 最小化到托盘
- 多gpu支持
- 升级更新时保存设置
- pdf格式支持
---------------------------------------------------------------------------
Integrated component(In releases):
- gifsicle version 1.92(release)
- SoX 14.4.2-win32(release)
- waifu2x-caffe 1.2.0.2(release)
- Waifu2x-converter e7de04d
- waifu2x-ncnn-vulkan 20200606(release)
- SRMD-ncnn-Vulkan 20200606(release)
- realsr-ncnn-vulkan 20200606(release)
- ImageMagick 7.0.10-17-Q16-x64(release)
- Anime4KCPP v2.0.0(release)
- ffmpeg version 4.2.3(release)
---
Icons made by : Freepik (https://www.flaticon.com/authors/freepik) From Flaticon : https://www.flaticon.com/
*/
