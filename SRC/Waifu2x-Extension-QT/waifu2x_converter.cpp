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
    int BlockSize = ui->spinBox_BlockSize_converter->value();
    bool DelOriginal = ui->checkBox_DelOriginal->checkState();
    bool SaveAsJPG = ui->checkBox_SaveAsJPG->checkState();
    bool CompressJPG = ui->checkBox_CompressJPG->checkState();
    bool ReProcFinFiles = ui->checkBox_ReProcFinFiles->checkState();
    bool DisableGPU = ui->checkBox_DisableGPU_converter->checkState();
    bool ForceOpenCL = ui->checkBox_ForceOpenCL_converter->checkState();
    //========================= 拆解map得到参数 =============================
    int rowNum = File_map["rowNum"].toInt();
    QString status = "Processing";
    emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    QString SourceFile_fullPath = File_map["SourceFile_fullPath"];
    if(!file_isFileExist(SourceFile_fullPath))
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [File does not exist.]"));
        status = "Failed";
        emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        ThreadNumRunning--;//线程数量统计-1s
        return 0;
    }
    //=======================================================
    bool CustRes_isEnabled = false;
    int CustRes_height=0;
    int CustRes_width=0;
    if(CustRes_isContained(SourceFile_fullPath))
    {
        CustRes_isEnabled=true;
        QMap<QString, QString> Res_map = CustRes_getResMap(SourceFile_fullPath);//res_map["fullpath"],["height"],["width"]
        ScaleRatio = CustRes_CalNewScaleRatio(SourceFile_fullPath,Res_map["height"].toInt(),Res_map["width"].toInt());
        if(ScaleRatio==0)
        {
            emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [The resolution of the source file cannot be read, so the image cannot be scaled to a custom resolution.]"));
            status = "Failed";
            emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, status);
            ThreadNumRunning--;//线程数量统计-1s
            return 0;
        }
        CustRes_height=Res_map["height"].toInt();
        CustRes_width=Res_map["width"].toInt();
    }
    //=======================================================
    QFileInfo fileinfo(SourceFile_fullPath);
    QString file_name = file_getBaseName(fileinfo.filePath());
    QString file_ext = fileinfo.suffix();
    QString file_path = fileinfo.path();
    if(file_path.right(1)=="/")
    {
        file_path = file_path.left(file_path.length() - 1);
    }
    QString OutPut_Path = file_path + "/" + file_name + "_waifu2x_"+QString::number(ScaleRatio, 10)+"x_"+QString::number(DenoiseLevel, 10)+"n_"+file_ext+".png";
    //============================== 放大 =======================================
    QProcess *Waifu2x = new QProcess();
    QString Waifu2x_folder_path = Current_Path + "/waifu2x-converter";
    QString program = Waifu2x_folder_path + "/waifu2x-converter-cpp.exe";
    QString model_path= Waifu2x_folder_path + "/models_rgb";
    QString Denoise_cmd = " --noise-level " + QString::number(DenoiseLevel, 10);
    QString DisableGPU_cmd ="";
    if(DisableGPU)DisableGPU_cmd =" --disable-gpu ";
    QString ForceOpenCL_cmd ="";
    if(ForceOpenCL)ForceOpenCL_cmd =" --force-OpenCL ";
    //====
    QString cmd = "\"" + program + "\"" + " -i " + "\"" + SourceFile_fullPath + "\"" + " -o " + "\"" + OutPut_Path + "\"" + " --scale-ratio " + QString::number(ScaleRatio, 10) + Denoise_cmd + " --model-dir " + "\"" + model_path + "\" --block-size "+QString::number(BlockSize, 10)+DisableGPU_cmd+ForceOpenCL_cmd+Processor_converter_STR;
    //========
    Waifu2x->start(cmd);
    while(!Waifu2x->waitForStarted(100)&&!QProcess_stop) {}
    while(!Waifu2x->waitForFinished(500)&&!QProcess_stop)
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
    //========
    if(!file_isFileExist(OutPut_Path))
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Unable to scale the picture.]"));
        status = "Failed";
        emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        ThreadNumRunning--;
        return 0;
    }
    //============================ 调整大小 ====================================================
    if(CustRes_isEnabled)
    {
        QImage qimage_original;
        qimage_original.load(SourceFile_fullPath);
        int New_height=0;
        int New_width=0;
        if(CustRes_isEnabled)
        {
            New_height= CustRes_height;
            New_width= CustRes_width;
        }
        else
        {
            New_height = qimage_original.height()*ScaleRatio;
            New_width = qimage_original.width()*ScaleRatio;
        }
        QImage qimage_adj(OutPut_Path);
        //读取放大后的图片并调整大小
        QImage qimage_adj_scaled = qimage_adj.scaled(New_width,New_height,CustRes_AspectRatioMode,Qt::SmoothTransformation);
        QImageWriter qimageW_adj;
        qimageW_adj.setFormat("png");
        qimageW_adj.setFileName(OutPut_Path);
        if(qimageW_adj.canWrite())
        {
            qimageW_adj.write(qimage_adj_scaled);
        }
        if(!file_isFileExist(OutPut_Path))
        {
            emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Unable to resize the scaled picture to the target size]"));
            status = "Failed";
            emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, status);
            ThreadNumRunning--;//线程数量统计-1s
            return 0;
        }
    }
    if(CustRes_isEnabled)
    {
        QString OutPut_Path_CustRes = file_path + "/" + file_name + "_waifu2x_"+QString::number(CustRes_width, 10)+"x"+QString::number(CustRes_height, 10)+"_"+QString::number(DenoiseLevel, 10)+"n_"+file_ext+".png";
        QFile::rename(OutPut_Path,OutPut_Path_CustRes);
        OutPut_Path = OutPut_Path_CustRes;
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
            if(CustRes_isEnabled)
            {
                OutPut_Path_jpg = file_path + "/" + file_name + "_waifu2x_"+QString::number(CustRes_width, 10)+"x"+QString::number(CustRes_height, 10)+"_"+QString::number(DenoiseLevel, 10)+"n_compressed_"+file_ext+".jpg";
            }
            else
            {
                OutPut_Path_jpg = file_path + "/" + file_name + "_waifu2x_"+QString::number(ScaleRatio, 10)+"x_"+QString::number(DenoiseLevel, 10)+"n_compressed_"+file_ext+".jpg";
            }
            qimageW.setQuality(ui->spinBox_JPGCompressedQuality->value());
            qimageW.setFileName(OutPut_Path_jpg);
        }
        else
        {
            if(CustRes_isEnabled)
            {
                OutPut_Path_jpg = file_path + "/" + file_name + "_waifu2x_"+QString::number(CustRes_width, 10)+"x"+QString::number(CustRes_height, 10)+"_"+QString::number(DenoiseLevel, 10)+"n_"+file_ext+".jpg";
            }
            else
            {
                OutPut_Path_jpg = file_path + "/" + file_name + "_waifu2x_"+QString::number(ScaleRatio, 10)+"x_"+QString::number(DenoiseLevel, 10)+"n_"+file_ext+".jpg";
            }
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
            emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Cannot save scaled picture as .jpg.]"));
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
    if(!file_isFileExist(SourceFile_fullPath))
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [File does not exist.]"));
        status = "Failed";
        emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, status);
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
    QString ResGIFPath = file_path + "/" + file_name + "_waifu2x_"+QString::number(ScaleRatio, 10)+"x_"+QString::number(DenoiseLevel, 10)+"n.gif";
    //=========================== 获取帧时间 ====================================
    int GIF_Duration = Gif_getDuration(SourceFile_fullPath);
    if(GIF_Duration==0)//检查是否成功获取duration
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Can't get Duration value of GIF file.]"));
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
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Can't split GIF into frames.]"));
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
    //===============
    QMap<QString,QString> Sub_Thread_info;
    Sub_Thread_info["SplitFramesFolderPath"]=SplitFramesFolderPath;
    Sub_Thread_info["ScaledFramesFolderPath"]=ScaledFramesFolderPath;
    Sub_Thread_info["SourceFile_fullPath"]=SourceFile_fullPath;
    //===============
    for(int i = 0; i < Frame_fileName_list.size(); i++)
    {
        InterPro_now++;
        if(ui->checkBox_ShowInterPro->checkState())
        {
            emit Send_TextBrowser_NewMessage(tr("File name:[")+SourceFile_fullPath+tr("]  Scale and Denoise progress:[")+QString::number(InterPro_now,10)+"/"+QString::number(InterPro_total,10)+"]");
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
            emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, status);
            ThreadNumRunning--;//线程数量统计-1s
            return 0;//如果启用stop位,则直接return
        }
        Sub_gif_ThreadNumRunning++;
        QString Frame_fileName = Frame_fileName_list.at(i);
        QtConcurrent::run(this,&MainWindow::Waifu2x_Converter_GIF_scale,Frame_fileName,Sub_Thread_info,&Sub_gif_ThreadNumRunning);
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
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Failed to scale frames.]"));
        status = "Failed";
        emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        file_DelDir(SplitFramesFolderPath);
        ThreadNumRunning--;//线程数量统计-1s
        return 0;//如果启用stop位,则直接return
    }
    //======================================== 组装 ======================================================
    if(CustRes_isEnabled)
    {
        ResGIFPath = file_path + "/" + file_name + "_waifu2x_"+QString::number(CustRes_width, 10)+"x"+QString::number(CustRes_height,10)+"_"+QString::number(DenoiseLevel, 10)+"n.gif";
    }
    Gif_assembleGif(ResGIFPath,ScaledFramesFolderPath,GIF_Duration);
    if(!file_isFileExist(ResGIFPath))
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Unable to assemble gif.]"));
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
        if(CustRes_isEnabled)
        {
            ResGIFPath_compressed = file_path + "/" + file_name + "_waifu2x_"+QString::number(CustRes_width, 10)+"x"+QString::number(CustRes_height,10)+"_"+QString::number(DenoiseLevel, 10)+"n_opt.gif";
        }
        else
        {
            ResGIFPath_compressed = file_path + "/" + file_name + "_waifu2x_"+QString::number(ScaleRatio, 10)+"x_"+QString::number(DenoiseLevel, 10)+"n_opt.gif";
        }
        Gif_compressGif(ResGIFPath,ResGIFPath_compressed);
        if(file_isFileExist(ResGIFPath_compressed))
        {
            QFile::remove(ResGIFPath);
        }
        else
        {
            emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Can't optimize gif.]"));
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

int MainWindow::Waifu2x_Converter_GIF_scale(QString Frame_fileName,QMap<QString, QString> Sub_Thread_info,int *Sub_gif_ThreadNumRunning)
{
    QString SplitFramesFolderPath = Sub_Thread_info["SplitFramesFolderPath"];
    QString ScaledFramesFolderPath = Sub_Thread_info["ScaledFramesFolderPath"];
    QString SourceFile_fullPath = Sub_Thread_info["SourceFile_fullPath"];
    //===========
    int ScaleRatio = ui->spinBox_ScaleRatio_gif->value();
    int DenoiseLevel = ui->spinBox_DenoiseLevel_gif->value();
    int BlockSize = ui->spinBox_BlockSize_converter->value();
    bool DisableGPU = ui->checkBox_DisableGPU_converter->checkState();
    bool ForceOpenCL = ui->checkBox_ForceOpenCL_converter->checkState();
    //========================================================================
    QProcess *Waifu2x = new QProcess();
    QString Waifu2x_folder_path = Current_Path + "/waifu2x-converter";
    QString program = Waifu2x_folder_path + "/waifu2x-converter-cpp.exe";
    QString model_path= Waifu2x_folder_path + "/models_rgb";
    QString InputPath = SplitFramesFolderPath+"/"+Frame_fileName;
    QString OutputPath = ScaledFramesFolderPath+"/"+Frame_fileName;
    QString Denoise_cmd = " --noise-level " + QString::number(DenoiseLevel, 10);
    QString DisableGPU_cmd ="";
    if(DisableGPU)DisableGPU_cmd =" --disable-gpu ";
    QString ForceOpenCL_cmd ="";
    if(ForceOpenCL)ForceOpenCL_cmd =" --force-OpenCL ";
    //======
    bool CustRes_isEnabled = false;
    int CustRes_height=0;
    int CustRes_width=0;
    if(CustRes_isContained(SourceFile_fullPath))
    {
        CustRes_isEnabled=true;
        QMap<QString, QString> Res_map = CustRes_getResMap(SourceFile_fullPath);//res_map["fullpath"],["height"],["width"]
        ScaleRatio = CustRes_CalNewScaleRatio(InputPath,Res_map["height"].toInt(),Res_map["width"].toInt());
        if(ScaleRatio==0)
        {
            emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+InputPath+tr("]. Error: [The resolution of the source file cannot be read, so the image cannot be scaled to a custom resolution.]"));
            *Sub_gif_ThreadNumRunning=*Sub_gif_ThreadNumRunning-1;
            return 0;
        }
        CustRes_height=Res_map["height"].toInt();
        CustRes_width=Res_map["width"].toInt();
    }
    //=======
    QString cmd = "\"" + program + "\"" + " -i " + "\"" + InputPath + "\"" + " -o " + "\"" + OutputPath + "\"" + " --scale-ratio " + QString::number(ScaleRatio, 10) + Denoise_cmd + " --model-dir " + "\"" + model_path + "\" --block-size "+QString::number(BlockSize, 10)+DisableGPU_cmd+ForceOpenCL_cmd+Processor_converter_STR;
    Waifu2x->start(cmd);
    while(!Waifu2x->waitForStarted(100)&&!QProcess_stop) {}
    while(!Waifu2x->waitForFinished(500)&&!QProcess_stop)
    {
        if(waifu2x_STOP)
        {
            Waifu2x->close();
            *Sub_gif_ThreadNumRunning=*Sub_gif_ThreadNumRunning-1;
            return 0;
        }
    }
    //============================ 调整大小 ====================================================
    if(CustRes_isEnabled)
    {
        QImage qimage_original;
        qimage_original.load(InputPath);
        int New_height=0;
        int New_width=0;
        if(CustRes_isEnabled)
        {
            New_height= CustRes_height;
            New_width= CustRes_width;
        }
        else
        {
            New_height = qimage_original.height()*ScaleRatio;
            New_width = qimage_original.width()*ScaleRatio;
        }
        QImage qimage_adj(OutputPath);
        //读取放大后的图片并调整大小
        QImage qimage_adj_scaled = qimage_adj.scaled(New_width,New_height,CustRes_AspectRatioMode,Qt::SmoothTransformation);
        QImageWriter qimageW_adj;
        qimageW_adj.setFormat("png");
        qimageW_adj.setFileName(OutputPath);
        if(qimageW_adj.canWrite())
        {
            qimageW_adj.write(qimage_adj_scaled);
        }
    }
    QFile::remove(InputPath);
    *Sub_gif_ThreadNumRunning=*Sub_gif_ThreadNumRunning-1;
    return 0;
}




