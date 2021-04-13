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
对视频进行仅插帧(分段)
*/
int MainWindow::FrameInterpolation_Video_BySegment(int rowNum)
{
    //============================= 读取设置 ================================
    bool DelOriginal = (ui->checkBox_DelOriginal->isChecked()||ui->checkBox_ReplaceOriginalFile->isChecked());
    int SegmentDuration = ui->spinBox_SegmentDuration->value();
    //========================= 拆解map得到参数 =============================
    emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, "Processing");
    QString SourceFile_fullPath = Table_model_video->item(rowNum,2)->text();
    if(!QFile::exists(SourceFile_fullPath))
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [File does not exist.]"));
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed");
        emit Send_progressbar_Add();
        return 0;
    }
    //==========================
    QFileInfo SourceFile_fullPath_fileinfo(SourceFile_fullPath);
    QString SourceFile_BaseName = file_getBaseName(SourceFile_fullPath);
    QString SourceFile_Suffix = SourceFile_fullPath_fileinfo.suffix();
    QString SourceFile_FolderPath = file_getFolderPath(SourceFile_fullPath_fileinfo);
    //===================================================================
    //生成mp4
    QString video_mp4_fullpath=video_To_CFRMp4(SourceFile_fullPath);
    if(!QFile::exists(video_mp4_fullpath))//检查是否成功生成mp4
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Cannot convert video format to mp4.]"));
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed");
        emit Send_progressbar_Add();
        return 0;//如果启用stop位,则直接return
    }
    //=================
    QString AudioPath = SourceFile_FolderPath+"/Audio_"+SourceFile_BaseName+"_"+SourceFile_Suffix+"_W2xEX.wav";//音频
    QString SplitFramesFolderPath = SourceFile_FolderPath+"/"+SourceFile_BaseName+"_"+SourceFile_Suffix+"_SplitFrames_W2xEX";//拆分后存储frame的文件夹
    //=================
    QString VideoClipsFolderPath = "";//存储视频片段的文件夹(完整路径)
    QString DateStr = "";
    do
    {
        DateStr = video_getClipsFolderNo();
        VideoClipsFolderPath = SourceFile_FolderPath+"/"+DateStr+"_VideoClipsWaifu2xEX";//存储视频片段的文件夹(完整路径)
    }
    while(file_isDirExist(VideoClipsFolderPath));
    QString VideoClipsFolderName = DateStr+"_VideoClipsWaifu2xEX";//存储视频片段的文件夹(名称)
    //==========================
    //   检测之前的视频配置文件
    //==========================
    QString VideoConfiguration_fullPath = SourceFile_FolderPath+"/VideoConfiguration_"+SourceFile_BaseName+"_"+SourceFile_Suffix+"_Waifu2xEX_VFI.ini";
    if(QFile::exists(VideoConfiguration_fullPath))
    {
        QSettings *configIniRead = new QSettings(VideoConfiguration_fullPath, QSettings::IniFormat);
        configIniRead->setIniCodec(QTextCodec::codecForName("UTF-8"));
        //============ 修正文件夹名称 =============
        QString VideoClipsFolderPath_old = configIniRead->value("/VideoConfiguration/VideoClipsFolderPath").toString();
        QString VideoClipsFolderName_old = configIniRead->value("/VideoConfiguration/VideoClipsFolderName").toString();
        file_mkDir(VideoClipsFolderPath_old);
        if(file_isDirExist(VideoClipsFolderPath_old)==true)
        {
            VideoClipsFolderPath = VideoClipsFolderPath_old;
            VideoClipsFolderName = VideoClipsFolderName_old;
        }
        //============
        int MultipleOfFPS_old = configIniRead->value("/VideoConfiguration/MultipleOfFPS").toInt();
        if(MultipleOfFPS_old != ui->spinBox_MultipleOfFPS_VFI->value())
        {
            emit Send_TextBrowser_NewMessage(tr("Previous video cache will be deleted, due to the [Multiple of FPS] was changed. [")+SourceFile_fullPath+"]");
            DelVfiDir(video_mp4_fullpath);
            file_DelDir(SplitFramesFolderPath);
            QFile::remove(VideoConfiguration_fullPath);
            file_DelDir(VideoClipsFolderPath);
        }
    }
    else
    {
        emit Send_video_write_VideoConfiguration(VideoConfiguration_fullPath,0,0,false,0,0,"",true,VideoClipsFolderPath,VideoClipsFolderName,true,ui->spinBox_MultipleOfFPS_VFI->value());
    }
    //=======================
    //   检测缓存是否存在
    //=======================
    if(file_isDirExist(SplitFramesFolderPath))
    {
        emit Send_TextBrowser_NewMessage(tr("The previous video cache file is detected and processing of the previous video cache will continue. If you want to restart processing of the current video:[")+SourceFile_fullPath+tr("], delete the cache manually."));
    }
    else
    {
        QFile::remove(VideoConfiguration_fullPath);
        file_DelDir(SplitFramesFolderPath);
        file_DelDir(VideoClipsFolderPath);
        QFile::remove(AudioPath);
        DelVfiDir(video_mp4_fullpath);
        emit Send_video_write_VideoConfiguration(VideoConfiguration_fullPath,0,0,false,0,0,"",true,VideoClipsFolderPath,VideoClipsFolderName,true,ui->spinBox_MultipleOfFPS_VFI->value());
    }
    /*====================================
                  提取音频
    ======================================*/
    if(!QFile::exists(AudioPath))
    {
        video_get_audio(video_mp4_fullpath,AudioPath);
    }
    //================================== 开始分段处理视频 =================================================
    int StartTime = 0;//起始时间(秒)
    int VideoDuration = video_get_duration(video_mp4_fullpath);
    bool isSplitComplete = false;
    bool isScaleComplete = false;
    /*
    ============================================
                  开始之前先读取进度
    ============================================
    */
    int OLD_SegmentDuration=-1;
    bool read_OLD_SegmentDuration =false;
    int LastVideoClipNo = -1;
    if(QFile::exists(VideoConfiguration_fullPath))
    {
        QSettings *configIniRead = new QSettings(VideoConfiguration_fullPath, QSettings::IniFormat);
        configIniRead->setIniCodec(QTextCodec::codecForName("UTF-8"));
        //=================== 加载进度 =========================
        StartTime = configIniRead->value("/Progress/StartTime").toInt();
        isSplitComplete = configIniRead->value("/Progress/isSplitComplete").toBool();
        isScaleComplete = configIniRead->value("/Progress/isScaleComplete").toBool();
        OLD_SegmentDuration = configIniRead->value("/Progress/OLDSegmentDuration").toInt();
        LastVideoClipNo = configIniRead->value("/Progress/LastVideoClipNo").toInt();
    }
    if(OLD_SegmentDuration>0)
    {
        read_OLD_SegmentDuration = true;
    }
    /*
    加载进度条
    */
    int SegmentDuration_tmp_progressbar = 0;
    if(read_OLD_SegmentDuration)
    {
        SegmentDuration_tmp_progressbar = OLD_SegmentDuration;
    }
    else
    {
        SegmentDuration_tmp_progressbar = SegmentDuration;
    }
    if(ui->checkBox_ShowInterPro->isChecked()&&VideoDuration>SegmentDuration_tmp_progressbar)
    {
        emit Send_CurrentFileProgress_Start(SourceFile_BaseName+"."+SourceFile_Suffix,VideoDuration);
        if(StartTime>0)
        {
            emit Send_CurrentFileProgress_progressbar_Add_SegmentDuration(StartTime);
        }
    }
    /*
    ============================================
                    正式开始处理
    ============================================
    */
    int SegmentDuration_tmp=0;
    int TimeLeft_tmp=0;
    while(VideoDuration>StartTime)
    {
        /*==========================
                计算视频片段时间
        ==========================*/
        TimeLeft_tmp = VideoDuration-StartTime;
        if((TimeLeft_tmp)>=SegmentDuration)
        {
            SegmentDuration_tmp = SegmentDuration;
        }
        else
        {
            SegmentDuration_tmp = TimeLeft_tmp;
        }
        if(read_OLD_SegmentDuration)
        {
            SegmentDuration_tmp = OLD_SegmentDuration;
            read_OLD_SegmentDuration=false;
        }
        /*==========================
                 拆分视频片段
        ==========================*/
        if(isSplitComplete==false)
        {
            if(file_isDirExist(SplitFramesFolderPath))
            {
                file_DelDir(SplitFramesFolderPath);
                file_mkDir(SplitFramesFolderPath);
            }
            else
            {
                file_mkDir(SplitFramesFolderPath);
            }
            video_video2images_ProcessBySegment(video_mp4_fullpath,SplitFramesFolderPath,StartTime,SegmentDuration_tmp);
        }
        /*==========================
               处理视频片段的帧
        ==========================*/
        if(isScaleComplete==false)
        {
            //============================== 扫描获取文件名 ===============================
            QStringList Frame_fileName_list = file_getFileNames_in_Folder_nofilter(SplitFramesFolderPath);
            if(isSplitComplete==false)
            {
                if(Frame_fileName_list.isEmpty())//检查是否成功拆分为帧
                {
                    emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Unable to split video into pictures.]"));
                    emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed");
                    emit Send_progressbar_Add();
                    return 0;//如果启用stop位,则直接return
                }
            }
            /*
            记录进度
            帧拆分成功
            */
            emit Send_video_write_Progress_ProcessBySegment(VideoConfiguration_fullPath,StartTime,true,true,SegmentDuration_tmp,LastVideoClipNo);
        }
        /*==========================
            组装视频片段(由帧到视频)
        ==========================*/
        if(!file_isDirExist(VideoClipsFolderPath))
        {
            file_mkDir(VideoClipsFolderPath);
        }
        int VideoClipNo = LastVideoClipNo+1;
        QString video_mp4_scaled_clip_fullpath = VideoClipsFolderPath+"/"+QString::number(VideoClipNo,10)+".mp4";
        video_images2video(video_mp4_fullpath,video_mp4_scaled_clip_fullpath,SplitFramesFolderPath,"",false,1,1,false);
        if(!QFile::exists(video_mp4_scaled_clip_fullpath))//检查是否成功成功生成视频
        {
            if(waifu2x_STOP)
            {
                emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, "Interrupted");
                return 0;//如果启用stop位,则直接return
            }
            emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Unable to assemble pictures into videos.]"));
            emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed");
            emit Send_progressbar_Add();
            return 0;//如果启用stop位,则直接return
        }
        /*==========================
        处理完当前片段,时间后移,记录起始时间
        ==========================*/
        if(ui->checkBox_ShowInterPro->isChecked())
        {
            emit Send_CurrentFileProgress_progressbar_Add_SegmentDuration(SegmentDuration_tmp);
        }
        StartTime+=SegmentDuration_tmp;
        isSplitComplete = false;
        isScaleComplete = false;
        LastVideoClipNo=VideoClipNo;
        emit Send_video_write_Progress_ProcessBySegment(VideoConfiguration_fullPath,StartTime,false,false,-1,VideoClipNo);
    }
    emit Send_CurrentFileProgress_Stop();
    //======================================================
    //                    组装(片段到成片)
    //======================================================
    QString video_mp4_scaled_fullpath = "";
    video_mp4_scaled_fullpath = SourceFile_FolderPath+"/"+SourceFile_BaseName+"_W2xEX_VFI_"+SourceFile_Suffix+".mp4";
    QFile::remove(video_mp4_scaled_fullpath);
    video_AssembleVideoClips(VideoClipsFolderPath,VideoClipsFolderName,video_mp4_scaled_fullpath,AudioPath);
    if(QFile::exists(video_mp4_scaled_fullpath)==false)//检查是否成功生成视频
    {
        if(waifu2x_STOP)
        {
            emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, "Interrupted");
            return 0;//如果启用stop位,则直接return
        }
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Unable to assemble video clips.]"));
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed");
        emit Send_progressbar_Add();
        return 0;//如果启用stop位,则直接return
    }
    //============================== 删除缓存文件 ====================================================
    if(ui->checkBox_KeepVideoCache->isChecked()==false)
    {
        QFile::remove(VideoConfiguration_fullPath);
        file_DelDir(SplitFramesFolderPath);
        file_DelDir(VideoClipsFolderPath);
        QFile::remove(AudioPath);
        if(SourceFile_fullPath!=video_mp4_fullpath)QFile::remove(video_mp4_fullpath);
    }
    else
    {
        DelOriginal=false;
    }
    //============================= 删除原文件 &  & 更新table status ============================
    if(DelOriginal)
    {
        if(ReplaceOriginalFile(SourceFile_fullPath,video_mp4_scaled_fullpath)==false)
        {
            if(QAction_checkBox_MoveToRecycleBin_checkBox_DelOriginal->isChecked())
            {
                file_MoveToTrash(SourceFile_fullPath);
            }
            else
            {
                QFile::remove(SourceFile_fullPath);
            }
        }
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, "Finished, original file deleted");
    }
    else
    {
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, "Finished");
    }
    //========== 移动到输出路径 =========
    if(ui->checkBox_OutPath_isEnabled->isChecked())
    {
        MoveFileToOutputPath(video_mp4_scaled_fullpath,SourceFile_fullPath);
    }
    //============================ 更新进度条 =================================
    emit Send_progressbar_Add();
    //=========
    return 0;
}
/*
视频补帧
直接处理视频
*/
int MainWindow::FrameInterpolation_Video(int rowNum)
{
    //============================= 读取设置 ================================
    bool DelOriginal = (ui->checkBox_DelOriginal->isChecked()||ui->checkBox_ReplaceOriginalFile->isChecked());
    //========================= 拆解map得到参数 =============================
    emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, "Processing");
    QString SourceFile_fullPath = Table_model_video->item(rowNum,2)->text();
    if(!QFile::exists(SourceFile_fullPath))
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [File does not exist.]"));
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed");
        emit Send_progressbar_Add();
        return 0;
    }
    //==========================
    QFileInfo fileinfo(SourceFile_fullPath);
    QString file_name = file_getBaseName(SourceFile_fullPath);
    QString file_ext = fileinfo.suffix();
    QString file_path = file_getFolderPath(fileinfo);
    //===================================================================
    //生成mp4
    QString video_mp4_fullpath=video_To_CFRMp4(SourceFile_fullPath);
    if(!QFile::exists(video_mp4_fullpath))//检查是否成功生成mp4
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Cannot convert video format to mp4.]"));
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed");
        emit Send_progressbar_Add();
        return 0;//如果启用stop位,则直接return
    }
    QString AudioPath = file_path+"/Audio_"+file_name+"_"+file_ext+"_W2xEX.wav";//音频
    QString SplitFramesFolderPath = file_path+"/"+file_name+"_"+file_ext+"_SplitFrames_W2xEX";//拆分后存储frame的文件夹
    //==========================================
    //                   拆分(正常)
    //==========================================
    file_DelDir(SplitFramesFolderPath);
    file_mkDir(SplitFramesFolderPath);
    QFile::remove(AudioPath);
    video_video2images(video_mp4_fullpath,SplitFramesFolderPath,AudioPath);
    //============================== 扫描获取文件名 ===============================
    QStringList Frame_fileName_list = file_getFileNames_in_Folder_nofilter(SplitFramesFolderPath);
    if(Frame_fileName_list.isEmpty())//检查是否成功拆分为帧
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Unable to split video into pictures.]"));
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed");
        emit Send_progressbar_Add();
        return 0;//如果启用stop位,则直接return
    }
    //======================================== 组装 ======================================================
    QString video_mp4_scaled_fullpath = "";
    video_mp4_scaled_fullpath = file_path+"/"+file_name+"_W2xEX_VFI_"+file_ext+".mp4";
    video_images2video(video_mp4_fullpath,video_mp4_scaled_fullpath,SplitFramesFolderPath,AudioPath,false,1,1,false);
    if(QFile::exists(video_mp4_scaled_fullpath)==false)//检查是否成功成功生成视频
    {
        if(waifu2x_STOP)
        {
            emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, "Interrupted");
            return 0;//如果启用stop位,则直接return
        }
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Unable to assemble pictures into videos.]"));
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed");
        emit Send_progressbar_Add();
        return 0;//如果启用stop位,则直接return
    }
    //============================== 删除缓存文件 ====================================================
    if(ui->checkBox_KeepVideoCache->isChecked()==false)
    {
        file_DelDir(SplitFramesFolderPath);
        QFile::remove(AudioPath);
        if(SourceFile_fullPath!=video_mp4_fullpath)QFile::remove(video_mp4_fullpath);
    }
    else
    {
        DelOriginal=false;
    }
    //============================= 删除原文件 &  & 更新table status ============================
    if(DelOriginal)
    {
        if(ReplaceOriginalFile(SourceFile_fullPath,video_mp4_scaled_fullpath)==false)
        {
            if(QAction_checkBox_MoveToRecycleBin_checkBox_DelOriginal->isChecked())
            {
                file_MoveToTrash(SourceFile_fullPath);
            }
            else
            {
                QFile::remove(SourceFile_fullPath);
            }
        }
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, "Finished, original file deleted");
    }
    else
    {
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, "Finished");
    }
    //========== 移动到输出路径 =========
    if(ui->checkBox_OutPath_isEnabled->isChecked())
    {
        MoveFileToOutputPath(video_mp4_scaled_fullpath,SourceFile_fullPath);
    }
    //============================ 更新进度条 =================================
    emit Send_progressbar_Add();
    //===========================  ==============================
    return 0;
}


