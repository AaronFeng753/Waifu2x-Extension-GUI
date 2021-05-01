@echo off
color f0
title = FFmpeg encoders list - @Waifu2x-Extension-GUI
cd /d %~dp0
cls
ffmpeg_waifu2xEX.exe -hide_banner -loglevel panic -encoders
pause