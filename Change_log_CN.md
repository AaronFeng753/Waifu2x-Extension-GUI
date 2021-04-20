# 更新日志
---
#### [📝English Change log](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/blob/master/Change_log.md)

#### [⭐最新版](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/latest)
---
#### 🗂全部历史版本安装包合集:
- ☁百度云: https://pan.baidu.com/s/14grAcWeHs6v71tLnE1-81w 提取码: swe7
---
## [v3.31.13](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v3.31.13)
#### 更新日志:
- 新增超分辨率引擎: SRMD-CUDA (引擎作者: @MrZihan).
- 一些其他改进.
- 修复问题: 生成GIF的帧率一直是10 FPS.
---
## [v3.22.26](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v3.22.26)
#### 更新日志:
- 新特性: 现在您可以启用 [结束后自动打开] 以在处理完所有文件后自动打开输出文件夹.
- 新选项: [忽略帧率模式].
- 支持者名单页面(patreon).
- 改进生成视频的画质, 以及生成文件的命名.
- 为任务栏菜单添加更多选项.
- 修复多项问题, 优化性能以及其他诸多改进.
---
## [v3.22.14](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v3.22.14)
#### 更新日志:
- 新特性: 现已支持带有小数位的放大倍率.
- 改进GUI, 性能优化.
- 添加了一个Patreon按钮.
- 更新FFmpeg&FFprobe.
- 修复多项问题.
---
## [v3.21.13](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v3.21.13)
#### 更新日志:
- ✨新的插帧引擎: DAIN-NCNN-Vulkan.
- ✨新特性: 现在您可以调节[帧率倍率].
- 为任务栏菜单添加更多选项和图标.
- 新特性: 处理完所有文件后展示总结弹窗.
- 新特性: 现在您可以保存多份文件列表.
- 更新 FFmpeg&FFprobe,ImageMagick.
- ✅修复多项错误, 并优化性能.
---
## [v3.11.27](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v3.11.27)
#### 更新日志:
- 为Rife-ncnn-vulkan引擎新添加模型: [rife-v2.4]
- 新添加关闭屏幕按钮.
- 改进 [暂停] 功能.
- 更新rife-ncnn-vulkan, ImageMagick, FFmpeg&FFprobe.
- 优化性能
- ✅多项错误修复.
---
## [v3.11.19](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v3.11.19)
#### 更新日志:
- 新特性: 借助机器学习对视频进行插帧(补帧). 您可以选择在对进行视频放大后对视频进行插帧, 或者仅对视频进行插帧.
- 新增内置插帧引擎: RIFE-NCNN-Vulkan, CAIN-NCNN-Vulkan.
- 新的用户界面布局设计.
- 多项错误修复.
---
## [v2.71.22](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v2.71.22)
#### 更新日志:
- 最大线程数量将不再受到CPU逻辑核心数量的限制.
- 加快组装GIF的速度.
- 停止处理文件后，杀死滞留在后台的子进程。
- 修复bug: 当使用Anime4k时,GIF放大倍率错误.
- 修复bug: 即便在处理文件时,从列表中移除文件的热键依旧可用.
- 修复bug: 即使[剩余时间]和[预计完成时间]没有被显示,但是仍旧被计算.
- 修复bug: 暂停后修改视频片段时长然后继续处理会造成视频顺序错乱.
---
## [v2.71.12](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v2.71.12)
#### 更新日志:
- 重写大量代码,大幅提升所有引擎在处理视频和GIF时的效率,速度和稳定性.
- 旧版本的waifu2x-ncnn-vulkan引擎不再支持使用多显卡处理视频和GIF.
- 修复bug: 某些情况下无法正确计算视频的比特率.
- 修复bug: 使用srmd-ncnn-vulkan时放大倍率可能不准确.
- 修复bug: 因ImageMagick与新版AMD显卡驱动不兼容而无法组装GIF.
- 更新ImageMagick,FFmpeg&FFprobe.
---
## [v2.62.13](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v2.62.13)
#### 更新日志:
- 改进SRMD-NCNN-Vulkan的性能.
- 修复错误: 使用SRMD-NCNN-Vulkan时的放大倍率错误.
---
## [v2.62.12](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v2.62.12)
#### 更新日志:
- 加快视频和gif的处理速度。
- 替换掉python扩展以提高性能。
- 改进下载速度, 视频处理流程, 多线程性能以及其他性能改进。
- 更详尽的工具提示，修正错字。
- 修复错误：无法正确处理可变帧率视频。
- 修复错误：兼容性测试结束后，waifu2x-caffe进程可能滞留在后台。
---
## [v2.61.13](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v2.61.13)
#### 更新日志:
- 新特性: 当文件分辨率大于指定分辨率时,自动跳过文件.
- 新特性: Anime4k新增CUDA运算支持.
- 预载入引擎设定以改进性能,以及其他性能改进.
- 更新并适配新版Anime4K.
- 改进读取图片分辨率的方式.
- 根据 [@Nova-Aurora](https://github.com/Nova-Aurora) 的建议改进GUI.
- 更新FFmpeg&FFprobe,ImageMagick.
- 修复英语翻译问题(感谢 [@Nova-Aurora](https://github.com/Nova-Aurora)), 以及GUI问题修复.
- 修复bug: 在用户指定处理器后, Waifu2x-Converter的[禁用GPU]选项将不会生效.
- 修复bug: 如果在处理图片时暂停,会产生垃圾文件.
---
## [v2.59.15](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v2.59.15)
```
v2.59.15:
- 改进 GUI.

v2.59.04-beta:
- 仅在指定的图片格式不支持Alpha通道时,才将图像强制保存为PNG。
- 添加按钮以将块大小调整为2的n次方。
- 性能优化。
- 编辑工具提示。
- 其他改进。
- 更新ImageMagick，FFmpeg＆FFprobe。

v2.59.03-beta：
- 当文件列表为空时，自动禁用[保存文件列表]按钮。
- 更紧凑的GUI和其他GUI改进。
- 编辑工具提示和中文翻译。

v2.59.02-beta：
- 新功能：重置文件列表比例。
- 新功能：针对每种文件的文件计数。
- 新图标。
- 性能优化。
- 对GUI的其他改进。
- 完整的翻译。
- 修复bug：“加载文件”列表时未禁用[添加文件]按钮。
- 修复bug：使用[移除项目]的右键菜单后，文件列表的横向标题数据将丢失。

v2.59.01-beta：
- 新功能：现在您可以通过拖动来更轻松地隐藏组件。
- 改善GUI设计。

v2.58.14-beta：
- 新功能：现在，您可以调整文件列表的水平宽度。
- 将[移至回收站]选项移至[删除原始文件]的右键菜单
- 对GUI的其他改进。
- 当用户尝试启用多GPU并且GPU ID列表为空时，通知用户。
- 优化性能。
```
---
## [v2.58.13](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v2.58.13)
#### 更新日志:
- 新特性: Waifu2x-ncnn-vulkan 引擎支持使用CPU运算.
- 新特性: 文件列表支持键盘快捷键.
- 当文件列表内的文件已经全部处理完成或者被删除时, 提示用户.
- 更新 Waifu2x-NCNN-Vulkan,ImageMagick,FFmpeg&FFprobe.
- 与未来版本的NCNN-Vulkan引擎兼容.
- 更加详尽的工具提示.
---
## [v2.57.41](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v2.57.41)
#### 更新日志:
- ♥预祝各位新年快乐, 感谢各位在2020年中对本项目的支持.
- 使用强制重试时将不会再改变设定值.
- 如果优化后的GIF体积不小于原GIF,则其不会被用于替换原GIF.
- 更新Waifu2x-converter,Anime4K,ImageMagick,FFmpeg&FFprobe.
- 优化性能,修正错字,以及其他改进.
- 修复bug: 如果用户将"gif"放入文件扩展名列表中,会导致gif文件错位.
- 修复bug: 有时文件列表会随机横向滑动.
---
## [v2.57.31](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v2.57.31)
#### 更新日志:
- 更新Anime4K,FFmpeg&FFprobe,ImageMagick.
- 改进性能与兼容性.
- 一些其他小改进
- ✅修正多个(英语)语法,UI以及兼容性问题.
- ✅修复多个bug.
---
## [v2.57.21](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v2.57.21)
#### 更新日志:
- 新特性: 智能检测透明通道是否丢失, 并重新处理文件.
- 新特性: [`总是预处理带有Alpha通道的图片`].
- 更新 ImageMagick.
- 改进性能与稳定性.
- 修复bug: NCNN-Vulkan 系列引擎无法保留某些特殊png图片的透明通道.
- 修正错字.
---
## [v2.57.12](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v2.57.12)
#### 更新日志:
- 优化文件列表, 大大提高了文件读取速度.
- 其他性能优化.
- 更新 FFmpeg＆FFprobe, ImageMagick.
- 修正了工具提示中的英文拼写错误和语法问题. (感谢 @CaptainStabs 反馈)
---
## [v2.56.95](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v2.56.95)
#### 更新日志:
- 新特性: 现在您可以将图片另存为多种格式.(感谢 [@loafylemon](https://github.com/loafylemon) 的建议)
- 优化性能.
- 更新 FFmpeg&FFprobe.
- 移除多余的功能,代码,以及注释.
- 修复bug: 因帧率不正确导致的视频音画不同步.(感谢 [@CAFUCSteven](https://github.com/CAFUCSteven) 的反馈以及协助测试.)
- 以及其他改进.
---
## [v2.56.82](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v2.56.82)
#### 更新日志:
- 新特性: 现在您可以对文件列表内的文件进行排序.
- 对文件列表的其他改进.
- 添加更多好看的图标.
- 更新FFmpeg&FFprobe,ImageMagick.
- 改进性能.
- 减少缓存文件.
- 添加更多提示信息.
- 修复多个bug.
---
## [v2.56.71](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v2.56.71)
#### 更新日志:
- 新特性:文件列表右键菜单支持应用与取消自定义分辨率设定.
- 新特性:将文件输出到指定路径时可以选择保留上级文件夹.
- 性能优化.
- 改进用户界面.
- 更新与适配Anime4KCPP,FFmpeg&FFprobe,ImageMagick.
- 修复多个bug.
---
## [v2.56.52](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v2.56.52)
#### 更新日志:
- 新特性:可启用[替换原文件]以直接用处理后的文件替换原文件.(附加设置,可选,默认禁用)
- 性能优化.
- 更新FFmpeg&FFprobe,ImageMagick.
- 修复bug.
---
## [v2.56.46](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v2.56.46)
#### 更新日志:
- 在切换更新通道后自动检查更新.
- 为[移除条目]按钮添加右键菜单.
- 改善文件列表的易用性.
- 性能优化.
- 更新waifu2x-caffe,ImageMagick.
- 增强waifu2x-caffe引擎,Anime4k引擎的兼容性.
- 自动调整引擎设置后跳转到主页.
- 修复bug:鼠标右键无法选中文件列表内的文件.
---
## [v2.56.41](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v2.56.41)
#### 更新日志:
- 新特性:为兼容性测试增加进度条.
- 更新FFmpeg&FFprobe.
- 更正错误的描述.
- 补全多语言翻译.
- 修复bug.
---
## [v2.56.35](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v2.56.35)
#### 更新日志:
- 根据视频时长,自动判断是否需要启用分段处理.
- 更新NCNN-Vulkan系列引擎.
- 更新FFmpeg&FFprobe.
- 修复多个bug.
---
## [v2.56.32](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v2.56.32)
#### 更新日志:
- 添加更多处理完文件后可执行的动作(关机,睡眠,休眠,重启).(感谢 @Michael18751 的建议)
- 适配新版FFmpeg.
- 更新FFmpeg&FFprobe,ImageMagick.
- 性能优化.
- 修复多个bug.
---
## [v2.56.12](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v2.56.12)
#### 更新日志:
- Anime4k引擎在启用HDN后可支持调节降噪等级.
- 为Anime4k引擎添加自动检测Alpha通道的支持.(新版Anime4k引擎支持保留Alpha通道)
- 为[文件列表]添加右键菜单选项:[打开选中文件],[从列表移除所选文件]和[打开选中文件的文件夹]
- 为[输出路径]添加右键菜单以打开输出路径文件夹.
- 更新FFmpeg&FFprobe,Anime4K.
- 修复多个bug.
- 其他改进.
---
## [v2.55.24](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v2.55.24)
#### 更新日志:
- 更新FFmpeg&FFprobe,Python扩展,waifu2x-ncnn-vulkan,ImageMagick.
- 性能优化.
- 其他改进.
---
## [v2.55.12](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v2.55.12)
#### 更新日志:
- 新特性:为所有引擎加入显示当前多显卡设定的按钮.
- 更新FFmpeg&FFprobe,ImageMagick.
- 性能优化.
- 完善多语言支持.
- 修复多个bug.
---
## [v2.54.13](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v2.54.13)
#### 更新日志:
- 更新QT版本至5.15.0,并使用新版QT重新编译.
- 当检测到自定义分辨率包含奇数时,自动为视频修正自定义分辨率.
- 在启用[禁止连接码云]后,自动在后台检测是否可以连接raw.githubusercontent.com.
- 更新waifu2x-ncnn-vulkan,srmd-ncnn-vulkan,realsr-ncnn-vulkan,FFmpeg&FFprobe.
- 性能优化.
- 修复多个bug.
- [以及其他诸多改进, 点击查看详细更新日志](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/blob/master/.github/temp/v2.54.13_v2.53.14_Detailed_change_log.txt)
---
## [v2.53.14](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v2.53.14)
#### 更新日志:
- 新特性:[添加文件]按钮支持选中并添加多个文件.
- 新特性:加入[禁止连接码云]选项,启用本选项将禁止本软件从码云下载文件.
- 更新RealSR-ncnn-vulkan,Waifu2x-ncnn-vulkan,ImageMagick,Python扩展.
- 性能优化,减少网络流量消耗和对外部程序的调用.
- 修复多个bug.
- [以及其他诸多改进, 点击查看详细更新日志](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/blob/master/.github/temp/v2.53.14_v2.52.16_Detailed_change_log.txt)
---
## [v2.52.16](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v2.52.16)
#### 更新日志:
- 新特性:为Anime4k & Waifu2x-caffe引擎添加验证显卡配置的按钮.
- 优化SRMD-ncnn-vulkan处理图像的流程,提高性能.
- 更新ImageMagick,FFmpeg&FFprobe,waifu2x-caffe,Anime4k.
- 内置cuDNN.
- 性能优化.
- 修复多处bug.
- [以及其他诸多改进, 点击查看详细更新日志](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/blob/master/.github/temp/v2.52.16_v2.51.17_Detailed_change_log.txt)
---
## [v2.51.17](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v2.51.17)
#### 更新日志:
- 新特性:为所有引擎添加多显卡支持.
- 新特性:添加任务栏图标,支持最小化到任务栏.
- 新特性:后台模式.
- 新特性:可选择更新通道(稳定版,测试版).
- 新特性:更新后将保留原来的设置.
- 新特性:为Anime4k引擎的ACNet算法添加快速模式支持.
- 新特性:ACNet算法支持指定显卡.
- 新特性:为输出到指定文件夹添加[直接覆盖]选项.
- 更新ImageMagick,waifu2x-ncnn-vulkan,FFmpeg.
- 性能优化.
- 修复多处bug.
- [以及其他诸多改进, 点击查看详细更新日志](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/blob/master/.github/temp/v2.51.17_v2.45.18_Detailed_change_log.txt)
---
## [v2.45.18](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v2.45.18)
#### 更新日志:
- 新特性:为Anime4k引擎加入HDN模式支持,可改善画质.(启用ACNet后才能启用HDN)
- 新特性:保留原文件名.(可选,默认禁用)
- 更新Anime4K,ImageMagick,FFmpeg.
- 为提示音添加淡出效果.(由 @Michael18751 编辑)
- 性能优化.
- 修复多处Bug.
- 以及其他多项改进.
---
## [v2.44.18](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v2.44.18)
#### 更新日志:
- 新特性:为Anime4K引擎加入ACNet算法支持, ACNet可以在改善画质的同时实现高速处理.(您需要在Anime4k引擎设置中手动启用ACNet)
- 更新 ImageMagick,waifu2x-ncnn-vulkan,SRMD-ncnn-Vulkan,realsr-ncnn-vulkan,Anime4K.
- 优化性能,提高稳定性.
- 修复多处Bug.
- 以及其他多项改进.
---
## [v2.43.17](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v2.43.17)
#### 更新日志:
- 新特性:支持预处理图片以兼容更多图片格式.(附加设置,可选,默认禁用)
- 更新Python extension,waifu2x-ncnn-vulkan,srmd-ncnn-vulkan,realsr-ncnn-vulkan,FFmpeg,ImageMagick.
- 改进自动检查更新流程.
- 改进多线程调度机制.
- 添加与修改tooltip.
- 支持在线更新捐赠二维码.
- 添加一些提示,修改一些文本,修正错字.
- 性能优化.
- 修复若干bug.
- 其他改进.
---
## [v2.41.12](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v2.41.12)
#### 更新日志:
- #### 新特性:新增realsr-ncnn-vulkan引擎,大幅提升对3D写实风格图像的处理效果(感谢 @CAFUCSteven 协助测试软件).
- 新特性:除Anime4k外的引擎均支持自动识别并处理Alpha通道图片.
- 更新waifu2x-ncnn-vulkan引擎.
- 更新srmd-ncnn-vulkan引擎.
- 更新ImageMagick.
- 优化启动器.
- 优化检查更新的流程.
- 调整对带有Alpha通道的图片的处理流程.
- 修复若干bug.
- 其他改进.
---
## [v2.31.14](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v2.31.14)
#### 更新日志:
- 新特性:新增waifu2x-caffe引擎(实验性功能)(感谢 @CAFUCSteven 协助测试软件).
- 新特性:waifu2x-caffe引擎支持快速切换图像样式.
- 新特性:支持切换处理带有Alpha通道的图片时使用的引擎.
- 优化处理带有Alpha通道的图片的流程.
- 调整兼容性测试界面.
- 性能优化.
- 更改默认设置.
- 添加一些提示,修改一些文本,修正错字.
- 修复多个bug.
- 其他改进.
---
## [v2.21.12](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v2.21.12)
#### 更新日志:
- 新特性:处理视频或GIF时,单独显示当前文件的进度(进度条,消耗时间,剩余时间,ETA).
- 性能优化.
- 改善ffporbe插件的兼容性.
- 修复bug:一些情况下,处理视频时会卡在获取时长那一步.
- 其他改进.
---
## [v2.11.3](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v2.11.3)
#### 更新日志:
- 新特性:Anime4K引擎支持处理图片和GIF.
- 新特性:根据兼容性测试结果自动调整引擎设置.
- 兼容性测试结束后,根据测试结果给予用户提示.
- 更新Anime4K(v1.8.1).
- 更新ImageMagick(7.0.10-12-Q16-x64).
- 更改默认设置.
- 添加一些提示.
- 处理文件前检测是否拥有文件所在目录的写权限.
- 改进软件启动时的权限检测.
- 修复bug:特殊情况下,处理视频时可能会卡在获取时长那一步.
- 修复bug:无法正常检测是否拥有输出路径的写权限.
- 修复bug:当使用Anime4k引擎处理视频时,自定义分辨率设定无效.
---
## [v2.02.3](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v2.02.3)
#### 更新日志:
- 更新Anime4K(v1.7.0).
- 改进简体中文支持.
- 改进兼容性测试,减少资源占用.
- 添加一些提示.
- 修复bug:某些情况下,程序会在运行兼容性测试时卡住.
- 修复bug:某些组件未在处理文件时禁用,可能导致潜在错误.
---
## [v2.01.1](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v2.01.1)
#### 更新日志:
- 新特性:重新设计用户界面.
- 新特性:添加[繁體中文]支持(感謝 @uimee 協助翻譯).
- 新特性:Anime4K引擎支持指定GPU.
- 新特性:存储与读取所有引擎的GPU设置.
- 新特性:改进兼容性测试,增加检测项目,支持存储测试结果.
- 更新Anime4K(v1.6.0).
- 更新ImageMagick(7.0.10-10-Q16-x64).
- 改进多线程管理.
- 添加一些提示.
- 性能优化.
- 修复bug.
---
## [v1.73.3](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v1.73.3)
#### 更新日志:
- 新特性:为Anime4K引擎加入GPU加速支持.
- 新特性:安装软件后,首次启动时弹窗让用户选择自己所使用的语言.
- 更新Anime4K(v1.5.0)
- 不再允许禁用自动检查更新，但是可以禁用自动更新弹窗，禁用弹窗后将在文本框内显示更新提示.
- 兼容测试结束后弹窗提示用户查看测试结果.
- 完善兼容性测试.
- 当文件列表内包含GIF或视频时隐藏ETA和剩余时间.
- 完善多语言支持.
- 修复bug.
---
## [v1.71.1](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v1.71.1)
#### 更新日志:
- 新特性:用Anime4KCPP替换掉了之前的Anime4K-JAVA,Anime4KCPP与Anime4K-JAVA相比速度更快,画质更好.
- 新特性:为Anime4K引擎添加了大量自定义设置.
- 界面布局调整.
- 修正翻译错误.
- 修改部分文本.
---
## [v1.63.1](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v1.63.1)
#### 更新日志:
- 提高稳定性.
- 优化性能.
- 添加一些提示.
- 修改部分文字.
- 修正翻译错误.
- 修复bug:极少数情况下缓存文件会重名并导致错误.
- 修复bug:重置视频设置按钮无法正常工作.
---
## [v1.62.5](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v1.62.5)
#### 更新日志:
- 修复bug:当`分段处理视频`启用时,无法正常读取缓存,这将导致软件无法继续之前的进度.(感谢 Esper Wang 的反馈✔)
---
## [v1.62.4](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v1.62.4)
#### 更新日志:
- 修复bug: 一些情况下,输出的视频会音画不同步.(感谢  @FITHRPG6 的反馈)
---
## [v1.62.3](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v1.62.3)
#### 更新日志:
- 新特性:waifu2x-ncnn-vulkan新增了fp16p支持,改善了对一些老驱动的兼容性.
- 更新waifu2x-ncnn-vulkan(version 20200414).
- 更新ImageMagick(7.0.10-6-Q16-x64).
- 将默认的自定义分辨率的纵横比策略更改为保持纵横比.
- 修复bug:某些情况下的界面显示错误.
- 修复bug:当源视频文件名称内包含中文时,分段处理视频后无法正确的组装视频.
---
## [v1.61.3](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v1.61.3)
#### 更新日志:
- 新特性:分段处理视频,大幅减少处理视频时占用的硬盘空间. 🎉🥳🎉 (可选,附加设置,默认启用)
- 完善多语言支持.
- 其他小改进.
- 修复bug:停止处理进程时将强制终止ffmpeg进程,会导致无法正常生成缓存文件,影响下次读取任务进度.
- 修复bug:某些情况下进度条无法正常显示进度.
---
## [v1.51.3](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v1.51.3)
#### 更新日志:
- 性能优化.
- 删除冗余代码.
- 完善多语言支持.
- 修复bug:当软件语言为日语时,无法正常打开Readme.
- 修复bug:一些由本软件自动生成的缓存文件可能会被垃圾清理软件识别为空文件并删除.
- 修复bug:关机前的倒计时的时间不准确.
- 其他小改进.
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
- 新特性:支持记住上一次浏览的文件夹.
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
