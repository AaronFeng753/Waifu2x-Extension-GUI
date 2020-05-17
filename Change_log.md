# Change log
---
#### [中文版](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/blob/master/Change_log_CN.md)
---
## [v2.11.3](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v2.11.3)
#### Change log:
- New feature: Anime4K engine supports processing Image and GIF.
- New feature: Automatically adjust engine settings based on compatibility test results.
- After the compatibility test, a pop up will shows up according to the test results.
- Update Anime4K (v1.8.1).
- Update ImageMagick (7.0.10-12-Q16-x64).
- Change default settings.
- Add some tips.
- Check whether the software has write permission in the directory where the file is located before processing the file.
- Improved permission detection when the software starts.
- Fix bug: In special cases, the video may be stuck in the step of obtaining the duration.
- Fix bug: Can't detect whether the software has write permission for output path normally.
- Fix bug: when using Anime4k engine to process video, the custom resolution setting is invalid.
---
## [v2.02.3](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v2.02.3)
#### Change log:
- Update Anime4K (v1.7.0).
- Improve Simplified Chinese support.
- Reduce resource usage when running compatibility tests.
- Add some tips.
- Fix bug: In some cases, the program will get stuck when running compatibility tests.
- Fix bug: Some components are not disabled when processing files, which may cause potential errors.
---
## [v2.01.1](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v2.01.1)
#### Change log:
- New feature: Redesigned user interface.
- New feature: add [繁體中文] support (thanks @uimee for assistance in translation).
- New feature: Anime4K engine supports specified GPU.
- New feature: store and read GPU settings of all engines.
- New features: improved compatibility testing, increased inspection items, and support for storing test results.
- Update Anime4K (v1.6.0).
- Update ImageMagick (7.0.10-10-Q16-x64).
- Improved multi-thread management.
- Add some tips.
- Performance optimization.
- Fix bugs.
---
## [v1.73.3](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v1.73.3)
#### Change log:
- New feature: Add GPU acceleration support for Anime4K engine.
- New feature: After installing the software, a pop-up window allows users to choose their own language when they first start up.
- Update Anime4K (v1.5.0)
- It is no longer allowed to disable the automatic check for updates, but the automatic update pop-up window can be disabled. After disabling the pop-up window, an update prompt will be displayed in the text box.
- A pop-up window prompts the user to view the test results after the compatibility test.
- Improve compatibility test.
- Hide ETA and remaining time when GIF or video is included in the file list.
- Improve multi-language support.
- Fix bugs.
---
## [v1.71.1](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v1.71.1)
#### Change log:
- New feature: replaced the previous Anime4K-JAVA with Anime4KCPP.Anime4KCPP is faster and has better image quality than Anime4K-JAVA.
- New feature: Add a lot of custom settings for the Anime4K engine.
- Interface layout adjustment.
- Fixe translation errors.
- Modify some texts.
---
## [v1.63.1](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v1.63.1)
#### Change log:
- Improve stability.
- Optimize performance.
- Add some tips.
- Change some text.
- Fix translation errors.
- Fix bug: In rare cases, the cached files will have the same name and cause an error.
- Fix bug: Reset video settings button does not work properly.
---
## [v1.62.5](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v1.62.5)
#### Change log:
- Fix bug: when the `Process video by segment` is enabled, the cache cannot be read normally, which will cause the software unable to continue the previous progress.(Thanks to Esper Wang for feedback✔)
---
## [v1.62.4](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v1.62.4)
#### Change log:
- Fix bug: In some cases, the output video will be out of sync.(thanks to  @FITHRPG6 for feedback)
---
## [v1.62.3](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v1.62.3)
#### Change log:
- New feature: waifu2x-ncnn-vulkan adds fp16p support, which improves compatibility with some old drivers.
- Update waifu2x-ncnn-vulkan (version 20200414).
- Update ImageMagick (7.0.10-6-Q16-x64).
- Change the default custom resolution aspect ratio strategy to keep aspect ratio.
- Fixe bug: interface display error in some cases.
- Fixe bug: when the source video file name contains Chinese, the video cannot be assembled correctly after processing the video in segments.
---
## [v1.61.3](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v1.61.3)
#### Change log:
- New feature: Processing video in segments, greatly reducing the hard disk space occupied when processing video. 🎉🥳🎉 (Optional, additional settings, enabled by default)
- Improve multi-language support.
- Other minor improvements.
- Fix bug: When the processing process is stopped, the ffmpeg process will be forcibly terminated, which will cause the cache file cannot be generated normally.
- Fix bug: In some cases, the progress bar cannot display the progress normally.
---
## [v1.51.3](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v1.51.3)
#### Change log:
- Performance optimization.
- Remove redundant code.
- Improve multi-language support.
- Fix bug: When the software language is Japanese, the Readme cannot be opened normally.
- Fixed bug: Some cache files automatically generated by this software may be recognized as empty files by junk cleaning software and deleted.
- Fix bug: Inaccurate countdown time before shutdown.
- Other small improvements.
---
## [v1.50.2](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v1.50.2)
#### Change log:
- New feature: Use SoX (14.4.2-win32) to reduce the noise of the video's audio. (Optional, additional settings, disabled by default) (thanks @wenbob for the suggestion)
- New feature: Software settings will be saved before automatically shutting down the computer.
- Optimized interface display.
- Improve multi-language support and fix translation errors.
- Fix bug: When [Keep Video Cache] is enabled, the source video files will still be deleted.
---
## [v1.45](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v1.45)
#### Change log:
- New feature: You can choose whether to show a pop up when exit. (Optional, additional settings, enabled by default)
- New feature: Automatically enable(or disable) [Auto Detect Alpha Channel] based on the results of the compatibility test.
- New feature: Keep video cache. (Optional, additional settings, enabled by default)
- New feature: When the user stops the task or an error occurs, the video's cache will not be automatically deleted, and it can also be read by the software to continue the previous progress.
- Improve the stability of multi-threaded scheduling.
- Fix bug: In some cases, the file cannot be renamed properly then the video cannot be assembled normally.
- Fix bug: The files in the list could not be processed normally due to the thread scheduling error.
---
## [v1.37](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v1.37)
#### Change log:
- New feature: When converting videos to mp4, support to automatically obtain the bit rate of the original video and apply it to the converted mp4.
- New feature: When outputting video, the bit rate will be allocated using the software's built-in algorithm.
- Change the default [compressed ".jpg" image quality] value to 95.
- Fix bug: Progress bar could not display progress normally in some cases.
---
## [v1.31](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v1.31)
#### Change log:
- New feature: Automatically determine whether the picture has an alpha channel and switch to a compatible engine. (Optional, additional settings, disabled by default) (`Please make sure your computer is compatible with waiuf2x-converter before enabling this feature.`)
- New feature: Compatibility tests run automatically the first time the software is launched.
- Improved multi-thread management and stability.
- Remove the [Show Tips] button.
- Fix bug: In some cases, outputting the processor list of waifu2x-converter will cause the software to enter an endless loop.
---
## [v1.21](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v1.21)
#### Change log:
- New feature: support remember the last browsed folder
- New feature: Improved automatic check for updates, support for displaying update logs
- New feature: Launcher added multi-open detection.
- New feature: Support for typing extra commands in video customization settings.
- Update ImageMagick (7.0.9-27-Q16-x64)
- Update User-Agent used when checking for updates automatically.
- Changed some interface layouts.
- Improved stability.
- Added some tips.
- Modified some confusing text.
---
## [v1.11](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v1.11)
#### Change log:
- New feature: Add SRMD-NCNN-Vulkan engine. 🎉🥳🎉
The SRMD-NCNN-Vulkan engine can support both `2D Anime` and `3D Real-life` image styles.
You can [click this page](https://github.com/cszn/SRMD) to learn about SRMD.
---
## [v1.01](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v1.01)
#### Change log:
- ### End the beta test and start the development of the stable version. 🎉🥳🎉
- New feature: Add [ 日本語 Japanese ] language support.  🐱‍👤
`(Machine translation, may contain errors)`
- Added some tips.
- Fix bug: text error.
- Fixed bug: After switching languages, some texts were displayed incorrectly.
---
## [v0.57-beta](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v0.57-beta)
#### Change log:
- Add PayPal donate button
- Rewrite code related to file list to speed up operation and reduce memory consumption.
- Fix bug: When there are many entries in the list, removing a single entry will cause no response.
- Fix bug: After specifying the output path, files with the same file name will overwrite each other.
- Fix bug: New and old versions of waifu2x-ncnn-vulkan are not distinguished when detecting available GPU ID
---
## [v0.56-beta](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v0.56-beta)
#### Change log:
- Fix bug: slow reading files speed
- Fix bug: unable to stop processing file process normally due to waiting for ffmpeg to split video (or GIF)
- Fix bug: In some cases, external plugins are still running when exiting the program
---
## [v0.55-beta](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v0.55-beta)
#### Change log:
- Old version of waifu2x-ncnn-vulkan engine is enabled by default. (Because of the stability issue of new version of waifu2x-ncnn-vulkan)
`If you need to use the TTA feature of the waifu2x-ncnn-vulkan engine, switch to the new version.`
- Improve stability.
- Fix bug: Adding a large number of files at one time will cause the program to become unresponsive.
---
## [v0.54-beta](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v0.54-beta)
#### Change log:
- New feature: When the software is closed, a pop-up window asks the user whether to confirm the exit.
- New feature: Added [Forced Retry], when waifu2x thread is stuck, you can perform forced retry to remove the stuck, and it will not interrupt the entire [scale and denoise] process
- Improve error detection of waifu2x-ncnn-vulkan engine.
- Fix bug: not delete all temporary files in some cases
- Fix bug: In some special cases, multi-threaded scheduling error will cause stuck
---
## [v0.53-beta](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v0.53-beta)
#### Change log:
- New feature: Support switching waifu2x-ncnn-vulkan engine version. When your computer has problems using waifu2x-ncnn-vulkan engine, you can try to update the graphics card driver or enable the old version of waifu2x-ncnn-vulkan engine
- New feature: Added compatibility test for older versions of waifu2x-ncnn-vulkan engine
- New feature: Custom retry times (additional settings)
- Try to fix the problem of high DPI screen display. (I cannot test because I don’t have a high DPI display myself)
- Fix bug: text error
---
## [v0.52-beta](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v0.52-beta)
#### Change log:
- Improved stability, will retry when the engine crashes, and will not fail the entire process because of an engine crash
- Improve the stability of Anime4k engine
- Disable lineEdit of output path when output path is not enabled
- Fix text errors
- Update waifu2x-ncnn-vulkan (version 20200224)
---
## [v0.51-beta](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v0.51-beta)
#### Change log:
- New feature: When "Delete original file" is enabled, you can choose to move the original file to the recycle bin
- New feature: Support to choose whether to enable custom video settings (additional settings, optional, disabled by default)
- New feature: Specify output directory
- New feature: Scan subfolders (additional settings, optional, enabled by default)
- New feature: Added TTA support for waifu2x-ncnn-vulkan engine
- New feature: Added TTA support for waifu2x-converter engine
- New feature: file list interactivity settings (additional settings)
- Update waifu2x-ncnn-vulkan (20200223)
- Performance optimization
- Improve error detection
- Fix bug: text error
---
## [v0.461-beta](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v0.461-beta)
#### v0.461-beta Change log:
- Fix bug: Source video files may get lost in some cases
#### v0.46-beta Change log:
- New feature: Support for more video-related custom parameters
- New feature: Show prompts on initial startup
- Update waifu2x-ncnn-vulkan (20200222)
- Interface layout adjustment
- Enhance video quality
- Fix bug: Temporary mp4 video generated by software was not deleted when processing stopped
- Fix bug: When waifu2x-ncnn-vulkan is enabled, the target magnification cannot be calculated correctly (thanks to slimekingwin8cn for feedback)
---
## [v0.45-beta](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v0.45-beta)
#### Change log:
- New feature: waifu2x-ncnn-vulkan engine supports switching to cunet model
- New feature: custom video encoder, bitrate and pixel format (in additional settings)
- New feature: Custom resolution supports apply settings to all files together
- Enhanced video quality
- Performance optimization, reducing the time required to process GIF and video when custom resolution is enabled
- Fix bug: target magnification cannot be calculated correctly when custom resolution is enabled
---
## [v0.411-beta](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v0.411-beta)
#### v0.411 Change log:
- Improve compatibility of waifu2x-converter
#### v0.41 Change log:
- New feature: waifu2x-converter supports more noise reduction levels
- New feature: Added waifu2x-converter engine custom settings
- New feature: custom font
- New feature: minimize form
- New feature: hide text browser
- Update waifu2x-converter (v5.3.3) (thanks to NyanMisaka for feedback)
- Interface layout adjustment
- Improve stability
- Many other improvements
- Fix bug: File extensions are not recognized correctly (thanks to zhehenkaifa for feedback)
- Fix bug: When the video extension is ".MP4" instead of ".mp4", it will cause an error
- Fix bug: error caused by extra "." In file name (thanks to 我叫黄伟达 for feedback)
- Fix bug: unable to stop processing in some special cases
---
## [v0.37-beta](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v0.37-beta)
#### Change log:
- New feature: Custom ".jpg" file compressed image quality
- Improve file extension filtering in browse file dialog
- Update ImageMagick (7.0.9-22-Q16-x64)
- Improve stability
- Improve Simplified Chinese support
- Other small improvements
- Fix bug: stuck in the background after the program is closed
---
## [v0.35-beta](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v0.35-beta)
#### Change log:
- New feature: Custom global font size (additional settings)
- New feature: Browse and add files
- Performance optimization
- Improve stability
- Review code and add code comments
- Interface layout adjustment
- Multiple minor improvements
- Fix bug: Cannot be compatible with custom scaling of Windows system
- Fix bug: In some special cases, it can't judge whether the file is added repeatedly
- Fix bug: Can't save noise reduction level setting
---
## [v0.31-beta](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v0.31-beta)
#### Change log:
- New feature: show progress while loading file list
- New feature: Customizable aspect ratio strategy of "Custom Resolution" (in [Additional Settings])
- New feature: popup prompt after manually saving settings
- New feature: "Settings" can always be hidden (optional, disabled by default, in [Additional Settings])
- New feature: Show total number of GIF and video threads
- Fine-tune interface layout
- Fix bug: When processing gif, the file processing status cannot be changed normally after stopping the process
---
## [v0.291-beta](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v0.291-beta)
#### Change log:
- New feature: Automatically save settings when closing the program (optional, enabled by default)
- New feature: [input path] can be hidden all the time (optional, disabled by default)
- New feature: Add [About]
- New feature: Permission detection
- Adjust the interface layout, move some setting options to [Additional settings]
- Improve Simplified Chinese support
- Fix bug: In some special cases, the program will be stuck in the background
---
## [v0.281-beta](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v0.281-beta)
#### Change log:
- New feature: [Files List] supports deselect
- Interface Beautification
- Adjust interface layout
- Improve Simplified Chinese support
- Fix bug: GIF frames could not be arranged properly in some cases
- Fix bug: In some cases, the thread will be automatically closed in advance and the task cannot be completed
- Fix bug: In some cases, the program cannot exit completely and will stay in the background to execute an infinite loop
- Fix bug: When there is no Java runtime environment in the system, calling Anime4k may cause the software to enter an infinite loop
---
## [v0.25-beta](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v0.25-beta)
#### Change log:
- New feature: Read screen resolution and set as default custom resolution value when first started
- New feature: When loading File List, automatically output items with status [Failed] in Textbrowser
- Update FFmpeg (4.2.2)
- Fix bug: In some special cases, automatic check for updates will falsely report the existence of a new version
- Fix bug: Automatic shutdown of computer could not be performed normally
- Fixed bug: [File Count] cannot be displayed normally after language switching
- Improve Simplified Chinese language support
---
## [v0.21-beta](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v0.21-beta)
#### Change log:
- New feature: [Completed number of files/Total number of files] is displayed on the left side of the progress bar.
- New feature: Added setting options to control whether to automatically check for updates at startup (It is `not recommended` to turn off automatic updates)
- New feature: The total number of files is displayed below the file list.
- New feature: Save/Read file list
- New feature: Automatically save the file list before automatically shutting down the computer,  you can manually load the file list to check the file processing status after the next software startup
- New feature: pop-up window prompt before auto shutdown
- Improve Simplified Chinese language support
- Select the readme to open based on the current language
- Before processing a file, check if it exists
- Fix bug: After saving the settings, the [Image Style] settings that were saved cannot be read normally
- Fix bug: After removing a single row, the custom resolution settings of all remaining rows will be cleared
- Fix bug: When there are files with the same file name and different extensions in the file list, if multi-threading is enabled, it will cause read and write conflicts.
- Fix bug: The problem that the new version download page cannot be opened normally after detecting the new version automatically.
---
## [v0.12-beta](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v0.12-beta)
#### Change log:
- NEW Feature: Check for updates automatically.
- Fix bug: GIF and video cannot read the scale ratio setting normally.
- Fix bug: When processing videos and gif files, if an error occurs, it will cause thread management problems.
---
## [v0.11-beta](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v0.11-beta)
#### Change log:
- NEW Feature: Customize the resolution of image, GIF and video output. (At the request of an anonymous donor.🧐)
- NEW Feature: Multi-language.(Add Simplified Chinese.)(At the request of an anonymous donor.🧐)
- NEW Feature: Support to save the current settings, automatically restore the last saved settings when starting the software.
- NEW Feature: Switch the display status of setting options. (Hide / Show)
- NEW Feature: Switch the display status of [Input]. (Hide / Show)
- NEW Feature: Prompt when misoperation.
- NEW Feature: Automatically scroll to the bottom after changing the font size of Textbrowser.
- NEW Feature: Textbrowser automatically slides to the bottom after outputting new content
- NEW Feature: Tableview automatically adjusts sieze based on content
- NEW Feature: Check available GPU and set GPU ID.
- Improve tableview selection logic. When one tableview is selected, the other two are automatically unselected.
- Optimize the tableview display logic. After inputting the file, only change the display state of the corresponding tableview.
- Remove redundant code
- Fix bug: [Remove item] will cause the program to crash when the file is finished and [Re-process finished files] is not enabled.
- Fix bug: [video_gif_info.exe] Chinese characters cannot be read normally
- Fix bug: [Compatibility check] button is not automatically enabled after all files have been processed.
---
## [v0.01-beta](https://github.com/AaronFeng753/Waifu2x-Extension-GUI/releases/tag/v0.01-beta)
#### Change log:
- First release.
