# 更新日志
#### 历史版本下载:
#### 微云链接：https://share.weiyun.com/5abrs1W 密码：s4monw
---
## [v1.50.2](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v1.50.2)
#### 更新日志:
- 新特性:借助SoX(14.4.2-win32)对视频的音频进行降噪.(可选,附加设置,默认禁用)(感谢 @wenbob 的建议)
- 新特性:自动关闭计算机前将保存软件设置.
- 优化界面显示.
- 完善多语言支持,修正翻译错误.
- 修复bug:当启用[保存视频缓存]后,仍会删除源视频文件.
---
## [v1.45](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v1.45)
#### 更新日志:
- 新特性:可选择是否在退出时弹窗.(可选,附加设置,默认启用)
- 新特性:根据兼容性测试结果自动判断是否启用[自动检测Alpha通道].
- 新特性:保存视频缓存.(可选,附加设置,默认启用)
- 新特性:在用户停止任务或者发生错误时,视频的缓存不会被自动删除,并且还可以被软件读取以继续之前的进度.
- 提高多线程调度的稳定性.
- 修复bug:某些情况下无法正常重命名文件导致无法正常组装视频.
- 修复bug:因线程调度错误而导致无法正常处理列表内的文件.
---
## [v1.37](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v1.37)
#### 更新日志:
- 新特性:在将视频转换到mp4时,支持自动获取原视频的比特率并应用到转换后的mp4上.
- 新特性:在输出视频时,将使用软件内置算法分配比特率.
- 将默认的[压缩后的".jpg"图像质量]值改为95.
- 修复bug:某些情况下进度条无法正常显示进度.
---
## [v1.31](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v1.31)
#### 更新日志:
- 新特性:自动判断图片是否带有Alpha通道并切换到兼容的引擎.(可选,附加设置,默认禁用)(`启用此特性前请先确保您的电脑与waiuf2x-converter兼容.`)
- 新特性:第一次启动软件时自动运行兼容性测试.
- 改进多线程管理,提高稳定性.
- 去掉[显示Tips]按钮.
- 修复bug:某些情况下输出waifu2x-converter的处理器列表会造成软件陷入死循环.
---
## [v1.21](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v1.21)
#### 更新日志:
- 新特性:支持记住上一次浏览的文件夹.(remember the last browsed folder)
- 新特性:改进自动检查更新, 支持显示更新日志.
- 新特性:启动器加入多开检测.
- 新特性:视频自定义设置中支持键入额外指令.
- 更新 ImageMagick (7.0.9-27-Q16-x64)
- 更新 自动检查更新时使用的User-Agent.
- 更改部分界面布局.
- 改进稳定性.
- 添加了一些提示.
- 修改了一些令人迷惑的文本.
---
## [v1.11](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v1.11)
#### 更新日志:
- 新特性:新增SRMD-NCNN-Vulkan引擎. 🎉🥳🎉
SRMD-NCNN-Vulkan引擎可以同时支持`2D动漫`和`3D现实`两种图像样式.
您可以[点击本页面](https://github.com/cszn/SRMD)以了解SRMD技术.

---
## [v1.01](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v1.01)
#### 更新日志:
- ### 结束beta测试,开始正式版本的更新迭代. 🎉🥳🎉
- 新特性:新增[日本語]语言支持. 🐱‍👤
`(机器翻译,可能包含错误)`
- 增加一些提示.
- 修复bug:文本错误.
- 修复bug:切换语言后,部分文本显示错误的问题.
---
## [v0.57-beta](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v0.57-beta)
#### 更新日志:
- 添加PayPal捐赠按钮
- 重写文件列表相关的代码,加快运行速度,减少内存占用.
- 修复bug:当列表内条目较多时,移除单个条目会造成无响应.
- 修复bug:指定输出路径后, 同一文件名的文件将互相覆盖.
- 修复bug:当检测可用的GPU ID时未区分新旧版本的waifu2x-ncnn-vulkan
---
## [v0.56-beta](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v0.56-beta)
#### 更新日志:
- 修复bug:读取文件速度慢
- 修复bug:因为等待ffmpeg拆分视频(或GIF)而导致无法正常停止处理文件进程
- 修复bug:某些情况下,当退出程序后外部插件仍在运行
---
## [v0.55-beta](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v0.55-beta)
#### 更新日志:
- 默认启用旧版本的waifu2x-ncnn-vulkan引擎.(因为新版本的waifu2x-ncnn-vulkan存在稳定性问题)
`如果您需要使用waifu2x-ncnn-vulkan引擎的TTA的特性,切换到新版本即可.`
- 提高稳定性.
- 修复bug:一次性添加大量文件时会造成程序无响应.
---
## [v0.54-beta](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v0.54-beta)
#### 更新日志:
- 新特性:关闭软件时弹出窗口询问用户是否确认退出.
- 新特性:新增[强制重试], 当waifu2x线程卡死时可以执行强制重试以解除卡死,并且不会中断整个[放大与降噪]进程
- 完善对waifu2x-ncnn-vulkan引擎的错误检测, 提高稳定性.
- 修复bug:一些情况下未删除所有临时文件
- 修复bug:某些特殊情况下多线程调度出错导致卡死
---
## [v0.53-beta](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v0.53-beta)
#### 更新日志:
- 新特性:支持切换waifu2x-ncnn-vulkan引擎版本, 当您的电脑使用waifu2x-ncnn-vulkan引擎出现问题时, 可以尝试更新显卡驱动或启用旧版本的waifu2x-ncnn-vulkan引擎
- 新特性:兼容性测试新增对旧版本waifu2x-ncnn-vulkan引擎的测试
- 新特性:自定义重试次数(附加设置)
- 尝试修复高DPI屏幕显示的问题.(因为我自己没有高DPI显示器, 故无法测试)
- 修复bug:文本错误
---
## [v0.52-beta](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v0.52-beta)
#### 更新日志:
- 提高稳定性, 当引擎崩溃时将重试, 不会再因为一次引擎崩溃而整个处理过程失败
- 提高Anime4k引擎的稳定性
- 当未启用输出路径时, 禁用输出路径的lineEdit
- 修正文本错误
- 更新 waifu2x-ncnn-vulkan (version 20200224)
---
## [v0.51-beta](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v0.51-beta)
#### 更新日志:
- 新特性:当启用"删除原文件"时,可选择将原文件移动到回收站
- 新特性:支持选择是否启用自定义视频设置(附加设置,可选,默认禁用)
- 新特性:指定输出目录
- 新特性:扫描子文件夹(附加设置,可选,默认启用)
- 新特性:waifu2x-ncnn-vulkan 引擎新增TTA支持
- 新特性:waifu2x-converter 引擎新增TTA支持
- 新特性:文件列表互动性设置(附加设置)
- 更新waifu2x-ncnn-vulkan(20200223)
- 性能优化
- 完善错误检测
- 修复bug:文本错误
---
## [v0.461-beta](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v0.461-beta)
#### v0.461-beta 更新日志:
- 修复bug:某些情况下会造成源视频文件丢失
#### v0.46-beta 更新日志:
- 新特性:支持更多视频相关的自定义参数
- 新特性:初次启动时显示提示
- 更新 waifu2x-ncnn-vulkan (20200222)
- 界面布局调整
- 增强视频画质
- 修复bug:停止处理时没有删除软件生成的临时mp4视频
- 修复bug:启用waifu2x-ncnn-vulkan时,无法正确计算目标放大倍数 (感谢 slimekingwin8cn 反馈)
---
## [v0.45-beta](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v0.45-beta)
#### 更新日志:
- 新特性:waifu2x-ncnn-vulkan引擎支持切换到cunet模型
- 新特性:自定义视频的编码器,码率和像素格式(在附加设置中)
- 新特性:自定义分辨率支持应用设置到所有文件
- 视频画质增强
- 性能优化,减少启用自定义分辨率时处理GIF和视频所需的时间
- 修复bug:启用自定义分辨率时无法正确计算目标放大倍数
---
## [v0.411-beta](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v0.411-beta)
#### v0.411 更新日志:
- 改善waifu2x-converter的兼容性
#### v0.41 更新日志:
- 新特性: waifu2x-converter 支持更多降噪等级
- 新特性:新增 waifu2x-converter 引擎自定义设置
- 新特性:自定义字体
- 新特性:最小化窗体
- 新特性:隐藏文本框
- 更新 waifu2x-converter (v5.3.3) (感谢 NyanMisaka 反馈)
- 界面布局调整
- 提高稳定性
- 其他多项改进
- 修复bug:当文件扩展名大写时无法正确识别(感谢 zhehenkaifa 反馈)
- 修复bug:当视频扩展名为".MP4"而非".mp4"时, 会导致错误
- 修复bug:当文件名称内有多余的"."时会导致软件错误(感谢 我叫黄伟达 反馈)
- 修复bug:某些特殊情况下无法停止处理
---
## [v0.37-beta](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v0.37-beta)
#### 更新日志:
- 新特性:自定义".jpg"文件压缩后的图像质量
- 完善浏览文件对话框的文件扩展名过滤
- 更新ImageMagick(7.0.9-22-Q16-x64)
- 提高稳定性
- 完善简体中文支持
- 其他小改进
- 修复bug:程序关闭后滞留后台
---
## [v0.35-beta](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v0.35-beta)
#### 更新日志:
- 新特性:自定义全局字体大小(附加设置)
- 新特性:浏览与添加文件
- 性能优化
- 提高稳定性
- 审查代码,增加代码注释
- 界面布局调整
- 多处微小改进
- 修复bug:不能兼容Windows系统的自定义缩放
- 修复bug:某些特殊情况下无法正常识别文件是否被重复添加的问题
- 修复bug:无法保存降噪等级设置
---
## [v0.31-beta](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v0.31-beta)
#### 更新日志:
- 新特性:加载文件列表时显示进度
- 新特性:可自定义"自定义分辨率"的纵横比策略(在[附加设置]内)
- 新特性:手动保存设置后弹窗提示
- 新特性:可以一直隐藏"设置"(可选,默认禁用,在[附加设置]内)
- 新特性:显示GIF和视频线程数量之总数
- 界面布局微调
- 修复bug:当正在处理gif时,停止处理进程后无法正常改变文件处理状态
---
## [v0.291-beta](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v0.291-beta)
#### 更新日志:
- 新特性:关闭程序时自动保存设置(可选,默认启用)
- 新特性:可以一直隐藏"输入路径"(可选,默认禁用)
- 新特性:增加[关于]
- 新特性:权限检测
- 调整界面布局,将部分设置选项移动到[附加设置]内
- 完善简体中文支持
- 修复bug:一些特殊情况下程序会滞留在后台运行
---
## [v0.281-beta](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v0.281-beta)
#### 更新日志:
- 新特性:[文件列表]支持选中后再次点击取消选中
- 界面美化
- 调整界面布局
- 完善简体中文支持
- 修复bug:某些情况下GIF的帧无法正常排列
- 修复bug:某些情况下线程会被提前自动关闭导致任务无法完成
- 修复bug:某些情况下程序无法完全退出会滞留在后台执行无限循环
- 修复bug:当系统内没有Java运行环境时,调用Anime4k可能会造成软件进入无限循环
---
## [v0.25-beta](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v0.25-beta)
#### 更新日志:
- 新特性:首次启动时读取屏幕分辨率并设定为默认的自定义分辨率值
- 新特性:加载文件列表时,在Textbrowser内自动输出状态为[失败]的条目
- 更新FFmpeg(4.2.2)
- 修复bug:某些特殊的情况下,自动检查更新会误报存在新版本
- 修复bug:无法正常执行自动关闭计算机
- 修复bug:切换语言后无法正常显示[文件数量]
- 完善简体中文语言支持
---
## [v0.21-beta](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v0.21-beta)
#### 更新日志:
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
- 修复bug:自动检测到新版本后无法正常打开新版下载页面的问题.
---
## [v0.12-beta](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v0.12-beta)
#### 更新日志:
- 新增功能: 自动检查更新
- 修复bug: gif和视频无法正常获取放大倍数设定
- 修复bug: 当处理视频或gif时, 如果产生错误, 将导致线程管理问题.
---
## [v0.11-beta](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v0.11-beta)
#### 更新日志:
- 新增功能: 自定义图片, GIF和视频输出时的分辨率. (应一名匿名捐赠者的要求.🧐)
- 新增功能: 多语言.(加入简体中文) 🎉🥳🎉(应一名匿名捐赠者的要求.🧐)
- 新增功能: 支持保存当前设置, 启动软件时自动恢复上一次保存的设置.
- 新增功能: 切换设置选项显示状态.(隐藏/显示)
- 新增功能: 切换[输入路径]显示状态.(隐藏/显示)
- 新增功能: 用户误操作时弹出提示.
- 新增功能: 更改Textbrowser的字体大小后自动滑动到底端.
- 新增功能: Textbrowser输出新内容后自动滑动到最底端
- 新增功能: 文件列表表格自动根据显示内容调整大小
- 新增功能: 检测可用GPU与设定GPU ID
- 改进tableview选中逻辑. 当其中一个tableview被选中时, 其余两个自动取消选中.
- 优化tableview显示逻辑, 在输入文件后, 仅改变对应tableview的显示状态.
- 去除冗余代码
- 修复bug: 当[Re-process finished files]未启用时, 尝试移除已经处理完成的文件会导致程序崩溃.
- 修复bug: [video_gif_info.exe] 无法正常读取中文字符
- 修复bug: 处理完所有文件后未自动启用[兼容性检测]按钮
---
## [v0.01-beta](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v0.01-beta)
#### 更新日志:
- 第一个发行版.
