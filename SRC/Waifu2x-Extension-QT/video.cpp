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

QString MainWindow::video_AudioDenoise(QString OriginalAudioPath)
{
    /*
    sox 输入音频.wav -n noiseprof 噪音分析.prof
    sox 输入音频.wav 输出音频.wav noisered 噪音分析.prof 0.21
    */
    QFileInfo fileinfo(OriginalAudioPath);
    QString file_name = file_getBaseName(fileinfo.filePath());
    QString file_ext = fileinfo.suffix();
    QString file_path = file_getFolderPath(fileinfo);
    //================
    QString program = Current_Path+"/SoX/sox_waifu2xEX.exe";
    QString DenoiseProfile = file_path+"/"+file_name+"_DenoiseProfile.dp";
    QString DenoisedAudio = file_path+"/"+file_name+"_Denoised."+file_ext;
    double DenoiseLevel = ui->doubleSpinBox_AudioDenoiseLevel->value();
    //================
    QProcess vid;
    vid.start("\""+program+"\" \""+OriginalAudioPath+"\" -n noiseprof \""+DenoiseProfile+"\"");
    while(!vid.waitForStarted(100)&&!QProcess_stop) {}
    while(!vid.waitForFinished(100)&&!QProcess_stop) {}
    //================
    vid.start("\""+program+"\" \""+OriginalAudioPath+"\" \""+DenoisedAudio+"\" noisered \""+DenoiseProfile+"\" "+QString("%1").arg(DenoiseLevel));
    while(!vid.waitForStarted(100)&&!QProcess_stop) {}
    while(!vid.waitForFinished(100)&&!QProcess_stop) {}
    //================
    if(file_isFileExist(DenoisedAudio))
    {
        QFile::remove(DenoiseProfile);
        return DenoisedAudio;
    }
    else
    {
        emit Send_TextBrowser_NewMessage(tr("Error! Unable to denoise audio.[")+OriginalAudioPath+"]");
        return OriginalAudioPath;
    }
}

void MainWindow::video_write_VideoConfiguration(QString VideoConfiguration_fullPath,int ScaleRatio,int DenoiseLevel,bool CustRes_isEnabled,int CustRes_height,int CustRes_width,QString EngineName)
{
    QSettings *configIniWrite = new QSettings(VideoConfiguration_fullPath, QSettings::IniFormat);
    configIniWrite->setIniCodec(QTextCodec::codecForName("UTF-8"));
    //================= 添加警告 =========================
    configIniWrite->setValue("/Warning/EN", "Do not modify this file! It may cause the program to crash! If problems occur after the modification, delete this article and restart the program.");
    //==================== 存储视频信息 ==================================
    configIniWrite->setValue("/VideoConfiguration/ScaleRatio", ScaleRatio);
    configIniWrite->setValue("/VideoConfiguration/DenoiseLevel", DenoiseLevel);
    configIniWrite->setValue("/VideoConfiguration/CustRes_isEnabled", CustRes_isEnabled);
    configIniWrite->setValue("/VideoConfiguration/CustRes_height", CustRes_height);
    configIniWrite->setValue("/VideoConfiguration/CustRes_width", CustRes_width);
    configIniWrite->setValue("/VideoConfiguration/EngineName", EngineName);
}
/*
python_ext_waifu2xEX.exe:
get video fps : [python_ext_waifu2xEX.exe videoFilePath fps]
get video frame number : [python_ext_waifu2xEX.exe videoFilePath countframe]
get gif duration : [python_ext_waifu2xEX.exe videoFilePath countframedigits]
check update :[python_ext_waifu2xEX.exe Current_Path checkupdate]
*/
QString MainWindow::video_get_bitrate_AccordingToRes(QString ScaledFrameFolderPath)
{
    QStringList flist = file_getFileNames_in_Folder_nofilter(ScaledFrameFolderPath);
    flist.removeAll("..");
    flist.removeAll(".");
    QString Full_Path_File = "";
    if(!flist.isEmpty())
    {
        for(int i = 0; i < flist.size(); i++)
        {
            QString tmp = flist.at(i);
            Full_Path_File = ScaledFrameFolderPath + "/" + tmp;
            QFileInfo finfo(Full_Path_File);
            if(finfo.suffix()=="png")break;
        }
    }
    QImage qimage_original;
    qimage_original.load(Full_Path_File);
    int original_height = qimage_original.height();
    int original_width = qimage_original.width();
    if(original_height<=0||original_width<=0)
    {
        return "";
    }
    if(original_height<=original_width)
    {
        return QString::number(original_height*6,10);
    }
    else
    {
        return QString::number(original_width*6,10);
    }
}

