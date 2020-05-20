<p align="center">
<img src="/SRC/Waifu2x-Extension-QT/icon/icon_main.png" height="150">
</p>

<h2 align="center"> Waifu2x-Extension-GUI </h2>

<p align="center">
<img src="https://img.shields.io/github/v/release/aaronfeng753/waifu2x-extension-gui?label=Latest%20release&style=flat-square&color=brightgreen">
<img src="https://img.shields.io/badge/Support-Windows%20x64-blue?logo=Windows&style=flat-square">
<img src="https://img.shields.io/github/license/aaronfeng753/waifu2x-extension-gui?style=flat-square&label=License">
</p>

### [中文版说明文档](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/blob/master/README_CN.md#waifu2x-extension-gui)

# [Download Latest Build (Windows x64)](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/latest)
Download Latest installation (or portable) package for `Windows x64` PCs with `Intel/AMD/Nvidia` GPUs.

Download Latest Build: https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/latest

This package includes all the binaries and models required, so no CUDA or Caffe runtime environment is needed.

Supported Language: English, 简体中文, 日本語(機械翻訳), 繁體中文.

# What is Waifu2x-Extension-GUI?
`Image` & `GIF` & `Video` Super-Resolution using Deep Convolutional Neural Networks.

Based on `Waifu2x-ncnn-vulkan`,`SRMD-ncnn-Vulkan`,`Waifu2x-converter` and `Anime4K`. 

Thanks to waifu2x-ncnn-vulkan & SRMD-ncnn-Vulkan, Waifu2x-Extension-GUI could use any kind of gpu that supports `Vulkan`, including `Intel/AMD/Nvidia` GPUs. 

If your gpu doesn't support vulkan, you can use Waifu2x-converter, which is also intergrated into Waifu2x-Extension-GUI.

![mainwindow](/Screenshot/mainwindow.jpg)

#### Already been tested on the `AMD` RX 550, `NVIDIA` GeForce GTX 1070 and `Intel` UHD 620.

# Features
- Easy to use graphical user interface
- Supports batch processing of still pictures, gifs and videos
- Supports anime style and 3D realistic style images (that is, supports all image styles)
- Multiple integrated engines, strong hardware compatibility and support for NVIDIA, Intel and AMD GPUs
- Unlimited scale ratio
- Custom output resolution (support pictures & GIF & video)
- Audio denoising for video
- Flexible multi-thread management
- Processing video in segments, greatly reducing the hard disk space occupied when processing video.
- Support to save enlarged and noise-reduced pictures as .jpg and automatically perform lossless compression
- Automatically optimize the volume of the processed gif
- Compatibility test
- Multi-language (supports English, Simplified Chinese, Japanese)
- Support to store personalization settings and apply them automatically at startup
- Continually updated
- And more

# Samples

`All of these samples were processed using the Waifu2x-ncnn-vulkan engine.`

### **`Image`** : https://github.com/AaronFeng753/Waifu2x-Extension-GUI/tree/master/Samples/image

### **`Video`** : https://github.com/AaronFeng753/Waifu2x-Extension-GUI/tree/master/Samples/video

### **`GIF`** : https://github.com/AaronFeng753/Waifu2x-Extension-GUI/tree/master/Samples/gif

#### Original 2D Anime Image 480x300 (.jpg 93.2 KB):
![Original Imgae](/Samples/image/Original_[480x300].jpg)

#### After 8x magnification, level 3 denoise and compress 3840x2400 (.jpg 525 KB):
![Scaled Imgae](/Samples/image/Waifu2x_8x_[3840x2400].jpg)

#### Comparison
![Comparison](/Samples/image/Comparison.png)

#### Original 3D Real-life Image 800x535 (.jpg 81.4 KB):
![Original Imgae](/Samples/image/deer.jpg)

#### After 4x magnification, level 2 denoise and compress 3200x2140 (.jpg 881 KB):
![Scaled Imgae](/Samples/image/deer_Waifu2x.jpg)

#### Original 3D Game Imgae 1200x630 (.jpg 168 KB):
![Original Imgae](/Samples/image/Cyberpunk-2077.jpg)

#### After 4x magnification, level 2 denoise 4800x2520 (.jpg 3.99 MB):
![Scaled Imgae](/Samples/image/Cyberpunk-2077_waifu2x_4x_2n_jpg.jpg)

#### Original GIF 500 x 372 (493 KB):
![Original GIF](/Samples/gif/2_original.gif)

#### After 2x magnification, level 2 denoise and gif optimize 1000 x 744 (3.77 MB):
![Original GIF](/Samples/gif/2_waifu2x_compressed.gif)

### `Github doesn't online video playback, please check link below:`
### **`Video`** : https://github.com/AaronFeng753/Waifu2x-Extension-GUI/tree/master/Samples/video

# [License](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/blob/master/LICENSE)

Waifu2x-Extension-GUI is licensed under the [GNU Affero General Public License v3.0](https://www.gnu.org/licenses/agpl-3.0.en.html).

<p align="left">
<img src="/.github/AGPLv3_Logo.png" height="120">
</p>

# Privacy policy

We respect your privacy, so the following is the privacy policy of this software:
```
1. This software won't upload anything to the internet, so we won't collect any data from you, we even don't have a server.

2. This sofware will only connect to the internet when check for new update, which will download an .ini file from github (and gitee if you are using Simplified Chinese).

In conclusion, we won't collect any data from you. If you don't trust us, you can read the source code yourself or stop using this software.
```

# Credits:
- ❤All donors❤

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

- Icons made by : Freepik (https://www.flaticon.com/authors/freepik) From Flaticon : https://www.flaticon.com/

- Special thanks to @Nova-Aurora for fix several errors in the English README.

# ❤ Donate ❤ (推荐使用 支付宝 或者 微信)
# [PayPal](https://www.paypal.me/aaronfeng753)
### 除了扫码捐赠现金, 您也可以每天打开支付宝首页搜“682796694”领红包并使用, 我也可以获得收入.
![donate_new](/donate_new.jpg)
#### 如果donate(捐赠)二维码图片无法正常加载,请[打开此链接查看](https://gitee.com/aaronfeng0711/Waifu2x-Extension-GUI/raw/master/donate_new.jpg)或者合法的连接国际网络后刷新页面, 当然您也可以[按照这个教程来尝试修复github图片加载的问题](https://github.com/AaronFeng753/Github_dns_hosts#github_dns_hosts).