int MainWindow::Waifu2x_Converter_Video(QMap<QString, QString> File_map)
{
    //============================= 读取设置 ================================
    int ScaleRatio = ui->spinBox_ScaleRatio_video->value();
    int DenoiseLevel = ui->spinBox_DenoiseLevel_video->value();
    bool DelOriginal = ui->checkBox_DelOriginal->checkState();
    bool ReProcFinFiles = ui->checkBox_ReProcFinFiles->checkState();
    int Sub_video_ThreadNumRunning = 0;
    //========================= 拆解map得到参数 =============================
    int rowNum = File_map["rowNum"].toInt();
    QString status = "Processing";
    emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    QString SourceFile_fullPath = File_map["SourceFile_fullPath"];
    if(!file_isFileExist(SourceFile_fullPath))
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
    QString video_mp4_fullpath;
    if(file_ext!="mp4")
    {
        video_mp4_fullpath = file_path+"/"+file_name+"_"+file_ext+".mp4";
        QFile::remove(video_mp4_fullpath);
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
    for(int i = 0; i < Frame_fileName_list.size(); i++)
    {
        InterPro_now++;
        if(ui->checkBox_ShowInterPro->checkState())
        {
            emit Send_TextBrowser_NewMessage(tr("File name:[")+SourceFile_fullPath+tr("]  Scale and Denoise progress:[")+QString::number(InterPro_now,10)+"/"+QString::number(InterPro_total,10)+"]");
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
        QtConcurrent::run(this,&MainWindow::Waifu2x_Converter_Video_scale,Frame_fileName,Sub_Thread_info,&Sub_video_ThreadNumRunning);
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
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Unable to scale all frames.]"));
        status = "Failed";
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        file_DelDir(SplitFramesFolderPath);
        QFile::remove(AudioPath);
        ThreadNumRunning--;//线程数量统计-1s
        return 0;//如果启用stop位,则直接return
    }
    //======================================== 组装 ======================================================
    QString video_mp4_scaled_fullpath = "";
    if(CustRes_isEnabled)
    {
        video_mp4_scaled_fullpath = file_path+"/"+file_name+"_waifu2x_"+QString::number(CustRes_width,10)+"x"+QString::number(CustRes_height,10)+"_"+QString::number(DenoiseLevel,10)+"n"+"_"+file_ext+".mp4";
    }
    else
    {
        video_mp4_scaled_fullpath = file_path+"/"+file_name+"_waifu2x_"+QString::number(ScaleRatio,10)+"x_"+QString::number(DenoiseLevel,10)+"n"+"_"+file_ext+".mp4";
    }
    QFile::remove(video_mp4_scaled_fullpath);
    video_images2video(video_mp4_fullpath,video_mp4_scaled_fullpath,ScaledFramesFolderPath,AudioPath);
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
    //=========================== 删除转换格式生成的mp4 ==========================
    if(file_ext!="mp4")
    {
        QFile::remove(video_mp4_fullpath);
    }
    return 0;
}




int MainWindow::Waifu2x_Converter_Video_scale(QString Frame_fileName,QMap<QString,QString> Sub_Thread_info,int *Sub_video_ThreadNumRunning)
{
    QString SplitFramesFolderPath = Sub_Thread_info["SplitFramesFolderPath"];
    QString ScaledFramesFolderPath = Sub_Thread_info["ScaledFramesFolderPath"];
    QString SourceFile_fullPath = Sub_Thread_info["SourceFile_fullPath"];
    //===========
    int ScaleRatio = ui->spinBox_ScaleRatio_video->value();
    int DenoiseLevel = ui->spinBox_DenoiseLevel_video->value();
    int BlockSize = ui->spinBox_BlockSize_converter->value();
    bool DisableGPU = ui->checkBox_DisableGPU_converter->checkState();
    bool ForceOpenCL = ui->checkBox_ForceOpenCL_converter->checkState();
    QString Frame_fileFullPath = SplitFramesFolderPath+"/"+Frame_fileName;
    //========================================================================
    QProcess *Waifu2x = new QProcess();
    QString Waifu2x_folder_path = Current_Path + "/waifu2x-converter";
    QString program = Waifu2x_folder_path + "/waifu2x-converter-cpp.exe";
    QString model_path= Waifu2x_folder_path + "/models_rgb";
    QString InputPath = SplitFramesFolderPath+"/"+Frame_fileName;
    QString OutputPath = ScaledFramesFolderPath+"/"+Frame_fileName;
    QString Denoise_cmd = " --noise-level " + QString::number(DenoiseLevel, 10);
    QString DisableGPU_cmd ="";
    if(DisableGPU)DisableGPU_cmd =" --disable-gpu ";
    QString ForceOpenCL_cmd ="";
    if(ForceOpenCL)ForceOpenCL_cmd =" --force-OpenCL ";
    //======
    bool CustRes_isEnabled = false;
    int CustRes_height=0;
    int CustRes_width=0;
    if(CustRes_isContained(SourceFile_fullPath))
    {
        CustRes_isEnabled=true;
        QMap<QString, QString> Res_map = CustRes_getResMap(SourceFile_fullPath);//res_map["fullpath"],["height"],["width"]
        ScaleRatio = CustRes_CalNewScaleRatio(InputPath,Res_map["height"].toInt(),Res_map["width"].toInt());
        if(ScaleRatio==0)
        {
            emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+InputPath+tr("]. Error: [The resolution of the source file cannot be read, so the image cannot be scaled to a custom resolution.]"));
            *Sub_video_ThreadNumRunning=*Sub_video_ThreadNumRunning-1;
            return 0;
        }
        CustRes_height=Res_map["height"].toInt();
        CustRes_width=Res_map["width"].toInt();
    }
    //=======
    QString cmd = "\"" + program + "\"" + " -i " + "\"" + InputPath + "\"" + " -o " + "\"" + OutputPath + "\"" + " --scale-ratio " + QString::number(ScaleRatio, 10) + Denoise_cmd + " --model-dir " + "\"" + model_path + "\" --block-size "+QString::number(BlockSize, 10)+DisableGPU_cmd+ForceOpenCL_cmd+Processor_converter_STR;
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
    //============================ 调整大小 ====================================================
    if(CustRes_isEnabled)
    {
        QImage qimage_original;
        qimage_original.load(InputPath);
        int New_height=0;
        int New_width=0;
        if(CustRes_isEnabled)
        {
            New_height= CustRes_height;
            New_width= CustRes_width;
        }
        else
        {
            New_height = qimage_original.height()*ScaleRatio;
            New_width = qimage_original.width()*ScaleRatio;
        }
        QImage qimage_adj(OutputPath);
        //读取放大后的图片并调整大小
        if(CustRes_isEnabled)
        {
            double Original_height = qimage_original.height();
            double Original_width = qimage_original.width();
            double AspectRatio = Original_height/Original_width;
            if(CustRes_AspectRatioMode == Qt::KeepAspectRatio)
            {
                if(AspectRatio<1)
                {
                    New_height = (int)(New_width*AspectRatio);
                    if(New_height%2!=0)
                    {
                        New_height+=1;
                    }
                }
                else
                {
                    New_width = (int)(New_height/AspectRatio);
                    if(New_width%2!=0)
                    {
                        New_width+=1;
                    }
                }
            }
            if(CustRes_AspectRatioMode == Qt::KeepAspectRatioByExpanding)
            {
                if(AspectRatio<1)
                {
                    New_width = (int)(New_height/AspectRatio);
                    if(New_width%2!=0)
                    {
                        New_width+=1;
                    }
                }
                else
                {
                    New_height = (int)(New_width*AspectRatio);
                    if(New_height%2!=0)
                    {
                        New_height+=1;
                    }
                }
            }
        }
        QImage qimage_adj_scaled = qimage_adj.scaled(New_width,New_height,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        QImageWriter qimageW_adj;
        qimageW_adj.setFormat("png");
        qimageW_adj.setFileName(OutputPath);
        if(qimageW_adj.canWrite())
        {
            qimageW_adj.write(qimage_adj_scaled);
        }
    }
    QFile::remove(Frame_fileFullPath);
    *Sub_video_ThreadNumRunning=*Sub_video_ThreadNumRunning-1;
    return 0;
}


