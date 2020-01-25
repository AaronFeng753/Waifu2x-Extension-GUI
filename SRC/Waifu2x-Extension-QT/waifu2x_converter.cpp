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

int MainWindow::Waifu2x_Converter_Image(QMap<QString, QString> File_map)
{
    //============================= 读取设置 ================================
    int ScaleRatio = ui->spinBox_ScaleRatio_image->value();
    int DenoiseLevel = ui->spinBox_DenoiseLevel_image->value();
    bool DelOriginal = ui->checkBox_DelOriginal->checkState();
    bool SaveAsJPG = ui->checkBox_SaveAsJPG->checkState();
    bool CompressJPG = ui->checkBox_CompressJPG->checkState();
    bool ReProcFinFiles = ui->checkBox_ReProcFinFiles->checkState();
    //========================= 拆解map得到参数 =============================
    int rowNum = File_map["rowNum"].toInt();
    QString status = "Processing";
    emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    QString SourceFile_fullPath = File_map["SourceFile_fullPath"];
    /*
    QFile qfile_fullPath(SourceFile_fullPath);
    if(!qfile_fullPath.isWritable())
    {
        emit Send_TextBrowser_NewMessage("Error occured when processing ["+SourceFile_fullPath+"]. Error: [Insufficient permissions, doesn't has write permission. Please give this software administrator permission.]");
        status = "Failed";
        emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        ThreadNumRunning--;//线程数量统计-1s
        return 0;
    }
    */
    QFileInfo fileinfo(SourceFile_fullPath);
    QString file_name = fileinfo.baseName();
    QString file_ext = fileinfo.suffix();
    QString file_path = fileinfo.path();
    if(file_path.right(1)=="/")
    {
        file_path = file_path.left(file_path.length() - 1);
    }
    QString OutPut_Path = file_path + "/" + file_name + "_waifu2x_"+QString::number(ScaleRatio, 10)+"x_"+QString::number(DenoiseLevel, 10)+"n.png";
    //============================== 放大 =======================================
    QProcess *Waifu2x = new QProcess();
    QString Current_Path = qApp->applicationDirPath();
    QString Waifu2x_folder_path = Current_Path + "/waifu2x-converter";
    QString program = Waifu2x_folder_path + "/waifu2x-converter_x64.exe";
    QString model_path= Waifu2x_folder_path + "/models_rgb";
    QString Denoise_cmd = "";
    if(DenoiseLevel==0)
    {
        Denoise_cmd = " ";
    }
    else
    {
        Denoise_cmd = " --noise_level " + QString::number(DenoiseLevel, 10);
    }
    QString cmd = "\"" + program + "\"" + " -i " + "\"" + SourceFile_fullPath + "\"" + " -o " + "\"" + OutPut_Path + "\"" + " --scale_ratio " + QString::number(ScaleRatio, 10) + Denoise_cmd + " --model_dir " + "\"" + model_path + "\"";
    Waifu2x->start(cmd);
    Waifu2x->waitForStarted();
    while(!Waifu2x->waitForFinished(500))
    {
        if(waifu2x_STOP)
        {
            Waifu2x->close();
            status = "Interrupted";
            emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, status);
            ThreadNumRunning--;
            return 0;
        }
    }
    if(!file_isFileExist(OutPut_Path))
    {
        emit Send_TextBrowser_NewMessage("Error occured when processing ["+SourceFile_fullPath+"]. Error: [Unable to scale the picture.]");
        status = "Failed";
        emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        ThreadNumRunning--;
        return 0;
    }
    //=========================== 另存为JPG&压缩JPG ===========================================
    if(SaveAsJPG)
    {
        QString OutPut_Path_jpg;
        QImage qimage;
        qimage.load(OutPut_Path);
        QImageWriter qimageW;
        qimageW.setFormat("jpg");
        if(CompressJPG)//压缩JPG
        {
            OutPut_Path_jpg = file_path + "/" + file_name + "_waifu2x_"+QString::number(ScaleRatio, 10)+"x_"+QString::number(DenoiseLevel, 10)+"n_compressed.jpg";
            qimageW.setQuality(90);
            qimageW.setFileName(OutPut_Path_jpg);
        }
        else
        {
            OutPut_Path_jpg = file_path + "/" + file_name + "_waifu2x_"+QString::number(ScaleRatio, 10)+"x_"+QString::number(DenoiseLevel, 10)+"n.jpg";
            qimageW.setQuality(100);
            qimageW.setFileName(OutPut_Path_jpg);
        }
        if(qimageW.canWrite())
        {
            qimageW.write(qimage);
        }
        else
        {
            // 获取错误信息
            QImageWriter::ImageWriterError error = qimageW.error();
            QString strError = qimageW.errorString();
            emit TextBrowser_NewMessage(strError);
        }
        if(file_isFileExist(OutPut_Path_jpg))
        {
            QFile::remove(OutPut_Path);
        }
        else
        {
            emit Send_TextBrowser_NewMessage("Error occured when processing ["+SourceFile_fullPath+"]. Error: [Cannot save scaled picture as .jpg.]");
        }
    }
    //============================= 删除原文件 & 更新filelist & 更新table status ============================
    if(DelOriginal)
    {
        QFile::remove(SourceFile_fullPath);
        FileList_remove(File_map);
        status = "Finished, original file deleted";
        emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    }
    else
    {
        if(!ReProcFinFiles)
        {
            FileList_remove(File_map);
            FileList_image_finished.append(File_map);
        }
        status = "Finished";
        emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    }
    //============================ 更新进度条 =================================
    emit Send_progressbar_Add();
    //=========================== 更新线程数量统计==============================
    ThreadNumRunning--;//线程数量统计-1s
    return 0;
}


