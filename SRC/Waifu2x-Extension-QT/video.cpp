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
python_ext_waifu2xEX.exe:
get video fps : [python_ext_waifu2xEX.exe videoFilePath fps]
get video frame number : [python_ext_waifu2xEX.exe videoFilePath countframe]
get gif duration : [python_ext_waifu2xEX.exe videoFilePath countframedigits]
check update :[python_ext_waifu2xEX.exe null checkupdate]
*/

int MainWindow::video_get_fps(QString videoPath)
{
    QString program = Current_Path+"/python_ext_waifu2xEX.exe";
    QProcess vid;
    vid.start("\""+program+"\" \""+videoPath+"\" fps");
    while(!vid.waitForStarted(100)&&!QProcess_stop) {}
    while(!vid.waitForFinished(100)&&!QProcess_stop) {}
    int fps=vid.readAllStandardOutput().toInt();
    return fps;
}

int MainWindow::video_get_frameNumDigits(QString videoPath)
{
    QString program = Current_Path+"/python_ext_waifu2xEX.exe";
    QProcess vid;
    vid.start("\""+program+"\" \""+videoPath+"\" countframe");
    while(!vid.waitForStarted(100)&&!QProcess_stop) {}
    while(!vid.waitForFinished(100)&&!QProcess_stop) {}
    int FrameNum = vid.readAllStandardOutput().toInt();
    int frameNumDigits=1+(int)log10(FrameNum);
    return frameNumDigits;
}

void MainWindow::video_video2images(QString VideoPath,QString FrameFolderPath,QString AudioPath)
{
    QString ffmpeg_path = Current_Path+"/ffmpeg_waifu2xEX.exe";
    QFileInfo vfinfo(VideoPath);
    QString video_dir = vfinfo.path();
    if(video_dir.right(1)=="/")
    {
        video_dir = video_dir.left(video_dir.length() - 1);
    }
    QString video_filename = file_getBaseName(vfinfo.filePath());
    QString video_ext = vfinfo.suffix();
    QString video_mp4_fullpath = video_dir+"/"+video_filename+".mp4";
    //==============
    if(video_ext!="mp4")
    {
        video_mp4_fullpath = video_dir+"/"+video_filename+"_"+video_ext+".mp4";
    }
    else
    {
        video_mp4_fullpath = video_dir+"/"+video_filename+".mp4";
    }
    //======= 转换到mp4 =======
    if(video_ext!="mp4")
    {
        QString vcodec_copy_cmd = "";
        QString acodec_copy_cmd = "";
        QString bitrate_vid_cmd = "";
        QString bitrate_audio_cmd = "";
        if(ui->checkBox_videoSettings_isEnabled->checkState())
        {
            if(ui->checkBox_vcodec_copy_2mp4->checkState())
            {
                vcodec_copy_cmd = " -vcodec copy ";
            }
            else
            {
                bitrate_vid_cmd = " -b:v "+QString::number(ui->spinBox_bitrate_vid_2mp4->value(),10)+"k ";
            }
            if(ui->checkBox_acodec_copy_2mp4->checkState())
            {
                acodec_copy_cmd = " -acodec copy ";
            }
            else
            {
                bitrate_audio_cmd = " -b:a "+QString::number(ui->spinBox_bitrate_audio_2mp4->value(),10)+"k ";
            }
        }
        //=====
        QProcess video_tomp4;
        video_tomp4.start("\""+ffmpeg_path+"\" -y -i \""+VideoPath+"\" "+vcodec_copy_cmd+acodec_copy_cmd+bitrate_vid_cmd+bitrate_audio_cmd+" \""+video_mp4_fullpath+"\"");
        while(!video_tomp4.waitForStarted(100)&&!QProcess_stop) {}
        while(!video_tomp4.waitForFinished(100)&&!QProcess_stop) {}
    }
    //=====================
    int FrameNumDigits = video_get_frameNumDigits(video_mp4_fullpath);
    QProcess video_splitFrame;
    video_splitFrame.start("\""+ffmpeg_path+"\" -y -i \""+video_mp4_fullpath+"\" \""+FrameFolderPath+"/%0"+QString::number(FrameNumDigits,10)+"d.png\"");
    while(!video_splitFrame.waitForStarted(100)&&!QProcess_stop) {}
    while(!video_splitFrame.waitForFinished(100)&&!QProcess_stop) {}
    QStringList Frame_fileName_list= file_getFileNames_in_Folder_nofilter(FrameFolderPath);
    if(Frame_fileName_list.isEmpty())
    {
        video_splitFrame.start("\""+ffmpeg_path+"\" -y -i \""+video_mp4_fullpath+"\" \""+FrameFolderPath+"/%%0"+QString::number(FrameNumDigits,10)+"d.png\"");
        while(!video_splitFrame.waitForStarted(100)&&!QProcess_stop) {}
        while(!video_splitFrame.waitForFinished(100)&&!QProcess_stop) {}
    }
    QFile::remove(AudioPath);
    QProcess video_splitSound;
    video_splitSound.start("\""+ffmpeg_path+"\" -y -i \""+video_mp4_fullpath+"\" \""+AudioPath+"\"");
    while(!video_splitSound.waitForStarted(100)&&!QProcess_stop) {}
    while(!video_splitSound.waitForFinished(100)&&!QProcess_stop) {}
}