QString MainWindow::video_get_bitrate(QString videoPath)
{
    //========================= 调用ffprobe读取视频信息 ======================
    QProcess *Get_Bitrate_process = new QProcess();
    QString cmd = Current_Path+"/ffprobe_waifu2xEX.exe -i \""+videoPath+"\" -v quiet -print_format ini -show_format";
    Get_Bitrate_process->start(cmd);
    while(!Get_Bitrate_process->waitForStarted(100)&&!QProcess_stop) {}
    while(!Get_Bitrate_process->waitForFinished(100)&&!QProcess_stop) {}
    //============= 保存ffprobe输出的ini格式文本 =============
    QString ffprobe_output_str = Get_Bitrate_process->readAllStandardOutput();
    //================ 将ini写入文件保存 ================
    //创建文件夹
    QString video_info_dir = Current_Path+"/videoInfo";
    if(!file_isDirExist(video_info_dir))
    {
        file_mkDir(video_info_dir);
    }
    //========================
    QFileInfo videoFileInfo(videoPath);
    QString Path_video_info_ini = "";
    do
    {
        qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
        int random = qrand()%1000;
        Path_video_info_ini = video_info_dir+"/"+file_getBaseName(videoFileInfo.filePath())+"_videoInfo_"+QString::number(random,10)+"_Waifu2xEX.ini";
    }
    while(file_isFileExist(Path_video_info_ini));
    //=========
    QFile video_info_ini(Path_video_info_ini);
    video_info_ini.remove();
    if (video_info_ini.open(QIODevice::ReadWrite | QIODevice::Text)) //QIODevice::ReadWrite支持读写
    {
        QTextStream stream(&video_info_ini);
        stream << ffprobe_output_str;
    }
    video_info_ini.close();
    //================== 读取ini获得参数 =====================
    QSettings *configIniRead_videoInfo = new QSettings(Path_video_info_ini, QSettings::IniFormat);
    QString BitRate = configIniRead_videoInfo->value("/format/bit_rate").toString().trimmed();
    //=======================
    if(BitRate=="")emit Send_TextBrowser_NewMessage(tr("Warning! Unable to get the bitrate of the [")+videoPath+tr("]. The bit rate automatically allocated by ffmpeg will be used."));
    video_info_ini.remove();
    return BitRate;
}

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
    emit Send_TextBrowser_NewMessage(tr("Start splitting video: [")+VideoPath+"]");
    //=================
    QString ffmpeg_path = Current_Path+"/ffmpeg_waifu2xEX.exe";
    QFileInfo vfinfo(VideoPath);
    QString video_dir = file_getFolderPath(vfinfo);
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
        QFile::remove(video_mp4_fullpath);
        QString vcodec_copy_cmd = "";
        QString acodec_copy_cmd = "";
        QString bitrate_vid_cmd = "";
        QString bitrate_audio_cmd = "";
        QString Extra_command = "";
        QString bitrate_OverAll = "";
        if(ui->checkBox_videoSettings_isEnabled->checkState())
        {
            Extra_command = ui->lineEdit_ExCommand_2mp4->text().trimmed();
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
        else
        {
            QString BitRate = video_get_bitrate(VideoPath);
            if(BitRate!="")bitrate_OverAll = " -b "+BitRate+" ";
        }
        //=====
        QProcess video_tomp4;
        video_tomp4.start("\""+ffmpeg_path+"\" -y -i \""+VideoPath+"\" "+vcodec_copy_cmd+acodec_copy_cmd+bitrate_vid_cmd+bitrate_audio_cmd+bitrate_OverAll+" "+Extra_command+" \""+video_mp4_fullpath+"\"");
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
    //====================================
    emit Send_TextBrowser_NewMessage(tr("Finish splitting video: [")+VideoPath+"]");
}

