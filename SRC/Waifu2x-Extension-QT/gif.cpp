#include "mainwindow.h"
#include "ui_mainwindow.h"

/*
video_gif_info.exe:
get video fps : [video_gif_info.exe videoFilePath fps]
get video frame number : [video_gif_info.exe videoFilePath countframe]
get gif duration : [video_gif_info.exe videoFilePath countframedigits]
*/

int MainWindow::Gif_getDuration(QString gifPath)
{
    QString Current_Path = qApp->applicationDirPath();
    QString program = Current_Path+"/video_gif_info.exe";
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