/*
视频补帧
*/
bool MainWindow::FrameInterpolation(QString SourcePath,QString OutputPath)
{
    if(SourcePath.right(1)=="/")
    {
        SourcePath = SourcePath.left(SourcePath.length() - 1);
    }
    if(OutputPath.right(1)=="/")
    {
        OutputPath = OutputPath.left(OutputPath.length() - 1);
    }
    if(file_isDirExist(SourcePath)==false)return false;
    //=======
    emit Send_TextBrowser_NewMessage(tr("Starting to interpolate frames in:[")+SourcePath+"]");
    //==== 检测是否启用了自动调整线程数量,若启用则强制设定重试次数大于6 ====
    int retry_add = 0;
    if(ui->checkBox_AutoAdjustNumOfThreads_VFI->isChecked()==true && ui->spinBox_retry->value()<6 && ui->checkBox_MultiThread_VFI->isChecked())
    {
        retry_add = 6-ui->spinBox_retry->value();
    }
    //==== 检查是否正在使用rife,若使用则检测是否需要启用uhd模式 ====
    bool isUhdInput=false;
    if(ui->comboBox_Engine_VFI->currentIndex()==0 && ui->checkBox_UHD_VFI->isChecked()==false)
    {
        QStringList SourceImagesNames = file_getFileNames_in_Folder_nofilter(SourcePath);
        QString ImgName_tmp;
        QMap<QString,int> res_map;
        int original_height;
        int original_width;
        for(int i=0; i<SourceImagesNames.size(); i++)
        {
            ImgName_tmp = SourceImagesNames.at(i);
            QFileInfo ImgName_tmp_info(ImgName_tmp);
            if(ImgName_tmp_info.suffix()=="png")
            {
                res_map = Image_Gif_Read_Resolution(SourcePath+"/"+ImgName_tmp);
                original_height = res_map["height"];
                original_width = res_map["width"];
                if(original_height>0 && original_width>0)//判断是否读取失败
                {
                    isUhdInput = ((original_height*original_width)>=8294400);
                    break;
                }
            }
        }
    }
    if(isUhdInput==true)Send_TextBrowser_NewMessage(tr("UHD input detected, UHD Mode is automatically enabled."));
    //====================
    int FileNum_MAX = file_getFileNames_in_Folder_nofilter(SourcePath).size() * ui->spinBox_MultipleOfFPS_VFI->value();
    int FileNum_New = 0;
    int FileNum_Old = 0;
    //========
    QString FrameInterpolation_ProgramPath="";
    switch (ui->comboBox_Engine_VFI->currentIndex())
    {
        case 0:
            {
                FrameInterpolation_ProgramPath = Current_Path+"/rife-ncnn-vulkan/rife-ncnn-vulkan_waifu2xEX.exe";
                break;
            }
        case 1:
            {
                FrameInterpolation_ProgramPath = Current_Path+"/cain-ncnn-vulkan/cain-ncnn-vulkan_waifu2xEX.exe";
                break;
            }
        case 2:
            {
                FrameInterpolation_ProgramPath = Current_Path+"/dain-ncnn-vulkan/dain-ncnn-vulkan_waifu2xEX.exe";
                break;
            }
    }
    QString CMD ="";
    //========
    bool FrameInterpolation_QProcess_failed = false;
    QString ErrorMSG="";
    QString StanderMSG="";
    int FrameNumDigits = CalNumDigits(FileNum_MAX);
    //========
    int MultiFPS_MAX = ui->spinBox_MultipleOfFPS_VFI->value();
    int MultiFPS_Init = 2;
    if(ui->comboBox_Engine_VFI->currentIndex()==2)MultiFPS_Init=MultiFPS_MAX;
    //初始化进度讯息
    bool is_progressBar_CurrentFile_available = false;
    if(ui->checkBox_ShowInterPro->isChecked()==true && ui->progressBar_CurrentFile->isVisible()==false)
    {
        is_progressBar_CurrentFile_available=true;
        emit Send_CurrentFileProgress_Start(SourcePath.split("/").last(),FileNum_MAX);
    }
    //========
    for(int MultiFPS_curr = MultiFPS_Init; MultiFPS_curr<=MultiFPS_MAX; MultiFPS_curr*=2)
    {
        bool isThisRoundSucceed = false;
        QString OutputPath_Curr = "";
        QString SourcePath_Curr = "";
        if(MultiFPS_curr == MultiFPS_Init)
        {
            SourcePath_Curr = SourcePath;
        }
        else
        {
            SourcePath_Curr = OutputPath+"_"+QString::number(MultiFPS_curr/2);
        }
        if(MultiFPS_curr == MultiFPS_MAX)
        {
            OutputPath_Curr = OutputPath;
        }
        else
        {
            OutputPath_Curr = OutputPath+"_"+QString::number(MultiFPS_curr);
        }
        file_DelDir(OutputPath_Curr);
        file_mkDir(OutputPath_Curr);
        //=======
        for(int retry=0; retry<(ui->spinBox_retry->value()+retry_add); retry++)
        {
            FrameInterpolation_QProcess_failed = false;
            ErrorMSG="";
            StanderMSG="";
            //=====
            QProcess FrameInterpolation_QProcess;
            CMD ="\""+FrameInterpolation_ProgramPath+"\" -i \""+SourcePath_Curr+"\" -o \""+OutputPath_Curr+"\" -f %0"+QString("%1").arg(FrameNumDigits)+"d.png"+FrameInterpolation_ReadConfig(isUhdInput,FileNum_MAX);
            FrameInterpolation_QProcess.start(CMD);
            while(!FrameInterpolation_QProcess.waitForStarted(200)&&!QProcess_stop) {}
            while(!FrameInterpolation_QProcess.waitForFinished(200)&&!QProcess_stop)
            {
                if(waifu2x_STOP)
                {
                    FrameInterpolation_QProcess.close();
                    file_DelDir(OutputPath_Curr);
                    if(SourcePath_Curr != SourcePath)file_DelDir(SourcePath_Curr);
                    if(is_progressBar_CurrentFile_available)
                    {
                        emit Send_CurrentFileProgress_Stop();//停止当前文件进度条
                    }
                    return false;
                }
                //=========
                ErrorMSG.append(FrameInterpolation_QProcess.readAllStandardError().toLower());
                StanderMSG.append(FrameInterpolation_QProcess.readAllStandardOutput().toLower());
                if(ErrorMSG.contains("failed")||StanderMSG.contains("failed"))
                {
                    FrameInterpolation_QProcess_failed = true;
                    FrameInterpolation_QProcess.close();
                    file_DelDir(OutputPath_Curr);
                    break;
                }
                //=========
                if(ui->checkBox_ShowInterPro->isChecked())
                {
                    FileNum_New = file_getFileNames_in_Folder_nofilter(OutputPath_Curr).size();
                    if(FileNum_New!=FileNum_Old)
                    {
                        if(is_progressBar_CurrentFile_available==false)
                        {
                            emit Send_TextBrowser_NewMessage(tr("Interpolating frames in:[")+SourcePath_Curr+tr("] Progress:[")+QString::number(FileNum_New,10)+"/"+QString::number(FileNum_MAX,10)+"]");
                        }
                        else
                        {
                            emit Send_CurrentFileProgress_progressbar_SetFinishedValue(FileNum_New);
                        }
                        FileNum_Old=FileNum_New;
                    }
                }
            }
            if(FrameInterpolation_QProcess_failed==false)
            {
                ErrorMSG.append(FrameInterpolation_QProcess.readAllStandardError().toLower());
                StanderMSG.append(FrameInterpolation_QProcess.readAllStandardOutput().toLower());
                if(ErrorMSG.contains("failed")||StanderMSG.contains("failed"))
                {
                    FrameInterpolation_QProcess_failed = true;
                    file_DelDir(OutputPath_Curr);
                }
            }
            //========= 检测是否成功,是否需要重试 ============
            if(FrameInterpolation_QProcess_failed==false && (file_getFileNames_in_Folder_nofilter(SourcePath_Curr).size() * MultiFPS_Init == file_getFileNames_in_Folder_nofilter(OutputPath_Curr).size()))
            {
                isThisRoundSucceed = true;
                break;
            }
            else
            {
                file_DelDir(OutputPath_Curr);
                //===
                if(retry==(ui->spinBox_retry->value()+retry_add-1))
                {
                    break;
                }
                //===
                if(retry>=2 && ui->checkBox_AutoAdjustNumOfThreads_VFI->isChecked()==true)
                {
                    isSuccessiveFailuresDetected_VFI=true;
                }
                //===
                file_mkDir(OutputPath_Curr);
                emit Send_TextBrowser_NewMessage(tr("Automatic retry, please wait."));
                Delay_sec_sleep(5);
            }
        }
        if(isThisRoundSucceed==true)
        {
            if(MultiFPS_curr == MultiFPS_MAX)
            {
                if(SourcePath_Curr != SourcePath)file_DelDir(SourcePath_Curr);
                emit Send_TextBrowser_NewMessage(tr("Finish interpolating frames in:[")+SourcePath+"]");
                if(is_progressBar_CurrentFile_available)
                {
                    emit Send_CurrentFileProgress_Stop();//停止当前文件进度条
                }
                return true;
            }
            else
            {
                if(SourcePath_Curr != SourcePath)file_DelDir(SourcePath_Curr);
            }
        }
        else
        {
            emit Send_TextBrowser_NewMessage(tr("Failed to interpolate frames in:[")+SourcePath+"]");
            //=======
            if(is_progressBar_CurrentFile_available)
            {
                emit Send_CurrentFileProgress_Stop();//停止当前文件进度条
            }
            //========
            return false;
        }
    }
    //=======
    emit Send_TextBrowser_NewMessage(tr("Failed to interpolate frames in:[")+SourcePath+"]");
    if(is_progressBar_CurrentFile_available)
    {
        emit Send_CurrentFileProgress_Stop();//停止当前文件进度条
    }
    //=======
    return false;
}

