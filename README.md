# Waifu2x-Extension-GUI

![LICENSE](https://camo.githubusercontent.com/3867ce531c10be1c59fae9642d8feca417d39b58/68747470733a2f2f696d672e736869656c64732e696f2f6769746875622f6c6963656e73652f636f6f6b6965592f596561726e696e672e737667)
![Downloads](https://img.shields.io/github/downloads/AaronFeng753/Waifu2x-Extension-GUI/total)

### [中文版说明文档](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/blob/master/README_CN.md#waifu2x-extension-gui)
### Download: https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/latest
### Change log: https://github.com/AaronFeng753/Waifu2x-Extension-GUI/blob/master/Change_log.md
### Donation list: https://github.com/AaronFeng753/Waifu2x-Extension-GUI/blob/master/Donate_list.md

# What is Waifu2x-Extension-GUI?
`Image` & `GIF` & `Video` Super-Resolution using Deep Convolutional Neural Networks.

Based on `Waifu2x-ncnn-vulkan` and `Waifu2x-converter` . 

Thanks to waifu2x-ncnn-vulkan, Waifu2x-Extension-GUI could use any kind of gpu that support `Vulkan`, even Intel GPU. 

If your gpu doesn't support vulkan, you can use Waifu2x-converter, which is also intergrated into the Waifu2x-Extension-GUI.

![mainwindow](/Screenshot/mainwindow.jpg)

![processing](/Screenshot/processing.jpg)

![addsettings](/Screenshot/addsetiings.jpg)

#### Already been tested on `AMD` RX 550, `NVIDIA` GeForce GTX 1070 and `Intel` UHD 620.

# Features
- Easy to use graphical user interface
- Support batch processing of still pictures, gifs and videos
- Supports anime style and 3D realistic style images (that is, supports all image styles)
- Integrated multiple engines, strong hardware compatibility, support for computing with NVIDIA, Intel, AMD
- Unlimited scale ratio
- Custom output resolution (support pictures & GIF & video)
- Flexible multi-thread management
- Support to save enlarged and noise-reduced pictures as .jpg and automatically perform lossless compression
- Automatically optimize the volume of the processed gif
- Compatibility test
- Multi-language (supports simplified Chinese)
- Support to store personalization settings and apply them automatically at startup
- Continually updated
- And more

# Samples
### **`Image`** : https://github.com/AaronFeng753/Waifu2x-Extension-GUI/tree/master/Samples/image

### **`Video`** : https://github.com/AaronFeng753/Waifu2x-Extension-GUI/tree/master/Samples/video

### **`GIF`** : https://github.com/AaronFeng753/Waifu2x-Extension-GUI/tree/master/Samples/gif

#### Original 2D Anime Imgae 480x300 (.jpg 93.2 KB):
![Original Imgae](/Samples/image/Original_[480x300].jpg)

#### After 8x magnification, level 3 denoise and compress 3840x2400 (.jpg 525 KB):
![Scaled Imgae](/Samples/image/Waifu2x_8x_[3840x2400].jpg)

#### Comparison
![Comparison](/Samples/image/Comparison.png)

#### Original 3D Real-life Imgae 800x535 (.jpg 81.4 KB):
![Original Imgae](/Samples/image/deer.jpg)

#### After 4x magnification, level 2 denoise and compress 3200x2140 (.jpg 881 KB):
![Scaled Imgae](/Samples/image/deer_Waifu2x.jpg)

#### Original GIF 500 x 372 (493 KB):
![Original GIF](/Samples/gif/2_original.gif)

#### After 2x magnification, level 2 denoise and gif optimize 1000 x 744 (3.77 MB):
![Original GIF](/Samples/gif/2_waifu2x_compressed.gif)

### `Github doesn't support play video online, pls check link below:`
### **`Video`** : https://github.com/AaronFeng753/Waifu2x-Extension-GUI/tree/master/Samples/video

# How to fix compatibility issue :
#### waifu2x-ncnn-vulkan: Re-install gpu driver or update it to the latest.
#### waifu2x-converter: Buy a new computer.
#### Anime4k: Install the latest JDK and JRE

# Integrated component(In releases):
- waifu2x-ncnn-vulkan version Nov 4, 2019

- Anime4K Java v0.9 Beta

- ffmpeg version 4.2.2

- gifsicle version 1.92

- Waifu2x-converter version: 2015-11-30T02:17:24

- ImageMagick 7.0.9-12-portable-Q16-x64

# Credits:
- Donors: https://github.com/AaronFeng753/Waifu2x-Extension-GUI/blob/master/Donate_list.md

- waifu2x-ncnn-vulkan: https://github.com/nihui/waifu2x-ncnn-vulkan

- FFmpeg: https://ffmpeg.org/

- Gifsicle: https://www.lcdf.org/gifsicle/

- Anime4K: https://github.com/bloc97/Anime4K

- Waifu2x-converter: https://github.com/WL-Amigo/waifu2x-converter-cpp

- ImageMagick: http://www.imagemagick.org/

- NSIS: https://nsis.sourceforge.io/

- Icons made by : Freepik (https://www.flaticon.com/authors/freepik) From Flaticon : https://www.flaticon.com/

# `Donate`
### [Donation list](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/blob/master/Donate_list.md)
### 除了扫码捐赠现金, 您也可以 打开支付宝首页搜“682796694”领红包, 我也可以获得收入.
![donate_new](/donate_new.jpg)
#### 如果donate(捐赠)二维码图片无法正常加载,请[打开此链接查看](https://gitee.com/aaronfeng0711/Waifu2x-Extension-GUI/raw/master/donate.jpg)或者合法的连接国际网络后刷新页面, 当然您也可以[按照这个教程来尝试修复github图片加载的问题](https://github.com/AaronFeng753/Github_dns_hosts#github_dns_hosts).
