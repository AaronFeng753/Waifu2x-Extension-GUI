<p align="center">
<img src="/SRC/Waifu2x-Extension-QT/icon/icon_main.png" height="150">
</p>

<h2 align="center"> Waifu2x-Extension-GUI </h2>

<p align="center">
<img src="https://img.shields.io/github/v/release/aaronfeng753/waifu2x-extension-gui?label=Latest%20release&style=flat-square&color=brightgreen">
<img src="https://img.shields.io/badge/Support-Windows%20x64-blue?logo=Windows&style=flat-square">
<img src="https://img.shields.io/github/license/aaronfeng753/waifu2x-extension-gui?style=flat-square&label=License">
</p>

#### 如果README内的图片无法正常加载,请[点击本链接查看码云内存储的README](https://gitee.com/aaronfeng0711/Waifu2x-Extension-GUI/blob/master/README_CN.md)或者[使用法定信道进行国际联网](http://www.cac.gov.cn/1996-02/02/c_126468621.htm)后刷新页面.

# [下载最新版本 (Windwos x64)](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/latest)
下载最新版本的完整版(或绿色版)安装包, 适用于拥有`Intel/AMD/Nvidia`显卡的`Windwos 64位`系统个人电脑.

下载最新版: https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/latest

安装包内已经包含软件运行所需的所有文件, 故`无需`额外安装配置CUDA或者Caffe运行环境.

支持的语言: English, 简体中文, 日本語(機械翻訳), 繁體中文.