QString MainWindow::FrameInterpolation_ReadConfig(bool isUhdInput,int NumOfFrames)
{
    QString VFI_Config = " ";
    if(ui->comboBox_Engine_VFI->currentIndex()==0)
    {
        //TTA
        if(ui->checkBox_TTA_VFI->isChecked())
        {
            VFI_Config.append("-x ");
        }
        //UHD
        if(ui->checkBox_UHD_VFI->isChecked() || isUhdInput)
        {
            VFI_Config.append("-u ");
        }
    }
    //GPU & 多线程
    int NumOfThreads_VFI = 1;
    if(isSuccessiveFailuresDetected_VFI==false)
    {
        NumOfThreads_VFI = ui->spinBox_NumOfThreads_VFI->value();
    }
    if(ui->checkBox_MultiGPU_VFI->isChecked()==false)
    {
        //单显卡
        //GPU ID
        if(ui->comboBox_GPUID_VFI->currentText().trimmed().toLower()!="auto")
        {
            VFI_Config.append("-g "+ui->comboBox_GPUID_VFI->currentText().trimmed()+" ");
        }
        //线程数量
        QString jobs_num_str = QString("%1").arg(NumOfThreads_VFI);
        VFI_Config.append(QString("-j "+jobs_num_str+":"+jobs_num_str+":"+jobs_num_str+" "));
        //块大小(Dain)单显卡
        if(ui->comboBox_Engine_VFI->currentIndex()==2)VFI_Config.append("-t "+QString("%1").arg(ui->spinBox_TileSize_VFI->value())+" ");
    }
    else
    {
        //多显卡
        //GPU ID
        QString GPU_IDs_str = ui->lineEdit_MultiGPU_IDs_VFI->text().trimmed().trimmed().replace("，",",").remove(" ").remove("　");
        if(GPU_IDs_str.right(1)==",")
        {
            GPU_IDs_str = GPU_IDs_str.left(GPU_IDs_str.length() - 1);
        }
        QStringList GPU_IDs_StrList = GPU_IDs_str.split(",");
        GPU_IDs_StrList.removeAll("");
        GPU_IDs_StrList.removeDuplicates();
        if(GPU_IDs_StrList.isEmpty())
        {
            QString jobs_num_str = QString("%1").arg(NumOfThreads_VFI);
            VFI_Config.append(QString("-j "+jobs_num_str+":"+jobs_num_str+":"+jobs_num_str+" "));
            //块大小(Dain)单显卡
            if(ui->comboBox_Engine_VFI->currentIndex()==2)VFI_Config.append("-t "+QString("%1").arg(ui->spinBox_TileSize_VFI->value())+" ");
        }
        else
        {
            //===
            GPU_IDs_str = "";
            for(int i=0; i<GPU_IDs_StrList.size(); i++)
            {
                if(i==0)
                {
                    GPU_IDs_str.append(GPU_IDs_StrList.at(i));
                }
                else
                {
                    GPU_IDs_str.append(","+GPU_IDs_StrList.at(i));
                }
            }
            VFI_Config.append("-g "+GPU_IDs_str+" ");
            //线程数量
            int NumOfThreads_AVG = NumOfThreads_VFI / GPU_IDs_StrList.size();
            if(NumOfThreads_AVG<1)NumOfThreads_AVG=1;
            int NumOfThreads_Total = NumOfThreads_AVG * GPU_IDs_StrList.size();
            //===
            QString NumOfThreads_AVG_str = QString("%1").arg(NumOfThreads_AVG);
            QString NumOfThreads_Total_str = QString("%1").arg(NumOfThreads_Total);
            //===
            QString Jobs_Str = "";
            for(int i=0; i<GPU_IDs_StrList.size(); i++)
            {
                if(i==0)
                {
                    Jobs_Str.append(NumOfThreads_AVG_str);
                }
                else
                {
                    Jobs_Str.append(","+NumOfThreads_AVG_str);
                }
            }
            //===
            VFI_Config.append(QString("-j "+NumOfThreads_Total_str+":"+Jobs_Str+":"+NumOfThreads_Total_str+" "));
            //块大小(Dain)多显卡
            if(ui->comboBox_Engine_VFI->currentIndex()==2)
            {
                QString TileSizes_Str = "";
                for(int i=0; i<GPU_IDs_StrList.size(); i++)
                {
                    if(i==0)
                    {
                        TileSizes_Str.append(QString("%1").arg(ui->spinBox_TileSize_VFI->value()));
                    }
                    else
                    {
                        TileSizes_Str.append(","+QString("%1").arg(ui->spinBox_TileSize_VFI->value()));
                    }
                }
                VFI_Config.append("-t "+TileSizes_Str+" ");
            }
        }
    }
    //模型
    switch (ui->comboBox_Engine_VFI->currentIndex())
    {
        case 0://rife
            {
                VFI_Config.append("-m \""+Current_Path+"/rife-ncnn-vulkan/"+ui->comboBox_Model_VFI->currentText().trimmed()+"\" ");
                break;
            }
        case 1://cain
            {
                VFI_Config.append("-m \""+Current_Path+"/cain-ncnn-vulkan/cain\" ");
                break;
            }
        case 2://dain
            {
                VFI_Config.append("-m \""+Current_Path+"/dain-ncnn-vulkan/best\" ");
                //顺便把帧数倍率塞进去
                VFI_Config.append("-n "+QString("%1").arg(NumOfFrames)+" ");
                break;
            }
    }
    //========================
    return VFI_Config;
}