int MainWindow::Waifu2x_Converter_GIF(QMap<QString, QString> File_map)
{
    //============================= 读取设置 ================================
    int ScaleRatio = ui->spinBox_ScaleRatio_gif->value();
    int DenoiseLevel = ui->spinBox_DenoiseLevel_gif->value();
    bool DelOriginal = ui->checkBox_DelOriginal->checkState();
    bool ReProcFinFiles = ui->checkBox_ReProcFinFiles->checkState();
    bool OptGIF = ui->checkBox_OptGIF->checkState();
    int Sub_gif_ThreadNumRunning = 0;
    //========================= 拆解map得到参数 =============================
    int rowNum = File_map["rowNum"].toInt();
    QString status = "Processing";
    emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    QString SourceFile_fullPath = File_map["SourceFile_fullPath"];
    /*
    QFile qfile_fullPath(SourceFile_fullPath);
    if(!qfile_fullPath.isWritable())
    {
        emit Send_TextBrowser_NewMessage("Error occured when processing ["+SourceFile_fullPath+"]. Error: [Insufficient permissions, doesn't has write permission. Please give this software administrator permission.]");
        status = "Failed";
        emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        ThreadNumRunning--;//线程数量统计-1s
        return 0;
    }*/
    QFileInfo fileinfo(SourceFile_fullPath);
    QString file_name = fileinfo.baseName();
    QString file_ext = fileinfo.suffix();
    QString file_path = fileinfo.path();
    if(file_path.right(1)=="/")
    {
        file_path = file_path.left(file_path.length() - 1);
    }
    QString ResGIFPath = file_path + "/" + file_name + "_waifu2x_"+QString::number(ScaleRatio, 10)+"x_"+QString::number(DenoiseLevel, 10)+"n.gif";
    //=========================== 获取帧时间 ====================================
    int GIF_Duration = Gif_getDuration(SourceFile_fullPath);
    if(GIF_Duration==0)//检查是否成功获取duration
    {
        emit Send_TextBrowser_NewMessage("Error occured when processing ["+SourceFile_fullPath+"]. Error: [Can't get Duration value of GIF file.]");
        status = "Failed";
        emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        //file_DelDir(SplitFramesFolderPath);
        ThreadNumRunning--;//线程数量统计-1s
        return 0;//如果启用stop位,则直接return
    }
    //============================== 拆分 ==========================================
    QString SplitFramesFolderPath = file_path+"/"+file_name+"_splitFrames_waifu2x";//拆分后存储frame的文件夹
    if(file_isDirExist(SplitFramesFolderPath))
    {
        file_DelDir(SplitFramesFolderPath);
        file_mkDir(SplitFramesFolderPath);
    }
    else
    {
        file_mkDir(SplitFramesFolderPath);
    }
    Gif_splitGif(SourceFile_fullPath,SplitFramesFolderPath);
    //============================== 扫描获取文件名 ===============================
    QStringList Frame_fileName_list = file_getFileNames_in_Folder_nofilter(SplitFramesFolderPath);
    if(Frame_fileName_list.isEmpty())//检查是否成功拆分gif
    {
        emit Send_TextBrowser_NewMessage("Error occured when processing ["+SourceFile_fullPath+"]. Error: [Can't split GIF into frames.]");
        status = "Failed";
        emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        file_DelDir(SplitFramesFolderPath);
        ThreadNumRunning--;//线程数量统计-1s
        return 0;//如果启用stop位,则直接return
    }
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
    for(int i = 0; i < Frame_fileName_list.size(); i++)
    {
        InterPro_now++;
        if(ui->checkBox_ShowInterPro->checkState())
        {
            emit Send_TextBrowser_NewMessage("File name:["+SourceFile_fullPath+"]  Scale and Denoise progress:["+QString::number(InterPro_now,10)+"/"+QString::number(InterPro_total,10)+"]");
        }
        int Sub_gif_ThreadNumMax = ui->spinBox_ThreadNum_gif_internal->value();
        if(waifu2x_STOP)
        {
            while (Sub_gif_ThreadNumRunning > 0)
            {
                Delay_msec_sleep(500);
            }
            file_DelDir(SplitFramesFolderPath);
            status = "Interrupted";
            emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
            ThreadNumRunning--;//线程数量统计-1s
            return 0;//如果启用stop位,则直接return
        }
        Sub_gif_ThreadNumRunning++;
        QString Frame_fileName = Frame_fileName_list.at(i);
        QtConcurrent::run(this,&MainWindow::Waifu2x_Converter_GIF_scale,Frame_fileName,SplitFramesFolderPath,ScaledFramesFolderPath,&Sub_gif_ThreadNumRunning);
        while (Sub_gif_ThreadNumRunning >= Sub_gif_ThreadNumMax)
        {
            Delay_msec_sleep(500);
        }
    }
    while (Sub_gif_ThreadNumRunning!=0)
    {
        Delay_msec_sleep(500);
    }
    //======================= 检查是否成功放大所有帧 ===========================
    QStringList Frame_fileName_list_scaled = file_getFileNames_in_Folder_nofilter(ScaledFramesFolderPath);
    if(Frame_fileName_list.count()!=Frame_fileName_list_scaled.count())
    {
        emit Send_TextBrowser_NewMessage("Error occured when processing ["+SourceFile_fullPath+"]. Error: [Failed to scale frames.]");
        status = "Failed";
        emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        file_DelDir(SplitFramesFolderPath);
        ThreadNumRunning--;//线程数量统计-1s
        return 0;//如果启用stop位,则直接return
    }
    //======================================== 组装 ======================================================
    Gif_assembleGif(ResGIFPath,ScaledFramesFolderPath,GIF_Duration);
    if(!file_isFileExist(ResGIFPath))
    {
        emit Send_TextBrowser_NewMessage("Error occured when processing ["+SourceFile_fullPath+"]. Error: [Unable to assemble gif.]");
        status = "Failed";
        emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        file_DelDir(SplitFramesFolderPath);
        ThreadNumRunning--;//线程数量统计-1s
        return 0;//如果启用stop位,则直接return
    }
    //======================================= 优化gif ===================================================
    if(OptGIF)
    {
        QString ResGIFPath_compressed = file_path + "/" + file_name + "_waifu2x_"+QString::number(ScaleRatio, 10)+"x_"+QString::number(DenoiseLevel, 10)+"n_opt.gif";
        Gif_compressGif(ResGIFPath,ResGIFPath_compressed);
        if(file_isFileExist(ResGIFPath_compressed))
        {
            QFile::remove(ResGIFPath);
        }
        else
        {
            emit Send_TextBrowser_NewMessage("Error occured when processing ["+SourceFile_fullPath+"]. Error: [Can't optimize gif.]");
        }
    }
    //============================== 删除缓存文件 ====================================================
    file_DelDir(SplitFramesFolderPath);
    //============================= 删除原文件 & 更新filelist & 更新table status ============================
    if(DelOriginal)
    {
        QFile::remove(SourceFile_fullPath);
        FileList_remove(File_map);
        status = "Finished, original file deleted";
        emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    }
    else
    {
        if(!ReProcFinFiles)
        {
            FileList_remove(File_map);
            FileList_gif_finished.append(File_map);
        }
        status = "Finished";
        emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    }
    //============================ 更新进度条 =================================
    emit Send_progressbar_Add();
    //=========================== 更新filelist ==============================
    ThreadNumRunning--;//线程数量统计-1s
    return 0;
}

