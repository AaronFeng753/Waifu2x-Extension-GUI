/*
    Copyright (C) 2021  Aaron Feng

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
根据行数从自定义分辨率列表移除视频文件
*/
void MainWindow::video_RemoveFromCustResList(int RowNumber)
{
    QString SourceFile_fullPath = Table_model_video->item(RowNumber,2)->text();
    CustRes_remove(SourceFile_fullPath);
}

/*
当视频没有自定义分辨率且此时放大倍率为double,则计算一个添加到自定义列表里
*/
bool MainWindow::video_DoubleScaleRatioPrep(int RowNumber)
{
    QString SourceFile_fullPath = Table_model_video->item(RowNumber,2)->text();
    if(CustRes_isContained(SourceFile_fullPath) == true || ui->checkBox_FrameInterpolationOnly_Video->isChecked())
    {
        //如果已经被定义自定义分辨率,或者仅进行插帧
        return false;
    }
    //===================== 获取分辨率 =============================
    QMap<QString,int> Map_OrgRes = video_get_Resolution(SourceFile_fullPath);
    //========= 计算新的高度宽度 ==================
    double ScaleRatio_double = ui->doubleSpinBox_ScaleRatio_video->value();
    int Height_new = qRound(ScaleRatio_double * Map_OrgRes["height"]);
    int width_new = qRound(ScaleRatio_double * Map_OrgRes["width"]);
    if(Height_new<1 || width_new<1)
    {
        emit Send_TextBrowser_NewMessage("Warning! Unable to read the resolution of ["+SourceFile_fullPath+"]. This file will only be scaled to "+QString::number((int)ScaleRatio_double,10)+"X.");
        return false;
    }
    if(Height_new%2!=0)Height_new++;
    if(width_new%2!=0)width_new++;
    //======== 存入自定义分辨率列表中 ============
    QMap<QString,QString> res_map;
    res_map["fullpath"] = SourceFile_fullPath;
    res_map["height"] = QString::number(Height_new,10);
    res_map["width"] = QString::number(width_new,10);
    Custom_resolution_list.append(res_map);
    //=========
    return true;
}
/*
计算数字的位数
*/
int MainWindow::CalNumDigits(int input_num)
{
    int frameNumDigits=1+(int)log10(input_num);
    return frameNumDigits;
}