/*
================================================================================
                    REFI NCNN VULKAN 检测可用GPU
=================================================================================
*/

void MainWindow::on_pushButton_DetectGPU_VFI_clicked()
{
    //====
    ui->pushButton_DetectGPU_VFI->setText(tr("Detecting, please wait..."));
    //====
    pushButton_Start_setEnabled_self(0);
    ui->comboBox_GPUID_VFI->setEnabled(0);
    ui->pushButton_DetectGPU_VFI->setEnabled(0);
    Available_GPUID_FrameInterpolation.clear();
    QtConcurrent::run(this, &MainWindow::FrameInterpolation_DetectGPU);
}

int MainWindow::FrameInterpolation_DetectGPU()
{
    emit Send_TextBrowser_NewMessage(tr("Detecting available GPU, please wait."));
    //===============
    QString InputPath = Current_Path + "/Compatibility_Test/Compatibility_Test.jpg";
    QString InputPath_1 = Current_Path + "/Compatibility_Test/Compatibility_Test_1.jpg";
    QString OutputPath = Current_Path + "/Compatibility_Test/res.png";
    QFile::remove(OutputPath);
    //==============
    QString FrameInterpolation_ProgramPath = "";
    QString FrameInterpolation_ModelPath = "";
    QString TileSize_qstr = "";
    switch (ui->comboBox_Engine_VFI->currentIndex())
    {
        case 0:
            {
                FrameInterpolation_ProgramPath = Current_Path+"/rife-ncnn-vulkan/rife-ncnn-vulkan_waifu2xEX.exe";
                FrameInterpolation_ModelPath = Current_Path+"/rife-ncnn-vulkan/rife-v2.4";
                break;
            }
        case 1:
            {
                FrameInterpolation_ProgramPath = Current_Path+"/cain-ncnn-vulkan/cain-ncnn-vulkan_waifu2xEX.exe";
                FrameInterpolation_ModelPath = Current_Path+"/cain-ncnn-vulkan/cain";
                break;
            }
        case 2:
            {
                FrameInterpolation_ProgramPath = Current_Path+"/dain-ncnn-vulkan/dain-ncnn-vulkan_waifu2xEX.exe";
                FrameInterpolation_ModelPath = Current_Path+"/dain-ncnn-vulkan/best";
                TileSize_qstr = " -t 128 ";
                break;
            }
    }
    //=========
    int GPU_ID=-1;
    //=========
    while(true)
    {
        QFile::remove(OutputPath);
        QProcess *Waifu2x = new QProcess();
        QString gpu_str = " -g "+QString::number(GPU_ID,10)+" ";
        QString cmd = "\"" + FrameInterpolation_ProgramPath + "\"" + " -0 " + "\"" + InputPath + "\"" + " -1 " + "\"" + InputPath_1 + "\" -o " + "\"" + OutputPath + "\"" + " -j 1:1:1 " + gpu_str + " -m \""+FrameInterpolation_ModelPath+"\"" + TileSize_qstr;
        Waifu2x->start(cmd);
        while(!Waifu2x->waitForStarted(100)&&!QProcess_stop) {}
        while(!Waifu2x->waitForFinished(100)&&!QProcess_stop) {}
        if(QFile::exists(OutputPath) && (Waifu2x->readAllStandardError().toLower().contains("failed")||Waifu2x->readAllStandardOutput().toLower().contains("failed"))==false)
        {
            Available_GPUID_FrameInterpolation.append(QString::number(GPU_ID,10));
            GPU_ID++;
            QFile::remove(OutputPath);
        }
        else
        {
            if(GPU_ID > -1)
            {
                break;
            }
            else
            {
                GPU_ID++;
            }
        }
    }
    QFile::remove(OutputPath);
    //===============
    emit Send_TextBrowser_NewMessage(tr("Detection is complete!"));
    if(Available_GPUID_FrameInterpolation.isEmpty())
    {
        Send_TextBrowser_NewMessage(tr("No available GPU ID detected!"));
    }
    emit Send_FrameInterpolation_DetectGPU_finished();
    return 0;
}