int MainWindow::video_images2video(QString VideoPath,QString video_mp4_scaled_fullpath,QString ScaledFrameFolderPath,QString AudioPath,bool CustRes_isEnabled,int CustRes_height,int CustRes_width)
{
    emit Send_TextBrowser_NewMessage(tr("Start assembling video:[")+VideoPath+"]");
    bool Del_DenoisedAudio = false;
    //=================
    QString encoder_video_cmd="";
    QString bitrate_video_cmd="";
    //====
    QString encoder_audio_cmd="";
    QString bitrate_audio_cmd="";
    //===
    QString pixFormat_cmd=" -pix_fmt yuv420p ";
    //=======
    QString Extra_Command_cmd="";
    //=======
    if(ui->checkBox_videoSettings_isEnabled->checkState())
    {
        encoder_video_cmd=" -c:v "+ui->lineEdit_encoder_vid->text()+" ";
        bitrate_video_cmd=" -b:v "+QString::number(ui->spinBox_bitrate_vid->value(),10)+"k ";
        encoder_audio_cmd=" -c:a "+ui->lineEdit_encoder_audio->text()+" ";
        bitrate_audio_cmd=" -b:a "+QString::number(ui->spinBox_bitrate_audio->value(),10)+"k ";
        pixFormat_cmd=" -pix_fmt "+ui->lineEdit_pixformat->text()+" ";
        Extra_Command_cmd = ui->lineEdit_ExCommand_output->text().trimmed();
    }
    else
    {
        QString BitRate = video_get_bitrate_AccordingToRes(ScaledFrameFolderPath);
        if(BitRate!="")bitrate_video_cmd=" -b:v "+BitRate+"k ";
    }
    //================ 自定义分辨率 ======================
    QString resize_cmd ="";
    if(CustRes_isEnabled)
    {
        //============= 如果没有自定义视频参数, 则根据自定义分辨率再计算一次比特率 ==========
        if(ui->checkBox_videoSettings_isEnabled->checkState()==false)
        {
            int small_res =0;
            if(CustRes_width<=CustRes_height)
            {
                small_res = CustRes_width;
            }
            else
            {
                small_res = CustRes_height;
            }
            bitrate_video_cmd=" -b:v "+QString::number(small_res*6,10)+"k ";
        }
        //=================================================================
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
    //=========== 获取fps ===========
    QString ffmpeg_path = Current_Path+"/ffmpeg_waifu2xEX.exe";
    int FrameNumDigits = video_get_frameNumDigits(VideoPath);
    QFileInfo vfinfo(VideoPath);
    QString video_dir = file_getFolderPath(vfinfo);
    QString video_filename = file_getBaseName(vfinfo.filePath());
    QString video_ext = vfinfo.suffix();
    int fps = video_get_fps(VideoPath);
    if(fps<=0)
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+VideoPath+tr("]. Error: [Unable to get video frame rate.]"));
        return 0;
    }
    //=============== 音频降噪 ========================
    if((ui->checkBox_AudioDenoise->checkState())&&file_isFileExist(AudioPath))
    {
        QString AudioPath_tmp = video_AudioDenoise(AudioPath);
        if(AudioPath_tmp!=AudioPath)
        {
            AudioPath = AudioPath_tmp;
            Del_DenoisedAudio = true;
        }
    }
    //================= 开始处理 =============================
    QString CMD = "";
    if(file_isFileExist(AudioPath))
    {
        CMD = "\""+ffmpeg_path+"\" -y -f image2 -framerate "+QString::number(fps,10)+" -i \""+ScaledFrameFolderPath+"/%0"+QString::number(FrameNumDigits,10)+"d.png\" -i \""+AudioPath+"\" -r "+QString::number(fps,10)+encoder_video_cmd+bitrate_video_cmd+encoder_audio_cmd+bitrate_audio_cmd+pixFormat_cmd+resize_cmd+" "+Extra_Command_cmd+" \""+video_mp4_scaled_fullpath+"\"";
    }
    else
    {
        CMD = "\""+ffmpeg_path+"\" -y -f image2 -framerate "+QString::number(fps,10)+" -i \""+ScaledFrameFolderPath+"/%0"+QString::number(FrameNumDigits,10)+"d.png\" -r "+QString::number(fps,10)+encoder_video_cmd+bitrate_video_cmd+pixFormat_cmd+resize_cmd+" "+Extra_Command_cmd+" \""+video_mp4_scaled_fullpath+"\"";
    }
    QProcess images2video;
    images2video.start(CMD);
    while(!images2video.waitForStarted(100)&&!QProcess_stop) {}
    while(!images2video.waitForFinished(100)&&!QProcess_stop) {}
    //============== 尝试在Win7下可能兼容的指令 ================================
    if(!file_isFileExist(video_mp4_scaled_fullpath))
    {
        if(file_isFileExist(AudioPath))
        {
            CMD = "\""+ffmpeg_path+"\" -y -f image2 -framerate "+QString::number(fps,10)+" -i \""+ScaledFrameFolderPath+"/%%00d.png\" -i \""+AudioPath+"\" -r "+QString::number(fps,10)+encoder_video_cmd+bitrate_video_cmd+encoder_audio_cmd+bitrate_audio_cmd+pixFormat_cmd+resize_cmd+" "+Extra_Command_cmd+" \""+video_mp4_scaled_fullpath+"\"";
        }
        else
        {
            CMD = "\""+ffmpeg_path+"\" -y -f image2 -framerate "+QString::number(fps,10)+" -i \""+ScaledFrameFolderPath+"/%%00d.png\" -r "+QString::number(fps,10)+encoder_video_cmd+bitrate_video_cmd+pixFormat_cmd+resize_cmd+" "+Extra_Command_cmd+" \""+video_mp4_scaled_fullpath+"\"";
        }
        QProcess images2video;
        images2video.start(CMD);
        while(!images2video.waitForStarted(100)&&!QProcess_stop) {}
        while(!images2video.waitForFinished(100)&&!QProcess_stop) {}
    }
    //===================
    if(Del_DenoisedAudio)QFile::remove(AudioPath);
    //==============================
    emit Send_TextBrowser_NewMessage(tr("Finish assembling video:[")+VideoPath+"]");
    return 0;
}
