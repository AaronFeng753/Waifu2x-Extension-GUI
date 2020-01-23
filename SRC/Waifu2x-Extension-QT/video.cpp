#include "mainwindow.h"
#include "ui_mainwindow.h"


/*
video_gif_info.exe:
get video fps : [video_gif_info.exe videoFilePath fps]
get video frame number : [video_gif_info.exe videoFilePath countframe]
get gif duration : [video_gif_info.exe videoFilePath countframedigits]
*/

int MainWindow::video_get_fps(QString videoPath)
{
    QString Current_Path = qApp->applicationDirPath();
    QString program = Current_Path+"/video_gif_info.exe";
    QProcess vid;
    vid.start("\""+program+"\" \""+videoPath+"\" fps");
    vid.waitForStarted();
    vid.waitForFinished();
    int fps=vid.readAllStandardOutput().toInt();
    return fps;
}

int MainWindow::video_get_frameNumDigits(QString videoPath)
{
    QString Current_Path = qApp->applicationDirPath();
    QString program = Current_Path+"/video_gif_info.exe";
    QProcess vid;
    vid.start("\""+program+"\" \""+videoPath+"\" countframe");
    vid.waitForStarted();
    vid.waitForFinished();
    int FrameNum = vid.readAllStandardOutput().toInt();
    int frameNumDigits=1+(int)log10(FrameNum);
    return frameNumDigits;
}

void MainWindow::video_video2images(QString VideoPath,QString FrameFolderPath)
{
    QString Current_Path = qApp->applicationDirPath();
    QString ffmpeg_path = Current_Path+"/ffmpeg.exe";
    QFileInfo vfinfo(VideoPath);
    QString video_dir = vfinfo.path();
    if(video_dir.right(1)=="/")
    {
        video_dir = video_dir.left(video_dir.length() - 1);
    }
    QString video_filename = vfinfo.baseName();
    QString video_ext = vfinfo.suffix();
    QString video_mp4_fullpath = video_dir+"/"+video_filename+".mp4";
    if(video_ext!="mp4")
    {
        QProcess video_tomp4;
        video_tomp4.start("\""+ffmpeg_path+"\" -y -i \""+VideoPath+"\" \""+video_mp4_fullpath+"\"");
        video_tomp4.waitForStarted();
        video_tomp4.waitForFinished();
    }
    QProcess video_splitFrame;
    video_splitFrame.start("\""+ffmpeg_path+"\" -y -i \""+video_mp4_fullpath+"\" \""+FrameFolderPath+"/%00d.png\"");
    video_splitFrame.waitForStarted();
    video_splitFrame.waitForFinished();
    QStringList Frame_fileName_list= file_getFileNames_in_Folder_nofilter(FrameFolderPath);
    if(Frame_fileName_list.isEmpty())
    {
        video_splitFrame.start("\""+ffmpeg_path+"\" -y -i \""+video_mp4_fullpath+"\" \""+FrameFolderPath+"/%%00d.png\"");
        video_splitFrame.waitForStarted();
        video_splitFrame.waitForFinished();
    }
    QFile::remove(video_dir+"/audio_waifu2x.wav");
    QProcess video_splitSound;
    video_splitSound.start("\""+ffmpeg_path+"\" -y -i \""+video_mp4_fullpath+"\" \""+video_dir+"/audio_waifu2x.wav\"");
    video_splitSound.waitForStarted();
    video_splitSound.waitForFinished();
}

int MainWindow::video_images2video(QString VideoPath,QString ScaledFrameFolderPath)
{
    QString Current_Path = qApp->applicationDirPath();
    QString ffmpeg_path = Current_Path+"/ffmpeg.exe";
    QFileInfo vfinfo(VideoPath);
    QString video_dir = vfinfo.path();
    if(video_dir.right(1)=="/")
    {
        video_dir = video_dir.left(video_dir.length() - 1);
    }
    QString video_filename = vfinfo.baseName();
    QString video_mp4_scaled_fullpath = video_dir+"/"+video_filename+"_waifu2x.mp4";
    int fps = video_get_fps(VideoPath);
    if(fps<=0)
    {
        return 0;
    }
    QString AudioPath = video_dir+"/audio_waifu2x.wav";
    QString CMD = "";
    if(file_isFileExist(AudioPath))
    {
        CMD = "\""+ffmpeg_path+"\" -y -f image2 -framerate "+QString::number(fps,10)+" -i \""+ScaledFrameFolderPath+"/%00d.png\" -i \""+AudioPath+"\" -r "+QString::number(fps,10)+" -pix_fmt yuv420p \""+video_mp4_scaled_fullpath+"\"";
    }
    else
    {
        CMD = "\""+ffmpeg_path+"\" -y -f image2 -framerate "+QString::number(fps,10)+" -i \""+ScaledFrameFolderPath+"/%00d.png\" -r "+QString::number(fps,10)+" -pix_fmt yuv420p \""+video_mp4_scaled_fullpath+"\"";
    }
    QProcess images2video;
    images2video.start(CMD);
    images2video.waitForStarted();
    images2video.waitForFinished();
    if(!file_isFileExist(video_mp4_scaled_fullpath))
    {
        if(file_isFileExist(AudioPath))
        {
            CMD = "\""+ffmpeg_path+"\" -y -f image2 -framerate "+QString::number(fps,10)+" -i \""+ScaledFrameFolderPath+"/%%00d.png\" -i \""+AudioPath+"\" -r "+QString::number(fps,10)+" -pix_fmt yuv420p \""+video_mp4_scaled_fullpath+"\"";
        }
        else
        {
            CMD = "\""+ffmpeg_path+"\" -y -f image2 -framerate "+QString::number(fps,10)+" -i \""+ScaledFrameFolderPath+"/%%00d.png\" -r "+QString::number(fps,10)+" -pix_fmt yuv420p \""+video_mp4_scaled_fullpath+"\"";
        }
        QProcess images2video;
        images2video.start(CMD);
        images2video.waitForStarted();
        images2video.waitForFinished();
    }
    QFile::remove(AudioPath);
    return 0;
}