int MainWindow::FrameInterpolation_DetectGPU_finished()
{
    pushButton_Start_setEnabled_self(1);
    ui->pushButton_DetectGPU_VFI->setEnabled(1);
    if(ui->checkBox_MultiGPU_VFI->isChecked()==false)ui->comboBox_GPUID_VFI->setEnabled(1);
    //====
    ui->comboBox_GPUID_VFI->clear();
    ui->comboBox_GPUID_VFI->addItem("auto");
    if(!Available_GPUID_FrameInterpolation.isEmpty())
    {
        QString AvaIDs_Str=tr("Available GPU IDs for current Frame Interpolation engine:[");
        for(int i=0; i<Available_GPUID_FrameInterpolation.size(); i++)
        {
            ui->comboBox_GPUID_VFI->addItem(Available_GPUID_FrameInterpolation.at(i));
            if(i == (Available_GPUID_FrameInterpolation.size()-1))
            {
                AvaIDs_Str.append(Available_GPUID_FrameInterpolation.at(i));
            }
            else
            {
                AvaIDs_Str.append(Available_GPUID_FrameInterpolation.at(i)+",");
            }
        }
        AvaIDs_Str.append("]");
        emit Send_TextBrowser_NewMessage(AvaIDs_Str);
    }
    //====
    ui->pushButton_DetectGPU_VFI->setText(tr("Detect available GPU ID"));
    //====
    return 0;
}