int MainWindow::Waifu2x_Converter_GIF_scale(QString Frame_fileName,QString SplitFramesFolderPath,QString ScaledFramesFolderPath,int *Sub_gif_ThreadNumRunning)
{
    int ScaleRatio = ui->spinBox_ScaleRatio_gif->value();
    int DenoiseLevel = ui->spinBox_DenoiseLevel_gif->value();
    //========================================================================
    QProcess *Waifu2x = new QProcess();
    QString Current_Path = qApp->applicationDirPath();
    QString Waifu2x_folder_path = Current_Path + "/waifu2x-converter";
    QString program = Waifu2x_folder_path + "/waifu2x-converter_x64.exe";
    QString model_path= Waifu2x_folder_path + "/models_rgb";
    QString InputPath = SplitFramesFolderPath+"/"+Frame_fileName;
    QString OutputPath = ScaledFramesFolderPath+"/"+Frame_fileName;
    QString Denoise_cmd = "";
    if(DenoiseLevel==0)
    {
        Denoise_cmd = " ";
    }
    else
    {
        Denoise_cmd = " --noise_level " + QString::number(DenoiseLevel, 10);
    }
    QString cmd = "\"" + program + "\"" + " -i " + "\"" + InputPath + "\"" + " -o " + "\"" + OutputPath + "\"" + " --scale_ratio " + QString::number(ScaleRatio, 10) + Denoise_cmd + " --model_dir " + "\"" + model_path + "\"";
    Waifu2x->start(cmd);
    Waifu2x->waitForStarted();
    while(!Waifu2x->waitForFinished(500))
    {
        if(waifu2x_STOP)
        {
            Waifu2x->close();
            *Sub_gif_ThreadNumRunning=*Sub_gif_ThreadNumRunning-1;
            return 0;
        }
    }
    QFile::remove(InputPath);
    *Sub_gif_ThreadNumRunning=*Sub_gif_ThreadNumRunning-1;
    return 0;
}




