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
#include "ui_mainwindow.h"

/*
python_ext.exe:
get video fps : [python_ext.exe videoFilePath fps]
get video frame number : [python_ext.exe videoFilePath countframe]
get gif duration : [python_ext.exe videoFilePath countframedigits]
check update :[python_ext.exe null checkupdate]
*/

int MainWindow::Gif_getDuration(QString gifPath)
{
    QString Current_Path = qApp->applicationDirPath();
    QString program = Current_Path+"/python_ext.exe";
    QProcess GifDuration;
    GifDuration.start("\""+program+"\" \""+gifPath+"\" duration");
    GifDuration.waitForStarted();
    GifDuration.waitForFinished();
    int Duration=GifDuration.readAllStandardOutput().toInt();
    if(Duration<=0)
    {
        QMovie movie(gifPath);
        movie.setSpeed(1);
        movie.start();
        movie.stop();
        Duration = ((movie.nextFrameDelay()/100)+1)/10;
    }
    return Duration;
}

int MainWindow::Gif_getFrameDigits(QString gifPath)
{
    QMovie movie(gifPath);
    int FrameCount=movie.frameCount();
    FrameCount = 1+(int)log10(FrameCount);//获取frame位数
    return FrameCount;
}

void MainWindow::Gif_splitGif(QString gifPath,QString SplitFramesFolderPath)
{
    QString Current_Path = qApp->applicationDirPath();
    QString program = Current_Path+"/ffmpeg.exe";
    QString cmd = "\"" + program + "\"" + " -i " + "\"" + gifPath + "\"" + " " + "\"" + SplitFramesFolderPath + "/%00d.png\"";
    QProcess *SplitGIF=new QProcess();
    SplitGIF->start(cmd);
    SplitGIF->waitForStarted();
    SplitGIF->waitForFinished();
    QStringList Frame_fileName_list= file_getFileNames_in_Folder_nofilter(SplitFramesFolderPath);
    if(Frame_fileName_list.isEmpty())
    {
        QString cmd = "\"" + program + "\"" + " -i " + "\"" + gifPath + "\"" + " " + "\"" + SplitFramesFolderPath + "/%%00d.png\"";
        QProcess *SplitGIF=new QProcess();
        SplitGIF->start(cmd);
        SplitGIF->waitForStarted();
        SplitGIF->waitForFinished();
    }
}


void MainWindow::Gif_assembleGif(QString ResGifPath,QString ScaledFramesPath,int Duration)
{
    QString Current_Path = qApp->applicationDirPath();
    QString program = Current_Path+"/convert.exe";
    QString cmd = "\"" + program + "\"" + " -delay " + QString::number(Duration, 10) + " -loop 0 " + "\"" + ScaledFramesPath + "/*png\" \""+ResGifPath+"\"";
    QProcess *AssembleGIF=new QProcess();
    AssembleGIF->start(cmd);
    AssembleGIF->waitForStarted();
    AssembleGIF->waitForFinished();
}


void MainWindow::Gif_compressGif(QString gifPath,QString gifPath_compressd)
{
    QString Current_Path = qApp->applicationDirPath();
    QString program = Current_Path+"/gifsicle.exe";
    QString cmd = "\"" + program + "\"" + " -O3 -i \""+gifPath+"\" -o \""+gifPath_compressd+"\"";
    QProcess *CompressGIF=new QProcess();
    CompressGIF->start(cmd);
    CompressGIF->waitForStarted();
    CompressGIF->waitForFinished();
}