[点击访问Gitee(码云,中国大陆)的releases下载页面](https://gitee.com/aaronfeng0711/Waifu2x-Extension-GUI/releases)

# Waifu2x-Extension-GUI是什么?
 借助深度卷积神经网络对`图片` & `GIF` & `视频`进行超分辨率放大(即放大与降噪).

基于 `Waifu2x-ncnn-vulkan`, `SRMD-ncnn-Vulkan`,`Waifu2x-converter`和`Anime4K`. 

借助waifu2x-ncnn-vulkan和SRMD-ncnn-Vulkan, Waifu2x-Extension-GUI 可以借助任何支持`Vulkan`的显卡进行运算,包括`Intel/AMD/Nvidia`. 

如果您的显卡不支持vulkan, 您还可以将引擎切换为Waifu2x-converter,同样可以很快的对图像进行放大与降噪.(已集成在release内)

![mainwindow](/Screenshot/mainwindow_cn.jpg)

#### 已经在 `AMD` RX 550, `NVIDIA` GeForce GTX 1070 和 `Intel` UHD 620 上通过测试.

# 主要特性
- 简单易用的图形用户界面
- 支持批量处理静态图片，gif和视频
- 支持动漫风格和3D写实风格的图像（即支持所有图像风格）
- 集成多个引擎，强大的硬件兼容性，支持借助NVIDIA、Intel、AMD进行运算
- 无限放大倍数
- 自定义输出分辨率（支持图片&GIF&视频）
- 音频降噪(处理视频时)
- 灵活的多线程管理
- 分段处理视频,大幅减少磁盘占用.
- 支持将放大&降噪后的图片另存为.jpg并自动进行无损压缩
- 支持对放大&降噪后的gif自动进行体积优化
- 兼容性测试
- 多语言（支持英语,简体中文,日本语）
- 支持存储个性化设置并在启动时自动应用
- 持续更新
- 以及更多

# 示例

`所有示例图像均使用waifu2x-ncnn-vulkan引擎处理`

### **`图片`** : https://github.com/AaronFeng753/Waifu2x-Extension-GUI/tree/master/Samples/image

### **`视频`** : https://github.com/AaronFeng753/Waifu2x-Extension-GUI/tree/master/Samples/video

### **`GIF`** : https://github.com/AaronFeng753/Waifu2x-Extension-GUI/tree/master/Samples/gif

#### 原图 2D动漫图片 480x300 (.jpg 93.2 KB):
![Original Imgae](/Samples/image/Original_[480x300].jpg)

#### 经过8x放大, 3级降噪和压缩后 3840x2400 (.jpg 525 KB):
![Scaled Imgae](/Samples/image/Waifu2x_8x_[3840x2400].jpg)

#### 对比
![Comparison](/Samples/image/Comparison.png)

#### 原图 3D写实图片 800x535 (.jpg 81.4 KB):
![Original Imgae](/Samples/image/deer.jpg)

#### 经过4x放大,2级降噪和压缩后 3200x2140 (.jpg 881 KB):
![Scaled Imgae](/Samples/image/deer_Waifu2x.jpg)

#### 原图 3D游戏图片 1200x630 (.jpg 168 KB):
![Original Imgae](/Samples/image/Cyberpunk-2077.jpg)

#### 经过 4x放大, 2级降噪 4800x2520 (.jpg 3.99 MB):
![Scaled Imgae](/Samples/image/Cyberpunk-2077_waifu2x_4x_2n_jpg.jpg)

#### 原图 GIF 500 x 372 (493 KB):
![Original GIF](/Samples/gif/2_original.gif)

#### 经过2x放大,2级降噪和gif优化后 1000 x 744 (3.77 MB):
![Original GIF](/Samples/gif/2_waifu2x_compressed.gif)

### `github不支持在线播放视频, 请点击下方链接查看视频示例:`
### **`视频`** : https://github.com/AaronFeng753/Waifu2x-Extension-GUI/tree/master/Samples/video

# Q&A
```
Q：在技术上和同样有gui的老牌软件waifu2x-caffe有什么区别？

A：waifu2x-extension-GUI集成了腾讯的一位大佬制作的waifu2x-ncnn-vulkan（原程序并没有自带图形界面，只有命令行），
vulkan是通用的全平台API，所以所有主流显卡（NVIDIA，Intel，AMD）均支持vulkan.
而caffe仅支持NVIDIA的CUDA，在没有n卡的平台只能用CPU进行运算，速度很慢很慢。
并且caffe不支持处理gif和视频，而extension通过集成外部开源组件实现了对gif和视频的自动拆分，放大，组装。
这也是我开发extension的初衷，让所有人（包括我自己），无论用什么显卡，都能快速的借助waifu2x技术处理图像。
```
```
Q：我的电脑连显卡都没有(或者显卡太老不支持vulkan)怎么办呢？

A：waifu2x-extension-GUI还集成了waifu2x-converter引擎, 可以借助cpu和老旧显卡进行快速进行运算.
速度参考:
配置:Windows10专业版1909,i7-8550U,Intel UHD 620
原图片:https://github.com/AaronFeng753/Waifu2x-Extension-GUI/blob/master/Samples/image/2_Original.jpg
软件设置:v0.12-beta,使用waifu2x-converter引擎,图片两倍放大,二级降噪
处理结果:耗时11秒,放大到1920x1080
```

# [许可协议](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/blob/master/LICENSE)

Waifu2x-Extension-GUI遵循[GNU Affero General Public License v3.0](https://www.gnu.org/licenses/agpl-3.0.en.html)许可协议.

<p align="left">
<img src="/.github/AGPLv3_Logo.png" height="120">
</p>

# 隐私政策

我们尊重您的隐私,所以本软件的隐私政策如下:
```
1. 本软件不会向互联网上传任何数据, 所以我们不会收集您的任何信息, 我们甚至没有服务器.

2. 本软件仅在检查更新时会连接互联网, 此时将从github(如果你在用简体中文,那么还包括码云)下载一个.ini文件.

总之, 我们不会收集您的任何信息. 如果您不信任我们, 您可以自行阅读本软件的源代码或者停止使用本软件.
```

# 鸣谢:
- ❤捐赠者们❤

- QT: https://www.qt.io/

- waifu2x-ncnn-vulkan: https://github.com/nihui/waifu2x-ncnn-vulkan

- SRMD-ncnn-Vulkan: https://github.com/nihui/srmd-ncnn-vulkan

- FFmpeg: https://ffmpeg.org/

- Gifsicle: https://www.lcdf.org/gifsicle/

- Anime4K: https://github.com/bloc97/Anime4K

- Waifu2x-converter: https://github.com/DeadSix27/waifu2x-converter-cpp

- ImageMagick: http://www.imagemagick.org/

- NSIS: https://nsis.sourceforge.io/

- Waifu2x-Extension: https://github.com/AaronFeng753/Waifu2x-Extension

- waifu2x: https://github.com/nagadomi/waifu2x

- SRMD: https://github.com/cszn/SRMD

- SoX: http://sox.sourceforge.net/

- Anime4KCPP: https://github.com/TianZerL/Anime4KCPP

- 图标作者: Freepik (https://www.flaticon.com/authors/freepik) 来自 Flaticon : https://www.flaticon.com/

# ❤ 捐赠 ❤ (推荐使用 支付宝 或者 微信)
# [PayPal](https://www.paypal.me/aaronfeng753)
### 除了扫码捐赠现金, 您也可以每天打开支付宝首页搜“682796694”领红包并使用, 我也可以获得收入.
![donate_new](/donate_new.jpg)
#### 如果donate(捐赠)二维码图片无法正常加载,请[打开此链接查看](https://gitee.com/aaronfeng0711/Waifu2x-Extension-GUI/raw/master/donate_new.jpg)或者合法的连接国际网络后刷新页面.