int MainWindow::Waifu2x_Converter_Video(QMap<QString, QString> File_map)
{
    //============================= 读取设置 ================================
    bool DelOriginal = ui->checkBox_DelOriginal->checkState();
    bool ReProcFinFiles = ui->checkBox_ReProcFinFiles->checkState();
    int Sub_video_ThreadNumRunning = 0;
    //========================= 拆解map得到参数 =============================
    int rowNum = File_map["rowNum"].toInt();
    QString status = "Processing";
    emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    QString SourceFile_fullPath = File_map["SourceFile_fullPath"];
    /*
    QFile qfile_fullPath(SourceFile_fullPath);
    if(!qfile_fullPath.isWritable())
    {
        emit Send_TextBrowser_NewMessage("Error occured when processing ["+SourceFile_fullPath+"]. Error: [Insufficient permissions, doesn't has write permission. Please give this software administrator permission.]");
        status = "Failed";
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        ThreadNumRunning--;//线程数量统计-1s
        return 0;
    }*/
    QFileInfo fileinfo(SourceFile_fullPath);
    QString file_name = fileinfo.baseName();
    QString file_ext = fileinfo.suffix();
    QString file_path = fileinfo.path();
    if(file_path.right(1)=="/")
    {
        file_path = file_path.left(file_path.length() - 1);
    }
    QString video_mp4_fullpath = file_path+"/"+file_name+".mp4";
    if(file_ext!="mp4")
    {
        QFile::remove(video_mp4_fullpath);
    }
    QString AudioPath = file_path+"/audio_waifu2x.wav";
    //============================== 拆分 ==========================================
    QString SplitFramesFolderPath = file_path+"/"+file_name+"_splitFrames_waifu2x";//拆分后存储frame的文件夹
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
    video_video2images(SourceFile_fullPath,SplitFramesFolderPath);
    if(!file_isFileExist(video_mp4_fullpath))//检查是否成功生成mp4
    {
        emit Send_TextBrowser_NewMessage("Error occured when processing ["+SourceFile_fullPath+"]. Error: [Cannot convert video format to mp4.]");
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
        emit Send_TextBrowser_NewMessage("Error occured when processing ["+SourceFile_fullPath+"]. Error: [Unable to split video into pictures.]");
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
    for(int i = 0; i < Frame_fileName_list.size(); i++)
    {
        InterPro_now++;
        if(ui->checkBox_ShowInterPro->checkState())
        {
            emit Send_TextBrowser_NewMessage("File name:["+SourceFile_fullPath+"]  Scale and Denoise progress:["+QString::number(InterPro_now,10)+"/"+QString::number(InterPro_total,10)+"]");
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
            status = "Interrupted";
            emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
            ThreadNumRunning--;//线程数量统计-1s
            return 0;//如果启用stop位,则直接return
        }
        Sub_video_ThreadNumRunning++;
        QString Frame_fileName = Frame_fileName_list.at(i);
        QtConcurrent::run(this,&MainWindow::Waifu2x_Converter_Video_scale,Frame_fileName,SplitFramesFolderPath,ScaledFramesFolderPath,&Sub_video_ThreadNumRunning);
        while (Sub_video_ThreadNumRunning >= Sub_video_ThreadNumMax)
        {
            Delay_msec_sleep(500);
        }
    }
    while (Sub_video_ThreadNumRunning!=0)
    {
        Delay_msec_sleep(500);
    }
    //================ 扫描放大后的帧文件数量,判断是否放大成功 =======================
    QStringList Frame_fileName_list_scaled = file_getFileNames_in_Folder_nofilter(ScaledFramesFolderPath);
    if(Frame_fileName_list.count()!=Frame_fileName_list_scaled.count())
    {
        emit Send_TextBrowser_NewMessage("Error occured when processing ["+SourceFile_fullPath+"]. Error: [Unable to scale all frames.]");
        status = "Failed";
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        file_DelDir(SplitFramesFolderPath);
        QFile::remove(AudioPath);
        ThreadNumRunning--;//线程数量统计-1s
        return 0;//如果启用stop位,则直接return
    }
    //======================================== 组装 ======================================================
    QString video_mp4_scaled_fullpath = file_path+"/"+file_name+"_waifu2x.mp4";
    QFile::remove(video_mp4_scaled_fullpath);
    video_images2video(video_mp4_fullpath,video_mp4_scaled_fullpath,ScaledFramesFolderPath);
    if(!file_isFileExist(video_mp4_scaled_fullpath))//检查是否成功成功生成视频
    {
        emit Send_TextBrowser_NewMessage("Error occured when processing ["+SourceFile_fullPath+"]. Error: [Unable to assemble pictures into videos.]");
        status = "Failed";
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        file_DelDir(SplitFramesFolderPath);
        QFile::remove(AudioPath);
        ThreadNumRunning--;//线程数量统计-1s
        return 0;//如果启用stop位,则直接return
    }
    //============================== 删除缓存文件 ====================================================
    if(file_isDirExist(SplitFramesFolderPath))
    {
        file_DelDir(SplitFramesFolderPath);
    }
    //============================= 删除原文件 & 更新filelist & 更新table status ============================
    if(DelOriginal)
    {
        QFile::remove(SourceFile_fullPath);
        QFile::remove(video_mp4_fullpath);
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
    //=========================== 更新filelist ==============================
    ThreadNumRunning--;//线程数量统计-1s
    return 0;
}




int MainWindow::Waifu2x_Converter_Video_scale(QString Frame_fileName,QString SplitFramesFolderPath,QString ScaledFramesFolderPath,int *Sub_video_ThreadNumRunning)
{
    int ScaleRatio = ui->spinBox_ScaleRatio_video->value();
    int DenoiseLevel = ui->spinBox_DenoiseLevel_video->value();
    QString Frame_fileFullPath = SplitFramesFolderPath+"/"+Frame_fileName;
    //========================================================================
    QProcess *Waifu2x = new QProcess();
    QString Current_Path = qApp->applicationDirPath();
    QString Waifu2x_folder_path = Current_Path + "/waifu2x-converter";
    QString program = Waifu2x_folder_path + "/waifu2x-converter_x64.exe";
    QString model_path= Waifu2x_folder_path + "/models_rgb";
    QString InputPath = SplitFramesFolderPath+"/"+Frame_fileName;
    QString OutputPath = ScaledFramesFolderPath+"/"+Frame_fileName;
    QString Denoise_cmd = "";
    if(DenoiseLevel==0)
    {
        Denoise_cmd = " ";
    }
    else
    {
        Denoise_cmd = " --noise_level " + QString::number(DenoiseLevel, 10);
    }
    QString cmd = "\"" + program + "\"" + " -i " + "\"" + InputPath + "\"" + " -o " + "\"" + OutputPath + "\"" + " --scale_ratio " + QString::number(ScaleRatio, 10) + Denoise_cmd + " --model_dir " + "\"" + model_path + "\"";
    Waifu2x->start(cmd);
    Waifu2x->waitForStarted();
    while(!Waifu2x->waitForFinished(500))
    {
        if(waifu2x_STOP)
        {
            Waifu2x->close();
            *Sub_video_ThreadNumRunning=*Sub_video_ThreadNumRunning-1;
            return 0;
        }
    }
    QFile::remove(Frame_fileFullPath);
    *Sub_video_ThreadNumRunning=*Sub_video_ThreadNumRunning-1;
    return 0;
}