void MainWindow::on_lineEdit_MultiGPU_IDs_VFI_editingFinished()
{
    QString TMP_str = ui->lineEdit_MultiGPU_IDs_VFI->text();
    TMP_str = TMP_str.trimmed().replace("，",",").remove(" ").remove("　");
    ui->lineEdit_MultiGPU_IDs_VFI->setText(TMP_str);
}

void MainWindow::on_checkBox_MultiGPU_VFI_stateChanged(int arg1)
{
    bool tmp_bool = ui->checkBox_MultiGPU_VFI->isChecked();
    ui->comboBox_GPUID_VFI->setEnabled(!tmp_bool);
    ui->lineEdit_MultiGPU_IDs_VFI->setEnabled(tmp_bool);
    ui->pushButton_Verify_MultiGPU_VFI->setEnabled(tmp_bool);
}

void MainWindow::on_groupBox_FrameInterpolation_clicked()
{
    ui->frame_FrameInterpolation->setEnabled(ui->groupBox_FrameInterpolation->isChecked());
    ui->spinBox_MultipleOfFPS_VFI->setEnabled(ui->groupBox_FrameInterpolation->isChecked());
    ui->pushButton_MultipleOfFPS_VFI_ADD->setEnabled(ui->groupBox_FrameInterpolation->isChecked());
    ui->pushButton_MultipleOfFPS_VFI_MIN->setEnabled(ui->groupBox_FrameInterpolation->isChecked());
    on_comboBox_Engine_VFI_currentIndexChanged(0);
    on_checkBox_MultiGPU_VFI_stateChanged(0);
    if(ui->groupBox_FrameInterpolation->isChecked()==false)
    {
        ui->checkBox_FrameInterpolationOnly_Video->setEnabled(0);
        ui->checkBox_FrameInterpolationOnly_Video->setChecked(0);
    }
    else
    {
        ui->checkBox_FrameInterpolationOnly_Video->setEnabled(1);
    }
    if(ui->checkBox_EnableVFI_Home->isChecked() != ui->groupBox_FrameInterpolation->isChecked())
    {
        ui->checkBox_EnableVFI_Home->setChecked(ui->groupBox_FrameInterpolation->isChecked());
    }
}