/*
判断视频是否是可变帧率的
*/
bool MainWindow::video_isVFR(QString videoPath)
{
    //========================= 调用ffprobe读取视频信息 ======================
    QProcess *Get_VideoFPS_process = new QProcess();
    QString cmd = "\""+Current_Path+"/ffprobe_waifu2xEX.exe\" -i \""+videoPath+"\" -select_streams v -show_streams -v quiet -print_format ini -show_format";
    Get_VideoFPS_process->start(cmd);
    while(!Get_VideoFPS_process->waitForStarted(100)&&!QProcess_stop) {}
    while(!Get_VideoFPS_process->waitForFinished(100)&&!QProcess_stop) {}
    //============= 保存ffprobe输出的ini格式文本 =============
    QString ffprobe_output_str = Get_VideoFPS_process->readAllStandardOutput();
    //================ 将ini写入文件保存 ================
    QFileInfo videoFileInfo(videoPath);
    QString Path_video_info_ini = "";
    QString video_dir = file_getFolderPath(videoPath);
    int FileNo = 0;
    do
    {
        FileNo++;
        Path_video_info_ini = video_dir+"/"+file_getBaseName(videoPath)+"_videoInfo_"+QString::number(FileNo,10)+"_Waifu2xEX.ini";
    }
    while(QFile::exists(Path_video_info_ini));
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
    QString avg_frame_rate = "";
    QString r_frame_rate = "";
    QSettings *configIniRead_videoInfo = new QSettings(Path_video_info_ini, QSettings::IniFormat);
    if(configIniRead_videoInfo->value("/streams.stream.0/avg_frame_rate") != QVariant() && configIniRead_videoInfo->value("/streams.stream.0/r_frame_rate") != QVariant())
    {
        avg_frame_rate = configIniRead_videoInfo->value("/streams.stream.0/avg_frame_rate").toString().trimmed();
        r_frame_rate = configIniRead_videoInfo->value("/streams.stream.0/r_frame_rate").toString().trimmed();
    }
    video_info_ini.remove();
    //=====
    if(avg_frame_rate!=""&&r_frame_rate!="")
    {
        return (avg_frame_rate!=r_frame_rate);
    }
    else
    {
        return true;
    }
}
/*
根据分辨率判断是否跳过
true = 跳过
*/
bool MainWindow::Video_AutoSkip_CustRes(int rowNum)
{
    if(ui->checkBox_AutoSkip_CustomRes->isChecked()==false || ui->checkBox_FrameInterpolationOnly_Video->isChecked()==true)return false;
    QString SourceFile_fullPath = Table_model_video->item(rowNum,2)->text();
    if(CustRes_isContained(SourceFile_fullPath))
    {
        QMap<QString, QString> Res_map = CustRes_getResMap(SourceFile_fullPath);//res_map["fullpath"],["height"],["width"]
        int CustRes_height=Res_map["height"].toInt();
        int CustRes_width=Res_map["width"].toInt();
        //=========================
        QMap<QString,int> res_map = video_get_Resolution(SourceFile_fullPath);
        int original_height = res_map["height"];
        int original_width = res_map["width"];
        if(original_height<=0||original_width<=0)//判断是否读取失败
        {
            return false;
        }
        //==========================
        if((CustRes_height*CustRes_width) <= (original_height*original_width))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

/*
直接获取视频的分辨率
*/
QMap<QString,int> MainWindow::video_get_Resolution(QString VideoFileFullPath)
{
    emit Send_TextBrowser_NewMessage(tr("Get resolution of the video:[")+VideoFileFullPath+"]");
    //========================= 调用ffprobe读取视频信息 ======================
    QProcess *Get_resolution_process = new QProcess();
    QString cmd = "\""+Current_Path+"/ffprobe_waifu2xEX.exe\" -i \""+VideoFileFullPath+"\" -select_streams v -show_streams -v quiet -print_format ini -show_format";
    Get_resolution_process->start(cmd);
    while(!Get_resolution_process->waitForStarted(100)&&!QProcess_stop) {}
    while(!Get_resolution_process->waitForFinished(100)&&!QProcess_stop) {}
    //============= 保存ffprobe输出的ini格式文本 =============
    QString ffprobe_output_str = Get_resolution_process->readAllStandardOutput();
    //================ 将ini写入文件保存 ================
    QFileInfo videoFileInfo(VideoFileFullPath);
    QString Path_video_info_ini = "";
    QString video_dir = file_getFolderPath(VideoFileFullPath);
    int FileNo = 0;
    do
    {
        FileNo++;
        Path_video_info_ini = video_dir+"/"+file_getBaseName(VideoFileFullPath)+"_videoInfo_"+QString::number(FileNo,10)+"_Waifu2xEX.ini";
    }
    while(QFile::exists(Path_video_info_ini));
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
    QString width_str = "";
    QString height_str = "";
    if(configIniRead_videoInfo->value("/streams.stream.0/width") != QVariant() && configIniRead_videoInfo->value("/streams.stream.0/height") != QVariant())
    {
        width_str = configIniRead_videoInfo->value("/streams.stream.0/width").toString().trimmed();
        height_str = configIniRead_videoInfo->value("/streams.stream.0/height").toString().trimmed();
    }
    video_info_ini.remove();
    //=======================
    if(width_str!="" && height_str!="")
    {
        int width_int = width_str.toInt();
        int height_int = height_str.toInt();
        if(width_int>0 && height_int>0)
        {
            QMap<QString,int> res_map;
            res_map["height"] = height_int;
            res_map["width"] = width_int;
            return res_map;
        }
    }
    emit Send_TextBrowser_NewMessage(tr("ERROR! Unable to read the resolution of the video. [")+VideoFileFullPath+"]");
    QMap<QString,int> empty;
    empty["height"] = 0;
    empty["width"] = 0;
    return empty;
}

/*
根据视频时长,判断是否需要分段处理
*/
bool MainWindow::video_isNeedProcessBySegment(int rowNum)
{
    if(ui->checkBox_ProcessVideoBySegment->isChecked()==false)return false;//如果没启用分段处理,直接返回false
    QString VideoFile = Table_model_video->item(rowNum,2)->text();
    if(video_get_duration(VideoFile)>ui->spinBox_SegmentDuration->value())
    {
        return true;
    }
    else
    {
        emit Send_TextBrowser_NewMessage(tr("This video is too short, so segment processing is automatically disabled.[")+VideoFile+"]");
        return false;
    }
}

/*
生成视频片段文件夹编号
*/
QString MainWindow::video_getClipsFolderNo()
{
    QString current_date =QDateTime::currentDateTime().toString("yyMMddhhmmss");
    return current_date;
}
/*
组装视频(从mp4片段组装)
*/
void MainWindow::video_AssembleVideoClips(QString VideoClipsFolderPath,QString VideoClipsFolderName,QString video_mp4_scaled_fullpath,QString AudioPath)
{
    emit Send_TextBrowser_NewMessage(tr("Start assembling video with clips:[")+video_mp4_scaled_fullpath+"]");
    //=================
    QStringList VideoClips_Scan_list = file_getFileNames_in_Folder_nofilter(VideoClipsFolderPath);
    QStringList VideoClips_fileName_list;
    VideoClips_fileName_list.clear();
    QFileInfo vfinfo(video_mp4_scaled_fullpath);
    QString video_dir = file_getFolderPath(video_mp4_scaled_fullpath);
    /*
    生成视频片段文件完整路径QStringList
    */
    for (int VideoNameNo = 0; VideoNameNo<VideoClips_Scan_list.size(); VideoNameNo++)
    {
        QString VideoClip_FullPath_tmp = VideoClipsFolderPath+"/"+QString::number(VideoNameNo,10)+".mp4";
        if(QFile::exists(VideoClip_FullPath_tmp))
        {
            VideoClips_fileName_list.append(VideoClip_FullPath_tmp);
        }
    }
    //获取一个有效的mp4片段文件完整路径
    QString Mp4Clip_forReadInfo = VideoClips_fileName_list.at(0);
    /*
    生成文件列表QString
    */
    QString FFMpegFileList_QString = "";
    for(int CurrentVideoClipNo = 0; CurrentVideoClipNo < VideoClips_fileName_list.size(); CurrentVideoClipNo++)
    {
        QString VideoClip_fullPath = VideoClips_fileName_list.at(CurrentVideoClipNo);
        QString Line = "file \'"+VideoClip_fullPath+"\'\n";
        FFMpegFileList_QString.append(Line);
    }
    //================ 将文件列表写入文件保存 ================
    QFileInfo videoFileInfo(video_mp4_scaled_fullpath);
    QString Path_FFMpegFileList = "";
    int FileNo = 0;
    do
    {
        FileNo++;
        Path_FFMpegFileList = video_dir+"/"+file_getBaseName(video_mp4_scaled_fullpath)+"_fileList_"+QString::number(FileNo,10)+"_Waifu2xEX.txt";
    }
    while(QFile::exists(Path_FFMpegFileList));
    //=========
    QFile FFMpegFileList(Path_FFMpegFileList);
    FFMpegFileList.remove();
    if (FFMpegFileList.open(QIODevice::ReadWrite | QIODevice::Text)) //QIODevice::ReadWrite支持读写
    {
        QTextStream stream(&FFMpegFileList);
        stream << FFMpegFileList_QString;
    }
    FFMpegFileList.close();
    //========
    /*
    组装视频
    */
    QString ffmpeg_path = Current_Path+"/ffmpeg_waifu2xEX.exe";
    bool Del_DenoisedAudio = false;
    //=============== 音频降噪 ========================
    if((ui->checkBox_AudioDenoise->isChecked())&&QFile::exists(AudioPath))
    {
        QString AudioPath_tmp = video_AudioDenoise(AudioPath);
        if(AudioPath_tmp!=AudioPath)
        {
            AudioPath = AudioPath_tmp;
            Del_DenoisedAudio = true;
        }
    }
    //================= 获取比特率 =================
    QString bitrate_video_cmd="";
    if(ui->spinBox_bitrate_vid->value()>0 && ui->groupBox_video_settings->isChecked())
    {
        bitrate_video_cmd = " -b:v "+QString::number(ui->spinBox_bitrate_vid->value(),10)+"k ";
    }
    else
    {
        QString BitRate_str = video_get_bitrate(Mp4Clip_forReadInfo,false,true);
        if(BitRate_str!="")
        {
            bitrate_video_cmd = " -b:v "+BitRate_str+" ";
        }
        else
        {
            int BitRate = video_UseRes2CalculateBitrate(Mp4Clip_forReadInfo);
            if(BitRate!=0)bitrate_video_cmd = " -b:v "+QString::number(BitRate,10)+"k ";
        }
    }
    //================= 读取视频编码器设定 ==============
    QString encoder_video_cmd="";
    if(ui->groupBox_video_settings->isChecked() && ui->lineEdit_encoder_vid->text().trimmed()!="")
    {
        encoder_video_cmd = " -c:v "+ui->lineEdit_encoder_vid->text().trimmed()+" ";//图像编码器
    }
    //=============== 读取音频编码设定 ====================
    QString encoder_audio_cmd="";
    QString bitrate_audio_cmd="";
    if(ui->groupBox_video_settings->isChecked())
    {
        if(ui->lineEdit_encoder_audio->text().trimmed()!="")
            encoder_audio_cmd=" -c:a "+ui->lineEdit_encoder_audio->text().trimmed()+" ";
        if(ui->spinBox_bitrate_audio->value()>0)
            bitrate_audio_cmd=" -b:a "+QString::number(ui->spinBox_bitrate_audio->value(),10)+"k ";
    }
    //==================
    QString Extra_command = "";
    if(ui->groupBox_video_settings->isChecked() && ui->lineEdit_ExCommand_output->text().trimmed()!="")
    {
        Extra_command = " "+ui->lineEdit_ExCommand_output->text().trimmed()+" ";//附加指令
    }
    //================ 获取fps =====================
    QString fps_video_cmd=" ";
    QString fps = video_get_fps(Mp4Clip_forReadInfo).trimmed();
    if(fps != "0.0")
    {
        fps_video_cmd = " -r "+fps+" ";
    }
    //================= 开始处理 =============================
    QString CMD = "";
    if(QFile::exists(AudioPath))
    {
        CMD = "\""+ffmpeg_path+"\" -y -f concat -safe 0 "+fps_video_cmd+" -i \""+Path_FFMpegFileList+"\" "+fps_video_cmd+" -i \""+AudioPath+"\" "+fps_video_cmd+" "+bitrate_video_cmd+encoder_video_cmd+fps_video_cmd+encoder_audio_cmd+bitrate_audio_cmd+Extra_command+"\""+video_mp4_scaled_fullpath+"\"";
    }
    else
    {
        CMD = "\""+ffmpeg_path+"\" -y -f concat -safe 0 "+fps_video_cmd+" -i \""+Path_FFMpegFileList+"\""+bitrate_video_cmd+encoder_video_cmd+fps_video_cmd+Extra_command+"\""+video_mp4_scaled_fullpath+"\"";
    }
    QProcess AssembleVideo;
    AssembleVideo.start(CMD);
    while(!AssembleVideo.waitForStarted(100)&&!QProcess_stop) {}
    while(!AssembleVideo.waitForFinished(100)&&!QProcess_stop)
    {
        if(waifu2x_STOP)
        {
            AssembleVideo.close();
            QFile::remove(video_mp4_scaled_fullpath);
            QFile::remove(Path_FFMpegFileList);
            return;
        }
    }
    //检查是否发生错误
    if(QFile::exists(video_mp4_scaled_fullpath)==false)//检查是否成功生成视频
    {
        MultiLine_ErrorOutput_QMutex.lock();
        emit Send_TextBrowser_NewMessage(tr("Error output for FFmpeg when processing:[")+video_mp4_scaled_fullpath+"]");
        emit Send_TextBrowser_NewMessage("\n--------------------------------------");
        //标准输出
        emit Send_TextBrowser_NewMessage(AssembleVideo.readAllStandardOutput());
        //错误输出
        emit Send_TextBrowser_NewMessage(AssembleVideo.readAllStandardError());
        emit Send_TextBrowser_NewMessage("\n--------------------------------------");
        MultiLine_ErrorOutput_QMutex.unlock();
    }
    QFile::remove(Path_FFMpegFileList);//删除文件列表
    //===================
    if(Del_DenoisedAudio)QFile::remove(AudioPath);
    //==============================
    emit Send_TextBrowser_NewMessage(tr("Finish assembling video with clips:[")+video_mp4_scaled_fullpath+"]");
}
/*
将视频拆分到帧(分段的)
*/
void MainWindow::video_video2images_ProcessBySegment(QString VideoPath,QString FrameFolderPath,int StartTime,int SegmentDuration)
{
    emit Send_TextBrowser_NewMessage(tr("Start splitting video: [")+VideoPath+"]");
    //=================
    QString ffmpeg_path = Current_Path+"/ffmpeg_waifu2xEX.exe";
    QString video_mp4_fullpath = VideoPath;
    //================ 获取fps =====================
    QString fps_video_cmd=" ";
    QString fps = video_get_fps(video_mp4_fullpath).trimmed();
    if(fps != "0.0")
    {
        fps_video_cmd = " -r "+fps+" ";
    }
    //=====================
    int FrameNumDigits = video_get_frameNumDigits(video_mp4_fullpath);
    if(FrameNumDigits==0)return;
    QFile::remove(isPreVFIDone_MarkFilePath(VideoPath));
    //=====================
    QProcess video_splitFrame;
    video_splitFrame.start("\""+ffmpeg_path+"\" -y"+fps_video_cmd+"-i \""+video_mp4_fullpath+"\" -ss "+QString::number(StartTime,10)+" -t "+QString::number(SegmentDuration,10)+fps_video_cmd+" \""+FrameFolderPath.replace("%","%%")+"/%0"+QString::number(FrameNumDigits,10)+"d.png\"");
    while(!video_splitFrame.waitForStarted(100)&&!QProcess_stop) {}
    while(!video_splitFrame.waitForFinished(100)&&!QProcess_stop) {}
    //============== 尝试在Win7下可能兼容的指令 ================================
    if(file_isDirEmpty(FrameFolderPath))
    {
        video_splitFrame.start("\""+ffmpeg_path+"\" -y"+fps_video_cmd+"-i \""+video_mp4_fullpath+"\" -ss "+QString::number(StartTime,10)+" -t "+QString::number(SegmentDuration,10)+fps_video_cmd+" \""+FrameFolderPath.replace("%","%%")+"/%%0"+QString::number(FrameNumDigits,10)+"d.png\"");
        while(!video_splitFrame.waitForStarted(100)&&!QProcess_stop) {}
        while(!video_splitFrame.waitForFinished(100)&&!QProcess_stop) {}
    }
    //======== 插帧 =========
    QFileInfo vfinfo(VideoPath);
    QString video_dir = file_getFolderPath(vfinfo);
    QString video_filename = file_getBaseName(VideoPath);
    QString VFI_FolderPath_tmp = video_dir+"/"+video_filename+"_PreVFI_W2xEX";
    if(ui->checkBox_VfiAfterScale_VFI->isChecked()==false && ui->groupBox_FrameInterpolation->isChecked()==true && ui->checkBox_FrameInterpolationOnly_Video->isChecked()==false) //如果启用了插帧
    {
        //如果检测到完整的已经插帧的帧缓存
        if(file_isDirExist(VFI_FolderPath_tmp) && (file_getFileNames_in_Folder_nofilter(FrameFolderPath).size() * ui->spinBox_MultipleOfFPS_VFI->value() == file_getFileNames_in_Folder_nofilter(VFI_FolderPath_tmp).size()))
        {
            file_DelDir(FrameFolderPath);
            QDir VFI_FolderPath_tmp_qdir(VFI_FolderPath_tmp);
            VFI_FolderPath_tmp_qdir.rename(VFI_FolderPath_tmp,FrameFolderPath);
            return;
        }
        else
        {
            if(FrameInterpolation(FrameFolderPath,VFI_FolderPath_tmp)==true)
            {
                //如果插帧成功
                file_DelDir(FrameFolderPath);
                QDir VFI_FolderPath_tmp_qdir(VFI_FolderPath_tmp);
                VFI_FolderPath_tmp_qdir.rename(VFI_FolderPath_tmp,FrameFolderPath);
                file_generateMarkFile(isPreVFIDone_MarkFilePath(VideoPath),QString("%1").arg(ui->spinBox_MultipleOfFPS_VFI->value()));
                return;
            }
            else
            {
                file_DelDir(FrameFolderPath);
                file_mkDir(FrameFolderPath);
                file_DelDir(VFI_FolderPath_tmp);
                if(waifu2x_STOP==false)emit Send_TextBrowser_NewMessage(tr("Failed to interpolate frames of video:[")+VideoPath+"]");
                return;
            }
        }
    }
    //====================================
    emit Send_TextBrowser_NewMessage(tr("Finish splitting video: [")+VideoPath+"]");
}

/*
提取视频的音频
*/
void MainWindow::video_get_audio(QString VideoPath,QString AudioPath)
{
    emit Send_TextBrowser_NewMessage(tr("Extract audio from video: [")+VideoPath+"]");
    //==============================================
    QString ffmpeg_path = Current_Path+"/ffmpeg_waifu2xEX.exe";
    QFile::remove(AudioPath);
    QProcess video_splitSound;
    video_splitSound.start("\""+ffmpeg_path+"\" -y -i \""+VideoPath+"\" \""+AudioPath+"\"");
    while(!video_splitSound.waitForStarted(100)&&!QProcess_stop) {}
    while(!video_splitSound.waitForFinished(100)&&!QProcess_stop) {}
    if(QFile::exists(AudioPath))
    {
        emit Send_TextBrowser_NewMessage(tr("Successfully extracted audio from video: [")+VideoPath+"]");
    }
    else
    {
        emit Send_TextBrowser_NewMessage(tr("Failed to extract audio from video: [")+VideoPath+tr("] This video might be a silent video, so will continue to process this video."));
    }
}
/*
将视频转换为mp4
*/
QString MainWindow::video_To_CFRMp4(QString VideoPath)
{
    QFileInfo vfinfo(VideoPath);
    QString video_ext = vfinfo.suffix();
    bool isVFR = false;
    //==============
    if(video_ext.toLower()=="mp4")
    {
        if(video_isVFR(VideoPath)==false)
        {
            return VideoPath;
        }
        else
        {
            isVFR = true;
        }
    }
    QString video_dir = file_getFolderPath(vfinfo);
    QString video_filename = file_getBaseName(VideoPath);
    QString video_mp4_fullpath = video_dir+"/"+video_filename+"_"+video_ext+"_CfrMp4.mp4";
    if(QFile::exists(video_mp4_fullpath))return video_mp4_fullpath;
    QFile::remove(video_mp4_fullpath);
    //=================
    emit Send_TextBrowser_NewMessage(tr("Start converting video: [")+VideoPath+tr("] to CFR MP4."));
    QString ffmpeg_path = Current_Path+"/ffmpeg_waifu2xEX.exe";
    QString vcodec_copy_cmd = "";
    QString acodec_copy_cmd = "";
    QString bitrate_vid_cmd = "";
    QString bitrate_audio_cmd = "";
    QString Extra_command = "";
    QString bitrate_FromOG = "";
    QString vsync_1 = " -vsync 1 ";
    if(ui->groupBox_video_settings->isChecked())
    {
        if(ui->checkBox_IgnoreFrameRateMode->isChecked()==true)
        {
            vsync_1 = "";
            isVFR = false;
        }
        Extra_command = ui->lineEdit_ExCommand_2mp4->text().trimmed();
        if(ui->checkBox_vcodec_copy_2mp4->isChecked()&&isVFR==false)
        {
            vcodec_copy_cmd = " -vcodec copy ";
        }
        else
        {
            if(ui->spinBox_bitrate_vid_2mp4->value()>0&&ui->spinBox_bitrate_audio_2mp4->value()>0)bitrate_vid_cmd = " -b:v "+QString::number(ui->spinBox_bitrate_vid_2mp4->value(),10)+"k ";
        }
        if(ui->checkBox_acodec_copy_2mp4->isChecked()&&isVFR==false)
        {
            acodec_copy_cmd = " -acodec copy ";
        }
        else
        {
            if(ui->spinBox_bitrate_vid_2mp4->value()>0&&ui->spinBox_bitrate_audio_2mp4->value()>0)bitrate_audio_cmd = " -b:a "+QString::number(ui->spinBox_bitrate_audio_2mp4->value(),10)+"k ";
        }
    }
    if((ui->groupBox_video_settings->isChecked()==false)||(ui->spinBox_bitrate_vid_2mp4->value()<=0||ui->spinBox_bitrate_audio_2mp4->value()<=0))
    {
        QString BitRate = video_get_bitrate(VideoPath,true,false);
        if(BitRate!="")
        {
            bitrate_FromOG = BitRate;
        }
        else
        {
            emit Send_TextBrowser_NewMessage(tr("Warning! Unable to get the bitrate of the [")+VideoPath+tr("]. The bit rate automatically allocated by ffmpeg will be used."));
        }
    }
    //=====
    QProcess video_tomp4;
    video_tomp4.start("\""+ffmpeg_path+"\" -y -i \""+VideoPath+"\""+vsync_1+vcodec_copy_cmd+acodec_copy_cmd+bitrate_vid_cmd+bitrate_audio_cmd+bitrate_FromOG+" "+Extra_command+" \""+video_mp4_fullpath+"\"");
    while(!video_tomp4.waitForStarted(100)&&!QProcess_stop) {}
    while(!video_tomp4.waitForFinished(100)&&!QProcess_stop) {}
    //======
    if(QFile::exists(video_mp4_fullpath))
    {
        emit Send_TextBrowser_NewMessage(tr("Successfully converted video: [")+VideoPath+tr("] to CFR MP4."));
        return video_mp4_fullpath;
    }
    return "null";
}
//===============
//获取时长(秒)
//===============
int MainWindow::video_get_duration(QString videoPath)
{
    emit Send_TextBrowser_NewMessage(tr("Get duration of the video:[")+videoPath+"]");
    //========================= 调用ffprobe读取视频信息 ======================
    QProcess *Get_Duration_process = new QProcess();
    QString cmd = "\""+Current_Path+"/ffprobe_waifu2xEX.exe\" -i \""+videoPath+"\" -select_streams v -show_streams -v quiet -print_format ini -show_format";
    Get_Duration_process->start(cmd);
    while(!Get_Duration_process->waitForStarted(100)&&!QProcess_stop) {}
    while(!Get_Duration_process->waitForFinished(100)&&!QProcess_stop) {}
    //============= 保存ffprobe输出的ini格式文本 =============
    QString ffprobe_output_str = Get_Duration_process->readAllStandardOutput();
    //================ 将ini写入文件保存 ================
    QFileInfo videoFileInfo(videoPath);
    QString Path_video_info_ini = "";
    QString video_dir = file_getFolderPath(videoPath);
    int FileNo = 0;
    do
    {
        FileNo++;
        Path_video_info_ini = video_dir+"/"+file_getBaseName(videoPath)+"_videoInfo_"+QString::number(FileNo,10)+"_Waifu2xEX.ini";
    }
    while(QFile::exists(Path_video_info_ini));
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
    QString Duration = "";
    QSettings *configIniRead_videoInfo = new QSettings(Path_video_info_ini, QSettings::IniFormat);
    if(configIniRead_videoInfo->value("/format/duration") != QVariant())
    {
        Duration = configIniRead_videoInfo->value("/format/duration").toString().trimmed();
    }
    video_info_ini.remove();
    //=======================
    if(Duration=="")
    {
        emit Send_TextBrowser_NewMessage(tr("ERROR! Unable to get the duration of the [")+videoPath+tr("]."));
        return 0;
    }
    //=======================
    double Duration_double = Duration.toDouble();
    int Duration_int = (int)Duration_double;
    //=====================
    return Duration_int;
}
/*
音频降噪
*/
QString MainWindow::video_AudioDenoise(QString OriginalAudioPath)
{
    emit Send_TextBrowser_NewMessage(tr("Starting to denoise audio.[")+OriginalAudioPath+"]");
    //===========
    QFileInfo fileinfo(OriginalAudioPath);
    QString file_name = file_getBaseName(OriginalAudioPath);
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
    if(QFile::exists(DenoisedAudio))
    {
        emit Send_TextBrowser_NewMessage(tr("Successfully denoise audio.[")+OriginalAudioPath+"]");
        QFile::remove(DenoiseProfile);
        return DenoisedAudio;
    }
    else
    {
        emit Send_TextBrowser_NewMessage(tr("Error! Unable to denoise audio.[")+OriginalAudioPath+"]");
        return OriginalAudioPath;
    }
}
/*
保存进度
*/
void MainWindow::video_write_Progress_ProcessBySegment(QString VideoConfiguration_fullPath,int StartTime,bool isSplitComplete,bool isScaleComplete,int OLDSegmentDuration,int LastVideoClipNo)
{
    QSettings *configIniWrite = new QSettings(VideoConfiguration_fullPath, QSettings::IniFormat);
    configIniWrite->setIniCodec(QTextCodec::codecForName("UTF-8"));
    //==================== 存储进度 ==================================
    configIniWrite->setValue("/Progress/StartTime", StartTime);
    configIniWrite->setValue("/Progress/isSplitComplete", isSplitComplete);
    configIniWrite->setValue("/Progress/isScaleComplete", isScaleComplete);
    configIniWrite->setValue("/Progress/OLDSegmentDuration", OLDSegmentDuration);
    configIniWrite->setValue("/Progress/LastVideoClipNo", LastVideoClipNo);
}
/*
保存视频配置
*/
void MainWindow::video_write_VideoConfiguration(QString VideoConfiguration_fullPath,int ScaleRatio,int DenoiseLevel,bool CustRes_isEnabled,int CustRes_height,int CustRes_width,QString EngineName,bool isProcessBySegment,QString VideoClipsFolderPath,QString VideoClipsFolderName,bool isVideoFrameInterpolationEnabled,int MultipleOfFPS)
{
    QSettings *configIniWrite = new QSettings(VideoConfiguration_fullPath, QSettings::IniFormat);
    configIniWrite->setIniCodec(QTextCodec::codecForName("UTF-8"));
    //================= 添加警告 =========================
    configIniWrite->setValue("/Warning/EN", "Do not modify this file! It may cause the program to crash! If problems occur after the modification, delete this file and restart the program.");
    //==================== 存储视频信息 ==================================
    configIniWrite->setValue("/VideoConfiguration/ScaleRatio", ScaleRatio);
    configIniWrite->setValue("/VideoConfiguration/DenoiseLevel", DenoiseLevel);
    configIniWrite->setValue("/VideoConfiguration/CustRes_isEnabled", CustRes_isEnabled);
    configIniWrite->setValue("/VideoConfiguration/CustRes_height", CustRes_height);
    configIniWrite->setValue("/VideoConfiguration/CustRes_width", CustRes_width);
    configIniWrite->setValue("/VideoConfiguration/EngineName", EngineName);
    configIniWrite->setValue("/VideoConfiguration/isProcessBySegment", isProcessBySegment);
    configIniWrite->setValue("/VideoConfiguration/VideoClipsFolderPath", VideoClipsFolderPath);
    configIniWrite->setValue("/VideoConfiguration/VideoClipsFolderName", VideoClipsFolderName);
    configIniWrite->setValue("/VideoConfiguration/isVideoFrameInterpolationEnabled", isVideoFrameInterpolationEnabled);
    configIniWrite->setValue("/VideoConfiguration/MultipleOfFPS", MultipleOfFPS);
    //==================== 存储进度 ==================================
    configIniWrite->setValue("/Progress/StartTime", 0);
    configIniWrite->setValue("/Progress/isSplitComplete", false);
    configIniWrite->setValue("/Progress/isScaleComplete", false);
    configIniWrite->setValue("/Progress/OLDSegmentDuration", -1);
    configIniWrite->setValue("/Progress/LastVideoClipNo", -1);
}

/*
直接读取视频 分辨率 然后用 自有算法 计算其应该有的比特率
单位为k
*/
int MainWindow::video_UseRes2CalculateBitrate(QString VideoFileFullPath)
{
    QMap<QString,int> res_map = video_get_Resolution(VideoFileFullPath);
    int original_height = res_map["height"];
    int original_width = res_map["width"];
    if(original_height<=0||original_width<=0)
    {
        return 0;
    }
    //=========
    double MultipleOfBitrate_OGFR = 1;
    QString fps = video_get_fps(VideoFileFullPath).trimmed();
    if(fps != "0.0")
    {
        QStringList FPS_Nums = fps.split("/");
        double FPS_Nums_0 = FPS_Nums.at(0).toDouble();
        double FPS_Nums_1 = FPS_Nums.at(1).toDouble();
        if(FPS_Nums_0 >= FPS_Nums_1)
        {
            MultipleOfBitrate_OGFR = ((FPS_Nums_0/FPS_Nums_1)/24)*0.75;
            if(MultipleOfBitrate_OGFR<1)
            {
                MultipleOfBitrate_OGFR = 1;
            }
        }
    }
    //=========
    if(original_height<=original_width)
    {
        return qRound(original_height*6*MultipleOfBitrate_OGFR);
    }
    else
    {
        return qRound(original_width*6*MultipleOfBitrate_OGFR);
    }
}

QString MainWindow::video_get_bitrate_AccordingToRes_FrameFolder(QString ScaledFrameFolderPath,QString VideoPath)
{
    QStringList flist = file_getFileNames_in_Folder_nofilter(ScaledFrameFolderPath);
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
    //=========
    double MultipleOfBitrate_OGFR = 1;
    QString fps = video_get_fps(VideoPath).trimmed();
    if(fps != "0.0")
    {
        QStringList FPS_Nums = fps.split("/");
        double FPS_Nums_0 = FPS_Nums.at(0).toDouble();
        double FPS_Nums_1 = FPS_Nums.at(1).toDouble();
        if(FPS_Nums_0 >= FPS_Nums_1)
        {
            MultipleOfBitrate_OGFR = ((FPS_Nums_0/FPS_Nums_1)/24)*0.75;
            if(MultipleOfBitrate_OGFR<1)
            {
                MultipleOfBitrate_OGFR = 1;
            }
        }
    }
    //=========
    double MultipleOfBitrate_MOF = 1;
    if(ui->groupBox_FrameInterpolation->isChecked()==true)
    {
        MultipleOfBitrate_MOF = ui->spinBox_MultipleOfFPS_VFI->value()*0.75;
    }
    //=========
    if(original_height<=original_width)
    {
        return QString::number(qRound(original_height*6*MultipleOfBitrate_MOF*MultipleOfBitrate_OGFR),10);
    }
    else
    {
        return QString::number(qRound(original_width*6*MultipleOfBitrate_MOF*MultipleOfBitrate_OGFR),10);
    }
}
/*
获取视频比特率
*/
QString MainWindow::video_get_bitrate(QString videoPath,bool isReturnFullCMD,bool isVidOnly)
{
    emit Send_TextBrowser_NewMessage(tr("Get bitrate of the video:[")+videoPath+"]");
    //========================= 调用ffprobe读取视频信息 ======================
    QProcess *Get_Bitrate_process = new QProcess();
    QString cmd = "\""+Current_Path+"/ffprobe_waifu2xEX.exe\" -i \""+videoPath+"\" -select_streams v -show_streams -v quiet -print_format ini -show_format";
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
    QString video_dir = file_getFolderPath(videoPath);
    int FileNo = 0;
    do
    {
        FileNo++;
        Path_video_info_ini = video_dir+"/"+file_getBaseName(videoPath)+"_videoInfo_"+QString::number(FileNo,10)+"_Waifu2xEX.ini";
    }
    while(QFile::exists(Path_video_info_ini));
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
    QString BitRate = "";
    QString BitRateCMD = "";
    if(configIniRead_videoInfo->value("/streams.stream.0/bit_rate")!=QVariant())
    {
        BitRate = configIniRead_videoInfo->value("/streams.stream.0/bit_rate").toString().trimmed();
        if(isReturnFullCMD==true)BitRateCMD = " -b:v "+BitRate+" ";
    }
    if(BitRate.toUInt() == 0 && isVidOnly == false)
    {
        if(configIniRead_videoInfo->value("/format/bit_rate")!=QVariant())
        {
            BitRate = configIniRead_videoInfo->value("/format/bit_rate").toString().trimmed();
            if(isReturnFullCMD==true)BitRateCMD = " -b "+BitRate+" ";
        }
    }
    //=======================
    if(BitRate.toUInt() == 0)
    {
        BitRate="";
    }
    //=======================
    if(isReturnFullCMD==true && BitRate!="" && BitRateCMD!="")
    {
        video_info_ini.remove();
        return BitRateCMD;
    }
    else
    {
        video_info_ini.remove();
        return BitRate;
    }
}
/*
获取视频FPS
*/
QString MainWindow::video_get_fps(QString videoPath)
{
    //========================= 调用ffprobe读取视频信息 ======================
    QProcess *Get_VideoFPS_process = new QProcess();
    QString cmd = "\""+Current_Path+"/ffprobe_waifu2xEX.exe\" -i \""+videoPath+"\" -select_streams v -show_streams -v quiet -print_format ini -show_format";
    Get_VideoFPS_process->start(cmd);
    while(!Get_VideoFPS_process->waitForStarted(100)&&!QProcess_stop) {}
    while(!Get_VideoFPS_process->waitForFinished(100)&&!QProcess_stop) {}
    //============= 保存ffprobe输出的ini格式文本 =============
    QString ffprobe_output_str = Get_VideoFPS_process->readAllStandardOutput();
    //================ 将ini写入文件保存 ================
    QFileInfo videoFileInfo(videoPath);
    QString Path_video_info_ini = "";
    QString video_dir = file_getFolderPath(videoPath);
    int FileNo = 0;
    do
    {
        FileNo++;
        Path_video_info_ini = video_dir+"/"+file_getBaseName(videoPath)+"_videoInfo_"+QString::number(FileNo,10)+"_Waifu2xEX.ini";
    }
    while(QFile::exists(Path_video_info_ini));
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
    QString FPS_Division = "";
    QSettings *configIniRead_videoInfo = new QSettings(Path_video_info_ini, QSettings::IniFormat);
    if(configIniRead_videoInfo->value("/streams.stream.0/avg_frame_rate") != QVariant())
    {
        FPS_Division = configIniRead_videoInfo->value("/streams.stream.0/avg_frame_rate").toString().trimmed();
    }
    video_info_ini.remove();
    //=======================
    if(FPS_Division=="")
    {
        return "0.0";
    }
    //=======================
    QStringList FPS_Nums = FPS_Division.split("/");
    if(FPS_Nums.size()!=2)
    {
        return "0.0";
    }
    double FPS_Num_0 = FPS_Nums.at(0).toDouble();
    double FPS_Num_1 = FPS_Nums.at(1).toDouble();
    if(FPS_Num_0<=0||FPS_Num_1<=0)
    {
        return "0.0";
    }
    return FPS_Division;
}

int MainWindow::video_get_frameNum(QString videoPath)
{
    //========================= 调用ffprobe读取视频信息 ======================
    QProcess *Get_VideoFrameNumDigits_process = new QProcess();
    QString cmd = "\""+Current_Path+"/ffprobe_waifu2xEX.exe\" -i \""+videoPath+"\" -select_streams v -show_streams -v quiet -print_format ini -show_format";
    Get_VideoFrameNumDigits_process->start(cmd);
    while(!Get_VideoFrameNumDigits_process->waitForStarted(100)&&!QProcess_stop) {}
    while(!Get_VideoFrameNumDigits_process->waitForFinished(100)&&!QProcess_stop) {}
    //============= 保存ffprobe输出的ini格式文本 =============
    QString ffprobe_output_str = Get_VideoFrameNumDigits_process->readAllStandardOutput();
    //================ 将ini写入文件保存 ================
    QFileInfo videoFileInfo(videoPath);
    QString Path_video_info_ini = "";
    QString video_dir = file_getFolderPath(videoPath);
    int FileNo = 0;
    do
    {
        FileNo++;
        Path_video_info_ini = video_dir+"/"+file_getBaseName(videoPath)+"_videoInfo_"+QString::number(FileNo,10)+"_Waifu2xEX.ini";
    }
    while(QFile::exists(Path_video_info_ini));
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
    int FrameNum = 0;
    QSettings *configIniRead_videoInfo = new QSettings(Path_video_info_ini, QSettings::IniFormat);
    if(configIniRead_videoInfo->value("/streams.stream.0/nb_frames") != QVariant())
    {
        FrameNum = configIniRead_videoInfo->value("/streams.stream.0/nb_frames").toInt();
    }
    video_info_ini.remove();
    if(FrameNum<1)
    {
        emit Send_TextBrowser_NewMessage(tr("ERROR! Unable to read the number of frames of the video: [")+videoPath+"]");
        return 0;
    }
    return FrameNum;
}

int MainWindow::video_get_frameNumDigits(QString videoPath)
{
    int frameNumDigits=1+(int)log10(video_get_frameNum(videoPath));
    return frameNumDigits;
}

void MainWindow::video_video2images(QString VideoPath,QString FrameFolderPath,QString AudioPath)
{
    emit Send_TextBrowser_NewMessage(tr("Start splitting video: [")+VideoPath+"]");
    //=================
    QString ffmpeg_path = Current_Path+"/ffmpeg_waifu2xEX.exe";
    //================ 获取fps =====================
    QString fps_video_cmd=" ";
    QString fps = video_get_fps(VideoPath).trimmed();
    if(fps != "0.0")
    {
        fps_video_cmd = " -r "+fps+" ";
    }
    //=====================
    int FrameNumDigits = video_get_frameNumDigits(VideoPath);
    if(FrameNumDigits==0)return;
    QFile::remove(isPreVFIDone_MarkFilePath(VideoPath));
    //=====================
    QProcess video_splitFrame;
    video_splitFrame.start("\""+ffmpeg_path+"\" -y"+fps_video_cmd+"-i \""+VideoPath+"\" "+fps_video_cmd+" \""+FrameFolderPath.replace("%","%%")+"/%0"+QString::number(FrameNumDigits,10)+"d.png\"");
    while(!video_splitFrame.waitForStarted(100)&&!QProcess_stop) {}
    while(!video_splitFrame.waitForFinished(100)&&!QProcess_stop) {}
    //============== 尝试在Win7下可能兼容的指令 ================================
    if(file_isDirEmpty(FrameFolderPath))
    {
        video_splitFrame.start("\""+ffmpeg_path+"\" -y"+fps_video_cmd+"-i \""+VideoPath+"\" "+fps_video_cmd+" \""+FrameFolderPath.replace("%","%%")+"/%%0"+QString::number(FrameNumDigits,10)+"d.png\"");
        while(!video_splitFrame.waitForStarted(100)&&!QProcess_stop) {}
        while(!video_splitFrame.waitForFinished(100)&&!QProcess_stop) {}
    }
    video_get_audio(VideoPath,AudioPath);//拆分音频
    //======== 插帧 =========
    QFileInfo vfinfo(VideoPath);
    QString video_dir = file_getFolderPath(vfinfo);
    QString video_filename = file_getBaseName(VideoPath);
    QString VFI_FolderPath_tmp = video_dir+"/"+video_filename+"_PreVFI_W2xEX";
    if(ui->checkBox_VfiAfterScale_VFI->isChecked()==false && ui->groupBox_FrameInterpolation->isChecked()==true && ui->checkBox_FrameInterpolationOnly_Video->isChecked()==false) //如果启用了插帧
    {
        //如果检测到完整的已经插帧的帧缓存
        if(file_isDirExist(VFI_FolderPath_tmp) && (file_getFileNames_in_Folder_nofilter(FrameFolderPath).size() * ui->spinBox_MultipleOfFPS_VFI->value() == file_getFileNames_in_Folder_nofilter(VFI_FolderPath_tmp).size()))
        {
            file_DelDir(FrameFolderPath);
            QDir VFI_FolderPath_tmp_qdir(VFI_FolderPath_tmp);
            VFI_FolderPath_tmp_qdir.rename(VFI_FolderPath_tmp,FrameFolderPath);
            return;
        }
        else
        {
            if(FrameInterpolation(FrameFolderPath,VFI_FolderPath_tmp)==true)
            {
                //如果插帧成功
                file_DelDir(FrameFolderPath);
                QDir VFI_FolderPath_tmp_qdir(VFI_FolderPath_tmp);
                VFI_FolderPath_tmp_qdir.rename(VFI_FolderPath_tmp,FrameFolderPath);
                file_generateMarkFile(isPreVFIDone_MarkFilePath(VideoPath),QString("%1").arg(ui->spinBox_MultipleOfFPS_VFI->value()));
                return;
            }
            else
            {
                //如果插帧失败且启用了仅插帧模式
                if(ui->checkBox_FrameInterpolationOnly_Video->isChecked()==true)
                {
                    file_DelDir(FrameFolderPath);
                    file_mkDir(FrameFolderPath);
                    file_DelDir(VFI_FolderPath_tmp);
                    if(waifu2x_STOP==false)emit Send_TextBrowser_NewMessage(tr("Failed to interpolate frames of video:[")+VideoPath+"]");
                    return;
                }
                //如果插帧失败但是已经超分辨率
                else
                {
                    file_DelDir(VFI_FolderPath_tmp);
                    if(waifu2x_STOP==false)emit Send_TextBrowser_NewMessage(tr("Failed to interpolate frames of video:[")+VideoPath+tr("]. Gonna generate a video without frame Interpolation."));
                }
            }
        }
    }
    //====================================
    emit Send_TextBrowser_NewMessage(tr("Finish splitting video: [")+VideoPath+"]");
}

int MainWindow::video_images2video(QString VideoPath,QString video_mp4_scaled_fullpath,QString ScaledFrameFolderPath,QString AudioPath,bool CustRes_isEnabled,int CustRes_height,int CustRes_width,bool isOverScaled)
{
    bool Del_DenoisedAudio = false;
    //=================
    QString bitrate_video_cmd="";
    //=======
    if(ui->groupBox_video_settings->isChecked()&&(ui->spinBox_bitrate_vid->value()>0))
    {
        bitrate_video_cmd=" -b:v "+QString::number(ui->spinBox_bitrate_vid->value(),10)+"k ";
    }
    else
    {
        QString BitRate = video_get_bitrate_AccordingToRes_FrameFolder(ScaledFrameFolderPath,VideoPath);
        if(BitRate!="")bitrate_video_cmd=" -b:v "+BitRate+"k ";
    }
    //================ 自定义分辨率 ======================
    QString resize_cmd ="";
    if(CustRes_isEnabled || isOverScaled)
    {
        if(isOverScaled==true && CustRes_isEnabled==false)
        {
            QMap<QString,int> res_map = video_get_Resolution(VideoPath);
            int scaleratio_orginal = ui->doubleSpinBox_ScaleRatio_video->value();
            resize_cmd =" -vf scale="+QString::number(res_map["width"]*scaleratio_orginal,10)+":"+QString::number(res_map["height"]*scaleratio_orginal,10)+" ";
        }
        //============= 如果没有自定义视频参数, 则根据自定义分辨率再计算一次比特率 ==========
        if(ui->groupBox_video_settings->isChecked()==false || (ui->spinBox_bitrate_vid->value()<1))
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
        if(CustRes_AspectRatioMode==Qt::IgnoreAspectRatio && CustRes_isEnabled==true)
        {
            resize_cmd =" -vf scale="+QString::number(CustRes_width,10)+":"+QString::number(CustRes_height,10)+" ";
        }
        if(CustRes_AspectRatioMode==Qt::KeepAspectRatio && CustRes_isEnabled==true)
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
        if(CustRes_AspectRatioMode==Qt::KeepAspectRatioByExpanding && CustRes_isEnabled==true)
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
    QString ffmpeg_path = Current_Path+"/ffmpeg_waifu2xEX.exe";
    int FrameNumDigits = video_get_frameNumDigits(VideoPath);
    if(FrameNumDigits==0)return 0;
    QFileInfo vfinfo(VideoPath);
    QString video_dir = file_getFolderPath(vfinfo);
    QString video_filename = file_getBaseName(VideoPath);
    QString video_ext = vfinfo.suffix();
    //=========== 获取fps ===========
    QString fps = video_get_fps(VideoPath).trimmed();
    if(fps == "0.0")
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+VideoPath+tr("]. Error: [Unable to get video frame rate.]"));
        return 0;
    }
    //=============== 补帧 ===============
    QString VFI_FolderPath_tmp = video_dir+"/"+video_filename+"_PostVFI_W2xEX";
    //如果启用了插帧
    if(ui->groupBox_FrameInterpolation->isChecked()==true)
    {
        bool isPreVFIDone = QFile::exists(isPreVFIDone_MarkFilePath(VideoPath));
        if((ui->checkBox_VfiAfterScale_VFI->isChecked()==false && ui->checkBox_FrameInterpolationOnly_Video->isChecked()==false) || isPreVFIDone==true)
        {
            if(isPreVFIDone==true)
            {
                FrameNumDigits = CalNumDigits((file_getFileNames_in_Folder_nofilter(ScaledFrameFolderPath).size()));
                //==========
                int Old_MultipleOfFPS = ui->spinBox_MultipleOfFPS_VFI->value();
                QFile f(isPreVFIDone_MarkFilePath(VideoPath));
                if(f.open(QIODevice::ReadOnly | QIODevice::Text))
                {
                    QTextStream txtInput(&f);
                    QString lineStr;
                    lineStr = txtInput.readAll();
                    int tmp_mof = lineStr.trimmed().toInt();
                    if(tmp_mof>1)
                    {
                        Old_MultipleOfFPS = tmp_mof;
                    }
                }
                //==========
                QStringList FPS_Nums = fps.split("/");
                fps = QString("%1/%2").arg(FPS_Nums.at(0).toDouble() * Old_MultipleOfFPS).arg(FPS_Nums.at(1).toDouble());
            }
        }
        else
        {
            //如果检测到完整的已经插帧的帧缓存
            if(file_isDirExist(VFI_FolderPath_tmp) && (file_getFileNames_in_Folder_nofilter(ScaledFrameFolderPath).size() * ui->spinBox_MultipleOfFPS_VFI->value() == file_getFileNames_in_Folder_nofilter(VFI_FolderPath_tmp).size()))
            {
                FrameNumDigits = CalNumDigits((file_getFileNames_in_Folder_nofilter(VFI_FolderPath_tmp).size()));
                ScaledFrameFolderPath = VFI_FolderPath_tmp;
                QStringList FPS_Nums = fps.split("/");
                fps = QString("%1/%2").arg(FPS_Nums.at(0).toDouble() * ui->spinBox_MultipleOfFPS_VFI->value()).arg(FPS_Nums.at(1).toDouble());
            }
            else
            {
                //如果插帧成功
                if(FrameInterpolation(ScaledFrameFolderPath,VFI_FolderPath_tmp)==true)
                {
                    FrameNumDigits = CalNumDigits((file_getFileNames_in_Folder_nofilter(VFI_FolderPath_tmp).size()));
                    ScaledFrameFolderPath = VFI_FolderPath_tmp;
                    QStringList FPS_Nums = fps.split("/");
                    fps = QString("%1/%2").arg(FPS_Nums.at(0).toDouble() * ui->spinBox_MultipleOfFPS_VFI->value()).arg(FPS_Nums.at(1).toDouble());
                }
                else
                {
                    //如果插帧失败且启用了分段处理或仅插帧模式
                    if(ui->checkBox_ProcessVideoBySegment->isChecked()==true || ui->checkBox_FrameInterpolationOnly_Video->isChecked()==true)
                    {
                        file_DelDir(VFI_FolderPath_tmp);
                        if(waifu2x_STOP==false)emit Send_TextBrowser_NewMessage(tr("Failed to interpolate frames of video:[")+VideoPath+"]");
                        return 0;
                    }
                    //如果插帧失败但是已经超分辨率且没分段
                    else
                    {
                        file_DelDir(VFI_FolderPath_tmp);
                        if(waifu2x_STOP==false)emit Send_TextBrowser_NewMessage(tr("Failed to interpolate frames of video:[")+VideoPath+tr("]. Gonna generate a video without frame Interpolation."));
                    }
                }
            }
        }
    }
    //=============== 音频降噪 ========================
    if((ui->checkBox_AudioDenoise->isChecked())&&QFile::exists(AudioPath))
    {
        QString AudioPath_tmp = video_AudioDenoise(AudioPath);
        if(AudioPath_tmp!=AudioPath)
        {
            AudioPath = AudioPath_tmp;
            Del_DenoisedAudio = true;
        }
    }
    //================= 开始处理 =============================
    emit Send_TextBrowser_NewMessage(tr("Start assembling video:[")+VideoPath+"]");
    //======
    QString CMD = "";
    if(QFile::exists(AudioPath))
    {
        CMD = "\""+ffmpeg_path+"\" -y -f image2 -framerate "+fps+" -r "+fps+" -i \""+ScaledFrameFolderPath.replace("%","%%")+"/%0"+QString::number(FrameNumDigits,10)+"d.png\" -i \""+AudioPath+"\" -r "+fps+bitrate_video_cmd+resize_cmd+video_ReadSettings_OutputVid(AudioPath)+" -r "+fps+" \""+video_mp4_scaled_fullpath+"\"";
    }
    else
    {
        CMD = "\""+ffmpeg_path+"\" -y -f image2 -framerate "+fps+" -r "+fps+" -i \""+ScaledFrameFolderPath.replace("%","%%")+"/%0"+QString::number(FrameNumDigits,10)+"d.png\" -r "+fps+bitrate_video_cmd+resize_cmd+video_ReadSettings_OutputVid(AudioPath)+" -r "+fps+" \""+video_mp4_scaled_fullpath+"\"";
    }
    QProcess images2video;
    QFile::remove(video_mp4_scaled_fullpath);//删除旧文件
    images2video.start(CMD);
    while(!images2video.waitForStarted(100)&&!QProcess_stop) {}
    while(!images2video.waitForFinished(100)&&!QProcess_stop)
    {
        if(waifu2x_STOP)
        {
            images2video.close();
            QFile::remove(video_mp4_scaled_fullpath);
            file_DelDir(VFI_FolderPath_tmp);
            if(Del_DenoisedAudio)QFile::remove(AudioPath);
            return 0;
        }
    }
    //============== 尝试在Win7下可能兼容的指令 ================================
    if(QFile::exists(video_mp4_scaled_fullpath)==false)
    {
        if(QFile::exists(AudioPath))
        {
            CMD = "\""+ffmpeg_path+"\" -y -f image2 -framerate "+fps+" -r "+fps+" -i \""+ScaledFrameFolderPath.replace("%","%%")+"/%%0"+QString::number(FrameNumDigits,10)+"d.png\" -i \""+AudioPath+"\" -r "+fps+bitrate_video_cmd+resize_cmd+video_ReadSettings_OutputVid(AudioPath)+" -r "+fps+" \""+video_mp4_scaled_fullpath+"\"";
        }
        else
        {
            CMD = "\""+ffmpeg_path+"\" -y -f image2 -framerate "+fps+" -r "+fps+" -i \""+ScaledFrameFolderPath.replace("%","%%")+"/%%0"+QString::number(FrameNumDigits,10)+"d.png\" -r "+fps+bitrate_video_cmd+resize_cmd+video_ReadSettings_OutputVid(AudioPath)+" -r "+fps+" \""+video_mp4_scaled_fullpath+"\"";
        }
        QProcess images2video;
        images2video.start(CMD);
        while(!images2video.waitForStarted(100)&&!QProcess_stop) {}
        while(!images2video.waitForFinished(100)&&!QProcess_stop)
        {
            if(waifu2x_STOP)
            {
                images2video.close();
                QFile::remove(video_mp4_scaled_fullpath);
                file_DelDir(VFI_FolderPath_tmp);
                if(Del_DenoisedAudio)QFile::remove(AudioPath);
                return 0;
            }
        }
    }
    //===================
    if(Del_DenoisedAudio)QFile::remove(AudioPath);
    if(ui->checkBox_KeepVideoCache->isChecked() == false || ui->checkBox_ProcessVideoBySegment->isChecked())file_DelDir(VFI_FolderPath_tmp);
    if(ui->checkBox_KeepVideoCache->isChecked() == false)QFile::remove(isPreVFIDone_MarkFilePath(VideoPath));
    //==============================
    emit Send_TextBrowser_NewMessage(tr("Finish assembling video:[")+VideoPath+"]");
    return 0;
}

QString MainWindow::video_ReadSettings_OutputVid(QString AudioPath)
{
    QString OutputVideoSettings= " ";
    //====
    if(ui->groupBox_video_settings->isChecked())
    {
        if(ui->lineEdit_encoder_vid->text().trimmed()!="")
        {
            OutputVideoSettings.append("-c:v "+ui->lineEdit_encoder_vid->text().trimmed()+" ");//图像编码器
        }
        //========
        if(QFile::exists(AudioPath))
        {
            if(ui->lineEdit_encoder_audio->text().trimmed()!="")
            {
                OutputVideoSettings.append("-c:a "+ui->lineEdit_encoder_audio->text().trimmed()+" ");//音频编码器
            }
            //=========
            if(ui->spinBox_bitrate_audio->value()>0)
            {
                OutputVideoSettings.append("-b:a "+QString::number(ui->spinBox_bitrate_audio->value(),10)+"k ");//音频比特率
            }
        }
        //=========
        if(ui->lineEdit_pixformat->text().trimmed()!="")
        {
            OutputVideoSettings.append("-pix_fmt "+ui->lineEdit_pixformat->text().trimmed()+" ");//pixel format
        }
        else
        {
            OutputVideoSettings.append("-pix_fmt yuv420p ");//pixel format
        }
        //===========
        if(ui->lineEdit_ExCommand_output->text().trimmed()!="")
        {
            OutputVideoSettings.append(ui->lineEdit_ExCommand_output->text().trimmed()+" ");//附加指令
        }
    }
    //=========
    else
    {
        OutputVideoSettings.append("-pix_fmt yuv420p ");//pixel format
    }
    //=======
    return OutputVideoSettings;
}

void MainWindow::DelVfiDir(QString VideoPath)
{
    QFileInfo vfinfo(VideoPath);
    QString video_dir = file_getFolderPath(vfinfo);
    QString video_filename = file_getBaseName(VideoPath);
    file_DelDir(video_dir+"/"+video_filename+"_PreVFI_W2xEX");
    file_DelDir(video_dir+"/"+video_filename+"_PostVFI_W2xEX");
}