int MainWindow::video_images2video(QString VideoPath,QString video_mp4_scaled_fullpath,QString ScaledFrameFolderPath,QString AudioPath,bool CustRes_isEnabled,int CustRes_height,int CustRes_width)
{
    QString encoder_video_cmd="";
    QString bitrate_video_cmd="";
    //====
    QString encoder_audio_cmd="";
    QString bitrate_audio_cmd="";
    //===
    QString pixFormat_cmd=" -pix_fmt yuv420p ";
    //=======
    if(ui->checkBox_videoSettings_isEnabled->checkState())
    {
        encoder_video_cmd=" -c:v "+ui->lineEdit_encoder_vid->text()+" ";
        bitrate_video_cmd=" -b:v "+QString::number(ui->spinBox_bitrate_vid->value(),10)+"k ";
        encoder_audio_cmd=" -c:a "+ui->lineEdit_encoder_audio->text()+" ";
        bitrate_audio_cmd=" -b:a "+QString::number(ui->spinBox_bitrate_audio->value(),10)+"k ";
        pixFormat_cmd=" -pix_fmt "+ui->lineEdit_pixformat->text()+" ";
    }
    //=====
    QString resize_cmd ="";
    if(CustRes_isEnabled)
    {
        if(CustRes_AspectRatioMode==Qt::IgnoreAspectRatio)
        {
            resize_cmd =" -vf scale="+QString::number(CustRes_width,10)+":"+QString::number(CustRes_height,10)+" ";
        }
        if(CustRes_AspectRatioMode==Qt::KeepAspectRatio)
        {
            if(CustRes_width>=CustRes_height)
            {
                resize_cmd =" -vf scale=-2:"+QString::number(CustRes_height,10)+" ";
            }
            else
            {
                resize_cmd =" -vf scale="+QString::number(CustRes_width,10)+":-2 ";
            }
        }
        if(CustRes_AspectRatioMode==Qt::KeepAspectRatioByExpanding)
        {
            if(CustRes_width>=CustRes_height)
            {
                resize_cmd =" -vf scale="+QString::number(CustRes_width,10)+":-2 ";
            }
            else
            {
                resize_cmd =" -vf scale=-2:"+QString::number(CustRes_height,10)+" ";
            }
        }
    }
    //===========
    QString ffmpeg_path = Current_Path+"/ffmpeg_waifu2xEX.exe";
    int FrameNumDigits = video_get_frameNumDigits(VideoPath);
    QFileInfo vfinfo(VideoPath);
    QString video_dir = vfinfo.path();
    if(video_dir.right(1)=="/")
    {
        video_dir = video_dir.left(video_dir.length() - 1);
    }
    QString video_filename = file_getBaseName(vfinfo.filePath());
    QString video_ext = vfinfo.suffix();
    int fps = video_get_fps(VideoPath);
    if(fps<=0)
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+VideoPath+tr("]. Error: [Unable to get video frame rate.]"));
        return 0;
    }
    QString CMD = "";
    if(file_isFileExist(AudioPath))
    {
        CMD = "\""+ffmpeg_path+"\" -y -f image2 -framerate "+QString::number(fps,10)+" -i \""+ScaledFrameFolderPath+"/%0"+QString::number(FrameNumDigits,10)+"d.png\" -i \""+AudioPath+"\" -r "+QString::number(fps,10)+encoder_video_cmd+bitrate_video_cmd+encoder_audio_cmd+bitrate_audio_cmd+pixFormat_cmd+resize_cmd+" \""+video_mp4_scaled_fullpath+"\"";
    }
    else
    {
        CMD = "\""+ffmpeg_path+"\" -y -f image2 -framerate "+QString::number(fps,10)+" -i \""+ScaledFrameFolderPath+"/%0"+QString::number(FrameNumDigits,10)+"d.png\" -r "+QString::number(fps,10)+encoder_video_cmd+bitrate_video_cmd+pixFormat_cmd+resize_cmd+" \""+video_mp4_scaled_fullpath+"\"";
    }
    QProcess images2video;
    images2video.start(CMD);
    while(!images2video.waitForStarted(100)&&!QProcess_stop) {}
    while(!images2video.waitForFinished(100)&&!QProcess_stop) {}
    //======
    if(!file_isFileExist(video_mp4_scaled_fullpath))
    {
        if(file_isFileExist(AudioPath))
        {
            CMD = "\""+ffmpeg_path+"\" -y -f image2 -framerate "+QString::number(fps,10)+" -i \""+ScaledFrameFolderPath+"/%%00d.png\" -i \""+AudioPath+"\" -r "+QString::number(fps,10)+encoder_video_cmd+bitrate_video_cmd+encoder_audio_cmd+bitrate_audio_cmd+pixFormat_cmd+resize_cmd+" \""+video_mp4_scaled_fullpath+"\"";
        }
        else
        {
            CMD = "\""+ffmpeg_path+"\" -y -f image2 -framerate "+QString::number(fps,10)+" -i \""+ScaledFrameFolderPath+"/%%00d.png\" -r "+QString::number(fps,10)+encoder_video_cmd+bitrate_video_cmd+pixFormat_cmd+resize_cmd+" \""+video_mp4_scaled_fullpath+"\"";
        }
        QProcess images2video;
        images2video.start(CMD);
        while(!images2video.waitForStarted(100)&&!QProcess_stop) {}
        while(!images2video.waitForFinished(100)&&!QProcess_stop) {}
    }
    QFile::remove(AudioPath);
    return 0;
}