void MainWindow::on_checkBox_EnableVFI_Home_clicked()
{
    if(ui->checkBox_EnableVFI_Home->isChecked() != ui->groupBox_FrameInterpolation->isChecked())
    {
        ui->groupBox_FrameInterpolation->setChecked(ui->checkBox_EnableVFI_Home->isChecked());
        on_groupBox_FrameInterpolation_clicked();
    }
}


void MainWindow::on_checkBox_isCompatible_RifeNcnnVulkan_clicked()
{
    ui->checkBox_isCompatible_RifeNcnnVulkan->setChecked(isCompatible_RifeNcnnVulkan);
}

void MainWindow::on_checkBox_isCompatible_CainNcnnVulkan_clicked()
{
    ui->checkBox_isCompatible_CainNcnnVulkan->setChecked(isCompatible_CainNcnnVulkan);
}

void MainWindow::on_checkBox_isCompatible_DainNcnnVulkan_clicked()
{
    ui->checkBox_isCompatible_DainNcnnVulkan->setChecked(isCompatible_DainNcnnVulkan);
}

void MainWindow::on_comboBox_Engine_VFI_currentIndexChanged(int index)
{
    if(Old_FrameInterpolation_Engine_Index!=ui->comboBox_Engine_VFI->currentIndex())
    {
        Available_GPUID_FrameInterpolation.clear();
        FrameInterpolation_DetectGPU_finished();
        ui->lineEdit_MultiGPU_IDs_VFI->setText("");
    }
    int MultipleOfFPS = ui->spinBox_MultipleOfFPS_VFI->value();
    if(ui->comboBox_Engine_VFI->currentIndex()==0)//rife
    {
        ui->checkBox_TTA_VFI->setEnabled(1);
        ui->checkBox_UHD_VFI->setEnabled(1);
        ui->comboBox_Model_VFI->setEnabled(1);
        ui->spinBox_TileSize_VFI->setEnabled(0);
        if((MultipleOfFPS&(MultipleOfFPS-1))!=0)
        {
            ui->spinBox_MultipleOfFPS_VFI->setValue(2);
        }
    }
    if(ui->comboBox_Engine_VFI->currentIndex()==1)//cain
    {
        ui->checkBox_TTA_VFI->setEnabled(0);
        ui->checkBox_UHD_VFI->setEnabled(0);
        ui->comboBox_Model_VFI->setEnabled(0);
        ui->spinBox_TileSize_VFI->setEnabled(0);
        if((MultipleOfFPS&(MultipleOfFPS-1))!=0)
        {
            ui->spinBox_MultipleOfFPS_VFI->setValue(2);
        }
    }
    if(ui->comboBox_Engine_VFI->currentIndex()==2)//dain
    {
        ui->checkBox_TTA_VFI->setEnabled(0);
        ui->checkBox_UHD_VFI->setEnabled(0);
        ui->comboBox_Model_VFI->setEnabled(0);
        ui->spinBox_TileSize_VFI->setEnabled(1);
    }
    Old_FrameInterpolation_Engine_Index = ui->comboBox_Engine_VFI->currentIndex();
}

