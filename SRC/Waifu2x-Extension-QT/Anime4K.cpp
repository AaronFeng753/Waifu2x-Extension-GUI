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
调用Anime4k处理视频
读取设置,拆分,创建放大子线程,组装视频
*/
int MainWindow::Anime4k_Video(QMap<QString, QString> File_map)
{
    //============================= 读取设置 ================================
    int ScaleRatio = ui->spinBox_ScaleRatio_video->value();//放大倍数
    bool DelOriginal = ui->checkBox_DelOriginal->checkState();//是否删除源文件
    bool ReProcFinFiles = ui->checkBox_ReProcFinFiles->checkState();//是否重复处理已完成文件
    int Sub_video_ThreadNumRunning = 0;//子线程总数
    QString OutPutPath_Final ="";
    //========================= 拆解map得到参数 =============================
    int rowNum = File_map["rowNum"].toInt();//得到所在row
    //=== 将状态改为正在处理 =====
    QString status = "Processing";
    emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    //========================
    QString SourceFile_fullPath = File_map["SourceFile_fullPath"];
    if(!file_isFileExist(SourceFile_fullPath))//判断源文件是否存在
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [File does not exist.]"));
        status = "Failed";
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        ThreadNumRunning--;//线程数量统计-1s
        return 0;
    }
    //==========================
    bool CustRes_isEnabled = false;
    int CustRes_height=0;
    int CustRes_width=0;
    if(CustRes_isContained(SourceFile_fullPath))
    {
        CustRes_isEnabled=true;
        QMap<QString, QString> Res_map = CustRes_getResMap(SourceFile_fullPath);//res_map["fullpath"],["height"],["width"]
        CustRes_height=Res_map["height"].toInt();
        CustRes_width=Res_map["width"].toInt();
    }
    //==========================
    QFileInfo fileinfo(SourceFile_fullPath);
    QString file_name = file_getBaseName(fileinfo.filePath());
    QString file_ext = fileinfo.suffix();
    QString file_path = fileinfo.path();
    if(file_path.right(1)=="/")
    {
        file_path = file_path.left(file_path.length() - 1);
    }
    QString video_mp4_fullpath="";
    if(file_ext!="mp4")
    {
        video_mp4_fullpath = file_path+"/"+file_name+"_"+file_ext+".mp4";
        if(SourceFile_fullPath!=video_mp4_fullpath)QFile::remove(video_mp4_fullpath);
    }
    else
    {
        video_mp4_fullpath = file_path+"/"+file_name+".mp4";
    }
    QString AudioPath = file_path+"/audio_"+file_name+"_"+file_ext+"_waifu2x.wav";
    //============================== 拆分 ==========================================
    QString SplitFramesFolderPath = file_path+"/"+file_name+"_"+file_ext+"_splitFrames_waifu2x";//拆分后存储frame的文件夹
    if(file_isDirExist(SplitFramesFolderPath))
    {
        file_DelDir(SplitFramesFolderPath);
        file_mkDir(SplitFramesFolderPath);
    }
    else
    {
        file_mkDir(SplitFramesFolderPath);
    }
    QFile::remove(AudioPath);
    video_video2images(SourceFile_fullPath,SplitFramesFolderPath,AudioPath);
    if(!file_isFileExist(video_mp4_fullpath))//检查是否成功生成mp4
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Cannot convert video format to mp4.]"));
        status = "Failed";
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        file_DelDir(SplitFramesFolderPath);
        QFile::remove(AudioPath);
        ThreadNumRunning--;//线程数量统计-1s
        return 0;//如果启用stop位,则直接return
    }
    //============================== 扫描获取文件名 ===============================
    QStringList Frame_fileName_list = file_getFileNames_in_Folder_nofilter(SplitFramesFolderPath);
    if(Frame_fileName_list.isEmpty())//检查是否成功拆分为帧
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Unable to split video into pictures.]"));
        status = "Failed";
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        file_DelDir(SplitFramesFolderPath);
        QFile::remove(AudioPath);
        ThreadNumRunning--;//线程数量统计-1s
        return 0;//如果启用stop位,则直接return
    }
    //============================== 放大 =======================================
    //===========建立存储放大后frame的文件夹===========
    QString ScaledFramesFolderPath = SplitFramesFolderPath+"/scaled";
    if(file_isDirExist(ScaledFramesFolderPath))
    {
        file_DelDir(ScaledFramesFolderPath);
        file_mkDir(ScaledFramesFolderPath);
    }
    else
    {
        file_mkDir(ScaledFramesFolderPath);
    }
    //==========开始放大==========================
    int InterPro_total = Frame_fileName_list.size();
    int InterPro_now = 0;
    //===============
    QMap<QString,QString> Sub_Thread_info;
    Sub_Thread_info["SplitFramesFolderPath"]=SplitFramesFolderPath;
    Sub_Thread_info["ScaledFramesFolderPath"]=ScaledFramesFolderPath;
    Sub_Thread_info["SourceFile_fullPath"]=SourceFile_fullPath;
    //===============
    //===============
    bool Frame_failed = false;
    //===============
    for(int i = 0; i < Frame_fileName_list.size(); i++)
    {
        InterPro_now++;
        if(ui->checkBox_ShowInterPro->checkState())
        {
            emit Send_TextBrowser_NewMessage(tr("File name:[")+SourceFile_fullPath+tr("]  Scale progress:[")+QString::number(InterPro_now,10)+"/"+QString::number(InterPro_total,10)+"]");
        }
        int Sub_video_ThreadNumMax = ui->spinBox_ThreadNum_video_internal->value();
        if(waifu2x_STOP)
        {
            while (Sub_video_ThreadNumRunning > 0)
            {
                Delay_msec_sleep(500);
            }
            file_DelDir(SplitFramesFolderPath);
            QFile::remove(AudioPath);
            if(SourceFile_fullPath!=video_mp4_fullpath)QFile::remove(video_mp4_fullpath);
            status = "Interrupted";
            emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
            ThreadNumRunning--;//线程数量统计-1s
            return 0;//如果启用stop位,则直接return
        }
        //====
        Sub_video_ThreadNumRunning++;
        //====
        QString Frame_fileName = Frame_fileName_list.at(i);
        Sub_Thread_info["Frame_fileName"]=Frame_fileName;
        QtConcurrent::run(this,&MainWindow::Anime4k_Video_scale,Sub_Thread_info,&Sub_video_ThreadNumRunning,&Frame_failed);
        while (Sub_video_ThreadNumRunning >= Sub_video_ThreadNumMax)
        {
            Delay_msec_sleep(500);
        }
        if(Frame_failed)
        {
            emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Unable to scale all frames.]"));
            status = "Failed";
            emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
            file_DelDir(SplitFramesFolderPath);
            QFile::remove(AudioPath);
            if(SourceFile_fullPath!=video_mp4_fullpath)QFile::remove(video_mp4_fullpath);
            ThreadNumRunning--;//线程数量统计-1s
            return 0;//如果启用stop位,则直接return
        }
    }
    while (Sub_video_ThreadNumRunning>0)
    {
        Delay_msec_sleep(500);
    }
    //================ 扫描放大后的帧文件数量,判断是否放大成功 =======================
    QStringList Frame_fileName_list_scaled = file_getFileNames_in_Folder_nofilter(ScaledFramesFolderPath);
    if(Frame_fileName_list.count()!=Frame_fileName_list_scaled.count())
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Unable to scale all frames.]"));
        status = "Failed";
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        file_DelDir(SplitFramesFolderPath);
        if(SourceFile_fullPath!=video_mp4_fullpath)QFile::remove(video_mp4_fullpath);
        QFile::remove(AudioPath);
        ThreadNumRunning--;//线程数量统计-1s
        return 0;//如果启用stop位,则直接return
    }
    //======================================== 组装 ======================================================
    QString video_mp4_scaled_fullpath = "";
    if(CustRes_isEnabled)
    {
        video_mp4_scaled_fullpath = file_path+"/"+file_name+"_waifu2x_"+QString::number(CustRes_width,10)+"x"+QString::number(CustRes_height,10)+"_"+file_ext+".mp4";
    }
    else
    {
        video_mp4_scaled_fullpath = file_path+"/"+file_name+"_waifu2x_"+QString::number(ScaleRatio,10)+"x"+"_"+file_ext+".mp4";
    }
    QFile::remove(video_mp4_scaled_fullpath);
    video_images2video(video_mp4_fullpath,video_mp4_scaled_fullpath,ScaledFramesFolderPath,AudioPath,CustRes_isEnabled,CustRes_height,CustRes_width);
    if(!file_isFileExist(video_mp4_scaled_fullpath))//检查是否成功成功生成视频
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Unable to assemble pictures into videos.]"));
        status = "Failed";
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        file_DelDir(SplitFramesFolderPath);
        QFile::remove(AudioPath);
        ThreadNumRunning--;//线程数量统计-1s
        return 0;//如果启用stop位,则直接return
    }
    OutPutPath_Final = video_mp4_scaled_fullpath;
    //============================== 删除缓存文件 ====================================================
    if(file_isDirExist(SplitFramesFolderPath))
    {
        file_DelDir(SplitFramesFolderPath);
    }
    //============================= 删除原文件 & 更新filelist & 更新table status ============================
    if(DelOriginal)
    {
        if(ui->checkBox_Move2RecycleBin->checkState())
        {
            file_MoveToTrash(SourceFile_fullPath);
        }
        else
        {
            QFile::remove(SourceFile_fullPath);
        }
        if(SourceFile_fullPath!=video_mp4_fullpath)QFile::remove(video_mp4_fullpath);
        FileList_remove(File_map);
        status = "Finished, original file deleted";
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    }
    else
    {
        if(!ReProcFinFiles)
        {
            FileList_remove(File_map);
            FileList_video_finished.append(File_map);
        }
        status = "Finished";
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    }
    //============================ 更新进度条 =================================
    emit Send_progressbar_Add();
    //=========================== 删除转换格式生成的mp4 ==========================
    if(file_ext!="mp4")
    {
        if(SourceFile_fullPath!=video_mp4_fullpath)QFile::remove(video_mp4_fullpath);
    }
    //========== 移动到输出路径 =========
    if(ui->checkBox_OutPath_isEnabled->checkState())
    {
        QFileInfo fileinfo_final(OutPutPath_Final);
        QString Final_fileName = fileinfo_final.fileName();
        file_MoveFile(OutPutPath_Final,OutPutFolder_main+"/"+Final_fileName);
    }
    //=========================== 更新filelist ==============================
    ThreadNumRunning--;//线程数量统计-1s
    //========
    return 0;
}
/*
Anime4k视频放大子线程
放大,修改大小
*/
int MainWindow::Anime4k_Video_scale(QMap<QString,QString> Sub_Thread_info,int *Sub_video_ThreadNumRunning,bool *Frame_failed)
{
    QString SplitFramesFolderPath = Sub_Thread_info["SplitFramesFolderPath"];
    QString ScaledFramesFolderPath = Sub_Thread_info["ScaledFramesFolderPath"];
    QString SourceFile_fullPath = Sub_Thread_info["SourceFile_fullPath"];
    QString Frame_fileName = Sub_Thread_info["Frame_fileName"];
    //===========
    int ScaleRatio = ui->spinBox_ScaleRatio_video->value();
    QString Frame_fileFullPath = SplitFramesFolderPath+"/"+Frame_fileName;
    //========================================================================
    QString Anime4k_folder_path = Current_Path + "/Anime4K";
    QString program = Anime4k_folder_path + "/Anime4K.jar";
    QString InputPath = SplitFramesFolderPath+"/"+Frame_fileName;
    QString OutputPath = ScaledFramesFolderPath+"/"+Frame_fileName;
    QString cmd = "java -jar \"" + program + "\" \"" + InputPath + "\" \"" + OutputPath + "\" " + QString::number(ScaleRatio, 10);
    //======
    if(CustRes_isContained(SourceFile_fullPath))
    {
        QMap<QString, QString> Res_map = CustRes_getResMap(SourceFile_fullPath);//res_map["fullpath"],["height"],["width"]
        ScaleRatio = CustRes_CalNewScaleRatio(InputPath,Res_map["height"].toInt(),Res_map["width"].toInt());
        if(ScaleRatio==0)
        {
            emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+InputPath+tr("]. Error: [The resolution of the source file cannot be read, so the image cannot be scaled to a custom resolution.]"));
            *Sub_video_ThreadNumRunning=*Sub_video_ThreadNumRunning-1;
            return 0;
        }
    }
    //=======
    for(int retry=0; retry<(ui->spinBox_retry->value()); retry++)
    {
        QProcess *Waifu2x = new QProcess();
        Waifu2x->start(cmd);
        while(!Waifu2x->waitForStarted(100)&&!QProcess_stop) {}
        while(!Waifu2x->waitForFinished(500)&&!QProcess_stop)
        {
            if(waifu2x_STOP)
            {
                Waifu2x->close();
                *Sub_video_ThreadNumRunning=*Sub_video_ThreadNumRunning-1;
                return 0;
            }
        }
        if(file_isFileExist(OutputPath))
        {
            break;
        }
        else
        {
            Delay_sec_sleep(5);
            emit Send_TextBrowser_NewMessage(tr("Retry"));
        }
    }
    //=========
    QFile::remove(Frame_fileFullPath);
    if(file_isFileExist(OutputPath)==false)*Frame_failed=true;
    //========
    *Sub_video_ThreadNumRunning=*Sub_video_ThreadNumRunning-1;
    Delay_msec_sleep(10);
    //========
    return 0;
}