void MainWindow::on_pushButton_Verify_MultiGPU_VFI_clicked()
{
    QString VerRes = "";
    //======
    QString GPU_IDs_str = ui->lineEdit_MultiGPU_IDs_VFI->text().trimmed().trimmed().replace("，",",").remove(" ").remove("　");
    if(GPU_IDs_str.right(1)==",")
    {
        GPU_IDs_str = GPU_IDs_str.left(GPU_IDs_str.length() - 1);
    }
    QStringList GPU_IDs_StrList = GPU_IDs_str.split(",");
    GPU_IDs_StrList.removeAll("");
    GPU_IDs_StrList.removeDuplicates();
    for(int i=0; i<GPU_IDs_StrList.size(); i++)
    {
        VerRes.append("GPU ["+QString::number(i,10)+"]: ID:["+GPU_IDs_StrList.at(i)+"]\n\n");
    }
    //=====
    if(VerRes=="")
    {
        VerRes=tr("Current configuration is wrong or empty.");
    }
    //======
    QMessageBox *MSG = new QMessageBox();
    MSG->setWindowTitle(tr("GPUs List"));
    MSG->setText(VerRes);
    MSG->setIcon(QMessageBox::Information);
    MSG->setModal(true);
    MSG->show();
}

void MainWindow::on_checkBox_MultiThread_VFI_stateChanged(int arg1)
{
    if(ui->checkBox_MultiThread_VFI->isChecked())
    {
        ui->checkBox_AutoAdjustNumOfThreads_VFI->setEnabled(1);
        ui->spinBox_NumOfThreads_VFI->setEnabled(1);
    }
    else
    {
        ui->checkBox_AutoAdjustNumOfThreads_VFI->setEnabled(0);
        ui->checkBox_AutoAdjustNumOfThreads_VFI->setChecked(0);
        ui->spinBox_NumOfThreads_VFI->setEnabled(0);
        ui->spinBox_NumOfThreads_VFI->setValue(1);
    }
}

void MainWindow::on_checkBox_MultiThread_VFI_clicked()
{
    if(ui->checkBox_MultiThread_VFI->isChecked())
    {
        ui->checkBox_AutoAdjustNumOfThreads_VFI->setEnabled(1);
        ui->checkBox_AutoAdjustNumOfThreads_VFI->setChecked(1);
        //========================
        QMessageBox *MSG_2 = new QMessageBox();
        MSG_2->setWindowTitle(tr("Warning"));
        MSG_2->setText(tr("The multi-threading of the frame interpolation engines is NOT very stable, we don't recommend you to enable this option."));
        MSG_2->setIcon(QMessageBox::Warning);
        MSG_2->setModal(true);
        MSG_2->show();
    }
}

QString MainWindow::isPreVFIDone_MarkFilePath(QString VideoPath)
{
    QFileInfo vfinfo(VideoPath);
    QString video_dir = file_getFolderPath(vfinfo);
    QString video_filename = file_getBaseName(VideoPath);
    QString video_ext = vfinfo.suffix();
    return video_dir+"/"+video_filename+"_"+video_ext+"_PreVFIDone.W2xEX";
}
