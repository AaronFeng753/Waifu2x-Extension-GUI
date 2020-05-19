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

int MainWindow::Waifu2x_Converter_Image(int rowNum)
{
    //============================= 读取设置 ================================
    int ScaleRatio = ui->spinBox_ScaleRatio_image->value();
    int DenoiseLevel = ui->spinBox_DenoiseLevel_image->value();
    if(ui->comboBox_Engine_Image->currentIndex()==0)
    {
        if(DenoiseLevel==-1)
        {
            DenoiseLevel=0;
        }
        else
        {
            if(DenoiseLevel==0)
            {
                DenoiseLevel=1;
            }
        }
    }
    if(ui->comboBox_Engine_Image->currentIndex()==2)
    {
        if(DenoiseLevel==-1)
        {
            DenoiseLevel=0;
        }
        else
        {
            if(DenoiseLevel>=0&&DenoiseLevel<=2)
            {
                DenoiseLevel=1;
            }
            else
            {
                if(DenoiseLevel>=3&&DenoiseLevel<=6)
                {
                    DenoiseLevel=2;
                }
                else
                {
                    DenoiseLevel=3;
                }
            }
        }
    }
    if(ui->comboBox_Engine_Image->currentIndex()==3)
    {
        DenoiseLevel=1;
    }
    int BlockSize = ui->spinBox_BlockSize_converter->value();
    bool DelOriginal = ui->checkBox_DelOriginal->checkState();
    bool SaveAsJPG = ui->checkBox_SaveAsJPG->checkState();
    //======
    if(SaveAsJPG)
    {
        SaveAsJPG = !(Imgae_hasAlphaChannel(rowNum));//如果含有alpha通道则不另存为jpg
    }
    //======
    bool CompressJPG = ui->checkBox_CompressJPG->checkState();
    bool DisableGPU = ui->checkBox_DisableGPU_converter->checkState();
    bool ForceOpenCL = ui->checkBox_ForceOpenCL_converter->checkState();
    bool TTA_isEnabled = ui->checkBox_TTA_converter->checkState();
    QString OutPutPath_Final ="";
    //========================= 拆解map得到参数 =============================
    QString status = "Processing";
    emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    QString SourceFile_fullPath = Table_model_image->item(rowNum,2)->text();
    if(!file_isFileExist(SourceFile_fullPath))
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [File does not exist.]"));
        status = "Failed";
        emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        emit Send_progressbar_Add();
        mutex_ThreadNumRunning.lock();
        ThreadNumRunning--;
        mutex_ThreadNumRunning.unlock();//线程数量统计-1s
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
            emit Send_progressbar_Add();
            mutex_ThreadNumRunning.lock();
            ThreadNumRunning--;
            mutex_ThreadNumRunning.unlock();//线程数量统计-1s
            return 0;
        }
        CustRes_height=Res_map["height"].toInt();
        CustRes_width=Res_map["width"].toInt();
    }
    //=======================================================
    QFileInfo fileinfo(SourceFile_fullPath);
    QString file_name = file_getBaseName(fileinfo.filePath());
    QString file_ext = fileinfo.suffix();
    QString file_path = file_getFolderPath(fileinfo);
    QString OutPut_Path = file_path + "/" + file_name + "_waifu2x_"+QString::number(ScaleRatio, 10)+"x_"+QString::number(DenoiseLevel, 10)+"n_"+file_ext+".png";
    //============================== 放大 =======================================
    QString Waifu2x_folder_path = Current_Path + "/waifu2x-converter";
    QString program = Waifu2x_folder_path + "/waifu2x-converter-cpp_waifu2xEX.exe";
    QString model_path= Waifu2x_folder_path + "/models_rgb";
    QString Denoise_cmd = " --noise-level " + QString::number(DenoiseLevel, 10);
    QString DisableGPU_cmd ="";
    if(DisableGPU)DisableGPU_cmd =" --disable-gpu ";
    QString ForceOpenCL_cmd ="";
    if(ForceOpenCL)ForceOpenCL_cmd =" --force-OpenCL ";
    //=======
    QString TTA_cmd = " -t 0 ";
    if(TTA_isEnabled)
    {
        TTA_cmd = " -t 1 ";
    }
    //====
    QString cmd = "\"" + program + "\"" + " -i " + "\"" + SourceFile_fullPath + "\"" + " -o " + "\"" + OutPut_Path + "\"" + " --scale-ratio " + QString::number(ScaleRatio, 10) + Denoise_cmd + " --model-dir " + "\"" + model_path + "\" --block-size "+QString::number(BlockSize, 10)+DisableGPU_cmd+ForceOpenCL_cmd+Processor_converter_STR+TTA_cmd;
    //========
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
                status = "Interrupted";
                emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, status);
                mutex_ThreadNumRunning.lock();
                ThreadNumRunning--;
                mutex_ThreadNumRunning.unlock();
                return 0;
            }
        }
        if(file_isFileExist(OutPut_Path))
        {
            break;
        }
        else
        {
            Delay_sec_sleep(5);
            emit Send_TextBrowser_NewMessage(tr("Automatic retry, please wait."));
        }
    }
    //========
    if(!file_isFileExist(OutPut_Path))
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Unable to scale the picture.]"));
        status = "Failed";
        emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        emit Send_progressbar_Add();
        mutex_ThreadNumRunning.lock();
        ThreadNumRunning--;
        mutex_ThreadNumRunning.unlock();
        return 0;
    }
    OutPutPath_Final = OutPut_Path;
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
            emit Send_progressbar_Add();
            mutex_ThreadNumRunning.lock();
            ThreadNumRunning--;
            mutex_ThreadNumRunning.unlock();//线程数量统计-1s
            return 0;
        }
        OutPutPath_Final = OutPut_Path;
    }
    if(CustRes_isEnabled)
    {
        QString OutPut_Path_CustRes = file_path + "/" + file_name + "_waifu2x_"+QString::number(CustRes_width, 10)+"x"+QString::number(CustRes_height, 10)+"_"+QString::number(DenoiseLevel, 10)+"n_"+file_ext+".png";
        QFile::rename(OutPut_Path,OutPut_Path_CustRes);
        OutPut_Path = OutPut_Path_CustRes;
        OutPutPath_Final = OutPut_Path;
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
            OutPutPath_Final = OutPut_Path_jpg;
        }
        else
        {
            emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Cannot save scaled picture as .jpg.]"));
        }
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
        status = "Finished, original file deleted";
        emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    }
    else
    {
        status = "Finished";
        emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    }
    //========== 移动到输出路径 =========
    if(ui->checkBox_OutPath_isEnabled->checkState())
    {
        QFileInfo fileinfo_final(OutPutPath_Final);
        QString Final_fileName = fileinfo_final.fileName();
        file_MoveFile(OutPutPath_Final,OutPutFolder_main+"/"+Final_fileName);
    }
    //============================ 更新进度条 =================================
    emit Send_progressbar_Add();
    //=========================== 更新线程数量统计==============================
    mutex_ThreadNumRunning.lock();
    ThreadNumRunning--;
    mutex_ThreadNumRunning.unlock();//线程数量统计-1s
    return 0;
}


int MainWindow::Waifu2x_Converter_GIF(int rowNum)
{
    //============================= 读取设置 ================================
    int ScaleRatio = ui->spinBox_ScaleRatio_gif->value();
    int DenoiseLevel = ui->spinBox_DenoiseLevel_gif->value();
    bool DelOriginal = ui->checkBox_DelOriginal->checkState();
    bool OptGIF = ui->checkBox_OptGIF->checkState();
    int Sub_gif_ThreadNumRunning = 0;
    QString OutPutPath_Final ="";
    //========================= 拆解map得到参数 =============================
    QString status = "Processing";
    emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    QString SourceFile_fullPath = Table_model_gif->item(rowNum,2)->text();
    if(!file_isFileExist(SourceFile_fullPath))
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [File does not exist.]"));
        status = "Failed";
        emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        emit Send_progressbar_Add();
        mutex_ThreadNumRunning.lock();
        ThreadNumRunning--;
        mutex_ThreadNumRunning.unlock();//线程数量统计-1s
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
    QString file_path = file_getFolderPath(fileinfo);
    QString ResGIFPath = file_path + "/" + file_name + "_waifu2x_"+QString::number(ScaleRatio, 10)+"x_"+QString::number(DenoiseLevel, 10)+"n.gif";
    //=========================== 获取帧时间 ====================================
    int GIF_Duration = Gif_getDuration(SourceFile_fullPath);
    if(GIF_Duration==0)//检查是否成功获取duration
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Can't get Duration value of GIF file.]"));
        status = "Failed";
        emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        //file_DelDir(SplitFramesFolderPath);
        emit Send_progressbar_Add();
        mutex_ThreadNumRunning.lock();
        ThreadNumRunning--;
        mutex_ThreadNumRunning.unlock();//线程数量统计-1s
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
        emit Send_progressbar_Add();
        mutex_ThreadNumRunning.lock();
        ThreadNumRunning--;
        mutex_ThreadNumRunning.unlock();//线程数量统计-1s
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
    if(ui->checkBox_ShowInterPro->checkState())
    {
        emit Send_CurrentFileProgress_Start(file_name+"."+file_ext,Frame_fileName_list.size());
    }
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
        if(ui->checkBox_ShowInterPro->checkState())
        {
            emit Send_CurrentFileProgress_progressbar_Add();
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
            mutex_ThreadNumRunning.lock();
            ThreadNumRunning--;
            mutex_ThreadNumRunning.unlock();//线程数量统计-1s
            return 0;//如果启用stop位,则直接return
        }
        mutex_SubThreadNumRunning.lock();
        Sub_gif_ThreadNumRunning++;
        mutex_SubThreadNumRunning.unlock();
        QString Frame_fileName = Frame_fileName_list.at(i);
        Sub_Thread_info["Frame_fileName"]=Frame_fileName;
        QtConcurrent::run(this,&MainWindow::Waifu2x_Converter_GIF_scale,Sub_Thread_info,&Sub_gif_ThreadNumRunning,&Frame_failed);
        while (Sub_gif_ThreadNumRunning >= Sub_gif_ThreadNumMax)
        {
            Delay_msec_sleep(500);
        }
        if(Frame_failed)
        {
            emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Failed to scale frames.]"));
            status = "Failed";
            emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, status);
            file_DelDir(SplitFramesFolderPath);
            emit Send_progressbar_Add();
            mutex_ThreadNumRunning.lock();
            ThreadNumRunning--;
            mutex_ThreadNumRunning.unlock();//线程数量统计-1s
            return 0;//如果启用stop位,则直接return
        }
    }
    emit Send_CurrentFileProgress_Stop();
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
        emit Send_progressbar_Add();
        mutex_ThreadNumRunning.lock();
        ThreadNumRunning--;
        mutex_ThreadNumRunning.unlock();//线程数量统计-1s
        return 0;//如果启用stop位,则直接return
    }
    //======================================== 组装 ======================================================
    if(CustRes_isEnabled)
    {
        ResGIFPath = file_path + "/" + file_name + "_waifu2x_"+QString::number(CustRes_width, 10)+"x"+QString::number(CustRes_height,10)+"_"+QString::number(DenoiseLevel, 10)+"n.gif";
    }
    Gif_assembleGif(ResGIFPath,ScaledFramesFolderPath,GIF_Duration,CustRes_isEnabled,CustRes_height,CustRes_width);
    if(!file_isFileExist(ResGIFPath))
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Unable to assemble gif.]"));
        status = "Failed";
        emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        file_DelDir(SplitFramesFolderPath);
        emit Send_progressbar_Add();
        mutex_ThreadNumRunning.lock();
        ThreadNumRunning--;
        mutex_ThreadNumRunning.unlock();//线程数量统计-1s
        return 0;//如果启用stop位,则直接return
    }
    OutPutPath_Final = ResGIFPath;
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
            OutPutPath_Final = ResGIFPath_compressed;
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
        if(ui->checkBox_Move2RecycleBin->checkState())
        {
            file_MoveToTrash(SourceFile_fullPath);
        }
        else
        {
            QFile::remove(SourceFile_fullPath);
        }
        status = "Finished, original file deleted";
        emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    }
    else
    {
        status = "Finished";
        emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    }
    //========== 移动到输出路径 =========
    if(ui->checkBox_OutPath_isEnabled->checkState())
    {
        QFileInfo fileinfo_final(OutPutPath_Final);
        QString Final_fileName = fileinfo_final.fileName();
        file_MoveFile(OutPutPath_Final,OutPutFolder_main+"/"+Final_fileName);
    }
    //============================ 更新进度条 =================================
    emit Send_progressbar_Add();
    //=========================== 更新filelist ==============================
    mutex_ThreadNumRunning.lock();
    ThreadNumRunning--;
    mutex_ThreadNumRunning.unlock();//线程数量统计-1s
    return 0;
}

int MainWindow::Waifu2x_Converter_GIF_scale(QMap<QString, QString> Sub_Thread_info,int *Sub_gif_ThreadNumRunning,bool *Frame_failed)
{
    QString SplitFramesFolderPath = Sub_Thread_info["SplitFramesFolderPath"];
    QString ScaledFramesFolderPath = Sub_Thread_info["ScaledFramesFolderPath"];
    QString SourceFile_fullPath = Sub_Thread_info["SourceFile_fullPath"];
    QString Frame_fileName = Sub_Thread_info["Frame_fileName"];
    //===========
    int ScaleRatio = ui->spinBox_ScaleRatio_gif->value();
    int DenoiseLevel = ui->spinBox_DenoiseLevel_gif->value();
    int BlockSize = ui->spinBox_BlockSize_converter->value();
    bool DisableGPU = ui->checkBox_DisableGPU_converter->checkState();
    bool ForceOpenCL = ui->checkBox_ForceOpenCL_converter->checkState();
    bool TTA_isEnabled = ui->checkBox_TTA_converter->checkState();
    //========================================================================
    QString Waifu2x_folder_path = Current_Path + "/waifu2x-converter";
    QString program = Waifu2x_folder_path + "/waifu2x-converter-cpp_waifu2xEX.exe";
    QString model_path= Waifu2x_folder_path + "/models_rgb";
    QString InputPath = SplitFramesFolderPath+"/"+Frame_fileName;
    QString OutputPath = ScaledFramesFolderPath+"/"+Frame_fileName;
    QString Denoise_cmd = " --noise-level " + QString::number(DenoiseLevel, 10);
    QString DisableGPU_cmd ="";
    if(DisableGPU)DisableGPU_cmd =" --disable-gpu ";
    QString ForceOpenCL_cmd ="";
    if(ForceOpenCL)ForceOpenCL_cmd =" --force-OpenCL ";
    //======
    if(CustRes_isContained(SourceFile_fullPath))
    {
        QMap<QString, QString> Res_map = CustRes_getResMap(SourceFile_fullPath);//res_map["fullpath"],["height"],["width"]
        ScaleRatio = CustRes_CalNewScaleRatio(InputPath,Res_map["height"].toInt(),Res_map["width"].toInt());
        if(ScaleRatio==0)
        {
            emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+InputPath+tr("]. Error: [The resolution of the source file cannot be read, so the image cannot be scaled to a custom resolution.]"));
            mutex_SubThreadNumRunning.lock();
            *Sub_gif_ThreadNumRunning=*Sub_gif_ThreadNumRunning-1;
            mutex_SubThreadNumRunning.unlock();
            return 0;
        }
    }
    //=======
    QString TTA_cmd = " -t 0 ";
    if(TTA_isEnabled)
    {
        TTA_cmd = " -t 1 ";
    }
    //=======
    for(int retry=0; retry<(ui->spinBox_retry->value()); retry++)
    {
        QString cmd = "\"" + program + "\"" + " -i " + "\"" + InputPath + "\"" + " -o " + "\"" + OutputPath + "\"" + " --scale-ratio " + QString::number(ScaleRatio, 10) + Denoise_cmd + " --model-dir " + "\"" + model_path + "\" --block-size "+QString::number(BlockSize, 10)+DisableGPU_cmd+ForceOpenCL_cmd+Processor_converter_STR+TTA_cmd;
        QProcess *Waifu2x = new QProcess();
        Waifu2x->start(cmd);
        while(!Waifu2x->waitForStarted(100)&&!QProcess_stop) {}
        while(!Waifu2x->waitForFinished(500)&&!QProcess_stop)
        {
            if(waifu2x_STOP)
            {
                Waifu2x->close();
                mutex_SubThreadNumRunning.lock();
                *Sub_gif_ThreadNumRunning=*Sub_gif_ThreadNumRunning-1;
                mutex_SubThreadNumRunning.unlock();
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
            emit Send_TextBrowser_NewMessage(tr("Automatic retry, please wait."));
        }
    }
    //========================
    QFile::remove(InputPath);
    if(file_isFileExist(OutputPath)==false)*Frame_failed=true;
    //=======
    mutex_SubThreadNumRunning.lock();
    *Sub_gif_ThreadNumRunning=*Sub_gif_ThreadNumRunning-1;
    mutex_SubThreadNumRunning.unlock();
    //========
    return 0;
}
/*
正常處理視頻(不分段)
*/
int MainWindow::Waifu2x_Converter_Video(int rowNum)
{
    //============================= 读取设置 ================================
    int ScaleRatio = ui->spinBox_ScaleRatio_video->value();
    int DenoiseLevel = ui->spinBox_DenoiseLevel_video->value();
    bool DelOriginal = ui->checkBox_DelOriginal->checkState();
    bool isCacheExists = false;
    bool isVideoConfigChanged = true;
    int Sub_video_ThreadNumRunning = 0;
    QString OutPutPath_Final ="";
    //========================= 拆解map得到参数 =============================
    QString status = "Processing";
    emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    QString SourceFile_fullPath = Table_model_video->item(rowNum,2)->text();
    if(!file_isFileExist(SourceFile_fullPath))
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [File does not exist.]"));
        status = "Failed";
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        emit Send_progressbar_Add();
        mutex_ThreadNumRunning.lock();
        ThreadNumRunning--;
        mutex_ThreadNumRunning.unlock();//线程数量统计-1s
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
    QString file_path = file_getFolderPath(fileinfo);
    //===================================================================
    QString video_mp4_fullpath="";//mp4路径
    if(file_ext!="mp4")
    {
        video_mp4_fullpath = file_path+"/"+file_name+"_"+file_ext+".mp4";
    }
    else
    {
        video_mp4_fullpath = file_path+"/"+file_name+".mp4";
    }
    QString AudioPath = file_path+"/audio_"+file_name+"_"+file_ext+"_waifu2x.wav";//音频
    QString SplitFramesFolderPath = file_path+"/"+file_name+"_"+file_ext+"_splitFrames_waifu2x";//拆分后存储frame的文件夹
    QString ScaledFramesFolderPath = SplitFramesFolderPath+"/scaled";//存储放大后的帧
    //==========================
    //   检测之前的视频配置文件
    //==========================
    QString VideoConfiguration_fullPath = file_path+"/VideoConfiguration_"+file_name+"_"+file_ext+"_Waifu2xEX.ini";
    if(file_isFileExist(VideoConfiguration_fullPath))
    {
        QSettings *configIniRead = new QSettings(VideoConfiguration_fullPath, QSettings::IniFormat);
        configIniRead->setIniCodec(QTextCodec::codecForName("UTF-8"));
        //=================== 加载之前存储的视频信息 =========================
        int ScaleRatio_old = configIniRead->value("/VideoConfiguration/ScaleRatio").toInt();
        int DenoiseLevel_old = configIniRead->value("/VideoConfiguration/DenoiseLevel").toInt();
        bool CustRes_isEnabled_old = configIniRead->value("/VideoConfiguration/CustRes_isEnabled").toBool();
        int CustRes_height_old = configIniRead->value("/VideoConfiguration/CustRes_height").toInt();
        int CustRes_width_old = configIniRead->value("/VideoConfiguration/CustRes_width").toInt();
        QString EngineName_old = configIniRead->value("/VideoConfiguration/EngineName").toString();
        bool isProcessBySegment_old = configIniRead->value("/VideoConfiguration/isProcessBySegment").toBool();
        //=================== 比对信息 ================================
        if(EngineName_old=="waifu2x-converter")
        {
            if(CustRes_isEnabled_old==false&&CustRes_isEnabled==false)
            {
                if(ScaleRatio_old!=ScaleRatio||DenoiseLevel_old!=DenoiseLevel)
                {
                    isVideoConfigChanged=true;
                }
                else
                {
                    isVideoConfigChanged=false;
                }
            }
            else
            {
                if(CustRes_isEnabled_old==true&&CustRes_isEnabled==true)
                {
                    if(CustRes_height_old!=CustRes_height||CustRes_width_old!=CustRes_width)
                    {
                        isVideoConfigChanged=true;
                    }
                    else
                    {
                        isVideoConfigChanged=false;
                    }
                }
                else
                {
                    isVideoConfigChanged=true;
                }
            }
            if(isProcessBySegment_old==true)
            {
                isVideoConfigChanged=true;
            }
        }
        else
        {
            isVideoConfigChanged=true;
        }
    }
    else
    {
        emit Send_video_write_VideoConfiguration(VideoConfiguration_fullPath,ScaleRatio,DenoiseLevel,CustRes_isEnabled,CustRes_height,CustRes_width,"waifu2x-converter",false,"","");
    }
    //=======================
    //   检测缓存是否存在
    //=======================
    if(file_isFileExist(video_mp4_fullpath)&&file_isDirExist(SplitFramesFolderPath)&&file_isDirExist(ScaledFramesFolderPath)&&file_isFileExist(VideoConfiguration_fullPath))
    {
        if(!isVideoConfigChanged)
        {
            isCacheExists=true;
            emit Send_TextBrowser_NewMessage(tr("The previous video cache file is detected and processing of the previous video cache will continue. If you want to restart processing of the current video:[")+SourceFile_fullPath+tr("], delete the cache manually."));
        }
        else
        {
            isCacheExists=false;
            //========
            QFile::remove(VideoConfiguration_fullPath);
            file_DelDir(SplitFramesFolderPath);
            QFile::remove(AudioPath);
            if(file_ext!="mp4")
            {
                if(SourceFile_fullPath!=video_mp4_fullpath)QFile::remove(video_mp4_fullpath);
            }
            emit Send_video_write_VideoConfiguration(VideoConfiguration_fullPath,ScaleRatio,DenoiseLevel,CustRes_isEnabled,CustRes_height,CustRes_width,"waifu2x-converter",false,"","");
            //=======
            emit Send_TextBrowser_NewMessage(tr("The previous video cache file was detected, but because you changed the settings about the video resolution or denoise level, the previous cache will be deleted and processing of the video:[")+SourceFile_fullPath+tr("] will restart."));
        }
    }
    else
    {
        isCacheExists=false;
        //========
        QFile::remove(VideoConfiguration_fullPath);
        file_DelDir(SplitFramesFolderPath);
        QFile::remove(AudioPath);
        if(file_ext!="mp4")
        {
            if(SourceFile_fullPath!=video_mp4_fullpath)QFile::remove(video_mp4_fullpath);
        }
        emit Send_video_write_VideoConfiguration(VideoConfiguration_fullPath,ScaleRatio,DenoiseLevel,CustRes_isEnabled,CustRes_height,CustRes_width,"waifu2x-converter",false,"","");
        //========
    }
    if(!isCacheExists)
    {
        //============================== 拆分 ==========================================
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
            emit Send_progressbar_Add();
            mutex_ThreadNumRunning.lock();
            ThreadNumRunning--;
            mutex_ThreadNumRunning.unlock();//线程数量统计-1s
            return 0;//如果启用stop位,则直接return
        }
    }
    //============================== 扫描获取文件名 ===============================
    QStringList Frame_fileName_list = file_getFileNames_in_Folder_nofilter(SplitFramesFolderPath);
    if(!isCacheExists)
    {
        if(Frame_fileName_list.isEmpty())//检查是否成功拆分为帧
        {
            emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Unable to split video into pictures.]"));
            status = "Failed";
            emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
            emit Send_progressbar_Add();
            mutex_ThreadNumRunning.lock();
            ThreadNumRunning--;
            mutex_ThreadNumRunning.unlock();//线程数量统计-1s
            return 0;//如果启用stop位,则直接return
        }
    }
    //============================== 放大 =======================================
    //===========建立存储放大后frame的文件夹===========
    if(!isCacheExists)
    {
        if(file_isDirExist(ScaledFramesFolderPath))
        {
            file_DelDir(ScaledFramesFolderPath);
            file_mkDir(ScaledFramesFolderPath);
        }
        else
        {
            file_mkDir(ScaledFramesFolderPath);
        }
    }
    //==========开始放大==========================
    if(ui->checkBox_ShowInterPro->checkState())
    {
        emit Send_CurrentFileProgress_Start(file_name+"."+file_ext,Frame_fileName_list.size());
    }
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
        if(ui->checkBox_ShowInterPro->checkState())
        {
            emit Send_CurrentFileProgress_progressbar_Add();
        }
        int Sub_video_ThreadNumMax = ui->spinBox_ThreadNum_video_internal->value();
        if(waifu2x_STOP)
        {
            while (Sub_video_ThreadNumRunning > 0)
            {
                Delay_msec_sleep(500);
            }
            status = "Interrupted";
            emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
            mutex_ThreadNumRunning.lock();
            ThreadNumRunning--;
            mutex_ThreadNumRunning.unlock();//线程数量统计-1s
            return 0;//如果启用stop位,则直接return
        }
        mutex_SubThreadNumRunning.lock();
        Sub_video_ThreadNumRunning++;
        mutex_SubThreadNumRunning.unlock();
        QString Frame_fileName = Frame_fileName_list.at(i);
        Sub_Thread_info["Frame_fileName"]=Frame_fileName;
        QtConcurrent::run(this,&MainWindow::Waifu2x_Converter_Video_scale,Sub_Thread_info,&Sub_video_ThreadNumRunning,&Frame_failed);
        while (Sub_video_ThreadNumRunning >= Sub_video_ThreadNumMax)
        {
            Delay_msec_sleep(500);
        }
        if(Frame_failed)
        {
            emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Unable to scale all frames.]"));
            status = "Failed";
            emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
            emit Send_progressbar_Add();
            mutex_ThreadNumRunning.lock();
            ThreadNumRunning--;
            mutex_ThreadNumRunning.unlock();//线程数量统计-1s
            return 0;//如果启用stop位,则直接return
        }
    }
    emit Send_CurrentFileProgress_Stop();
    while (Sub_video_ThreadNumRunning!=0)
    {
        Delay_msec_sleep(500);
    }
    //================ 扫描放大后的帧文件数量,判断是否放大成功 =======================
    QStringList Frame_fileName_list_scaled = file_getFileNames_in_Folder_nofilter(ScaledFramesFolderPath);
    if(Frame_fileName_list.count()>Frame_fileName_list_scaled.count())
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Unable to scale all frames.]"));
        status = "Failed";
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        emit Send_progressbar_Add();
        mutex_ThreadNumRunning.lock();
        ThreadNumRunning--;
        mutex_ThreadNumRunning.unlock();//线程数量统计-1s
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
    video_images2video(video_mp4_fullpath,video_mp4_scaled_fullpath,ScaledFramesFolderPath,AudioPath,CustRes_isEnabled,CustRes_height,CustRes_width);
    if(!file_isFileExist(video_mp4_scaled_fullpath))//检查是否成功成功生成视频
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Unable to assemble pictures into videos.]"));
        status = "Failed";
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        emit Send_progressbar_Add();
        mutex_ThreadNumRunning.lock();
        ThreadNumRunning--;
        mutex_ThreadNumRunning.unlock();//线程数量统计-1s
        return 0;//如果启用stop位,则直接return
    }
    OutPutPath_Final = video_mp4_scaled_fullpath;
    //============================== 删除缓存文件 ====================================================
    if(!ui->checkBox_KeepVideoCache->checkState())
    {
        QFile::remove(VideoConfiguration_fullPath);
        file_DelDir(SplitFramesFolderPath);
        QFile::remove(AudioPath);
        if(file_ext!="mp4")
        {
            if(SourceFile_fullPath!=video_mp4_fullpath)QFile::remove(video_mp4_fullpath);
        }
    }
    else
    {
        DelOriginal=false;
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
        status = "Finished, original file deleted";
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    }
    else
    {
        status = "Finished";
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    }
    //========== 移动到输出路径 =========
    if(ui->checkBox_OutPath_isEnabled->checkState())
    {
        QFileInfo fileinfo_final(OutPutPath_Final);
        QString Final_fileName = fileinfo_final.fileName();
        file_MoveFile(OutPutPath_Final,OutPutFolder_main+"/"+Final_fileName);
    }
    //============================ 更新进度条 =================================
    emit Send_progressbar_Add();
    //=========================== 更新filelist ==============================
    mutex_ThreadNumRunning.lock();
    ThreadNumRunning--;
    mutex_ThreadNumRunning.unlock();//线程数量统计-1s
    return 0;
}

/*
#### 分段处理 ####
调用Waifu2x-Converter处理视频
读取设置,拆分,创建放大子线程,组装视频
*/
int MainWindow::Waifu2x_Converter_Video_BySegment(int rowNum)
{
    //============================= 读取设置 ================================
    int ScaleRatio = ui->spinBox_ScaleRatio_video->value();
    int DenoiseLevel = ui->spinBox_DenoiseLevel_video->value();
    bool DelOriginal = ui->checkBox_DelOriginal->checkState();
    bool isCacheExists = false;
    bool isVideoConfigChanged = true;
    int Sub_video_ThreadNumRunning = 0;
    QString OutPutPath_Final ="";
    int SegmentDuration = ui->spinBox_SegmentDuration->value();
    //========================= 拆解map得到参数 =============================
    QString status = "Processing";
    emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    QString SourceFile_fullPath = Table_model_video->item(rowNum,2)->text();
    if(!file_isFileExist(SourceFile_fullPath))
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [File does not exist.]"));
        status = "Failed";
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        emit Send_progressbar_Add();
        mutex_ThreadNumRunning.lock();
        ThreadNumRunning--;
        mutex_ThreadNumRunning.unlock();//线程数量统计-1s
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
    QString file_path = file_getFolderPath(fileinfo);
    //===================================================================
    QString video_mp4_fullpath="";//mp4路径
    if(file_ext!="mp4")
    {
        video_mp4_fullpath = file_path+"/"+file_name+"_"+file_ext+".mp4";
    }
    else
    {
        video_mp4_fullpath = file_path+"/"+file_name+".mp4";
    }
    QString AudioPath = file_path+"/audio_"+file_name+"_"+file_ext+"_waifu2x.wav";//音频
    QString SplitFramesFolderPath = file_path+"/"+file_name+"_"+file_ext+"_splitFrames_waifu2x";//拆分后存储frame的文件夹
    QString ScaledFramesFolderPath = SplitFramesFolderPath+"/scaled";//存储放大后的帧
    //===
    QString VideoClipsFolderPath = "";//存储视频片段的文件夹(完整路径)
    QString DateStr = "";
    do
    {
        DateStr = video_getClipsFolderNo();
        VideoClipsFolderPath = file_path+"/"+DateStr+"_VideoClipsWaifu2xEX";//存储视频片段的文件夹(完整路径)
    }
    while(file_isDirExist(VideoClipsFolderPath));
    QString VideoClipsFolderName = DateStr+"_VideoClipsWaifu2xEX";//存储视频片段的文件夹(名称)
    //==========================
    //   检测之前的视频配置文件
    //==========================
    QString VideoConfiguration_fullPath = file_path+"/VideoConfiguration_"+file_name+"_"+file_ext+"_Waifu2xEX.ini";
    if(file_isFileExist(VideoConfiguration_fullPath))
    {
        QSettings *configIniRead = new QSettings(VideoConfiguration_fullPath, QSettings::IniFormat);
        configIniRead->setIniCodec(QTextCodec::codecForName("UTF-8"));
        //=================== 加载之前存储的视频信息 =========================
        int ScaleRatio_old = configIniRead->value("/VideoConfiguration/ScaleRatio").toInt();
        int DenoiseLevel_old = configIniRead->value("/VideoConfiguration/DenoiseLevel").toInt();
        bool CustRes_isEnabled_old = configIniRead->value("/VideoConfiguration/CustRes_isEnabled").toBool();
        int CustRes_height_old = configIniRead->value("/VideoConfiguration/CustRes_height").toInt();
        int CustRes_width_old = configIniRead->value("/VideoConfiguration/CustRes_width").toInt();
        QString EngineName_old = configIniRead->value("/VideoConfiguration/EngineName").toString();
        bool isProcessBySegment_old = configIniRead->value("/VideoConfiguration/isProcessBySegment").toBool();
        //=================== 比对信息 ================================
        if(EngineName_old=="waifu2x-converter")
        {
            if(CustRes_isEnabled_old==false&&CustRes_isEnabled==false)
            {
                if(ScaleRatio_old!=ScaleRatio||DenoiseLevel_old!=DenoiseLevel)
                {
                    isVideoConfigChanged=true;
                }
                else
                {
                    isVideoConfigChanged=false;
                }
            }
            else
            {
                if(CustRes_isEnabled_old==true&&CustRes_isEnabled==true)
                {
                    if(CustRes_height_old!=CustRes_height||CustRes_width_old!=CustRes_width)
                    {
                        isVideoConfigChanged=true;
                    }
                    else
                    {
                        isVideoConfigChanged=false;
                    }
                }
                else
                {
                    isVideoConfigChanged=true;
                }
            }
            if(isProcessBySegment_old == false)
            {
                isVideoConfigChanged=true;
            }
        }
        else
        {
            isVideoConfigChanged=true;
        }
        //============ 修正文件夹名称 =============
        QString VideoClipsFolderPath_old = configIniRead->value("/VideoConfiguration/VideoClipsFolderPath").toString();
        QString VideoClipsFolderName_old = configIniRead->value("/VideoConfiguration/VideoClipsFolderName").toString();
        if(file_isDirExist(VideoClipsFolderPath_old)==true)
        {
            VideoClipsFolderPath = VideoClipsFolderPath_old;
            VideoClipsFolderName = VideoClipsFolderName_old;
        }
    }
    else
    {
        emit Send_video_write_VideoConfiguration(VideoConfiguration_fullPath,ScaleRatio,DenoiseLevel,CustRes_isEnabled,CustRes_height,CustRes_width,"waifu2x-converter",true,VideoClipsFolderPath,VideoClipsFolderName);
    }
    //=======================
    //   检测缓存是否存在
    //=======================
    if(file_isFileExist(video_mp4_fullpath)&&file_isDirExist(SplitFramesFolderPath)&&file_isFileExist(AudioPath))
    {
        if(!isVideoConfigChanged)
        {
            isCacheExists=true;
            emit Send_TextBrowser_NewMessage(tr("The previous video cache file is detected and processing of the previous video cache will continue. If you want to restart processing of the current video:[")+SourceFile_fullPath+tr("], delete the cache manually."));
        }
        else
        {
            isCacheExists=false;
            //========
            QFile::remove(VideoConfiguration_fullPath);
            file_DelDir(SplitFramesFolderPath);
            file_DelDir(VideoClipsFolderPath);
            QFile::remove(AudioPath);
            if(file_ext!="mp4")
            {
                if(SourceFile_fullPath!=video_mp4_fullpath)QFile::remove(video_mp4_fullpath);
            }
            emit Send_video_write_VideoConfiguration(VideoConfiguration_fullPath,ScaleRatio,DenoiseLevel,CustRes_isEnabled,CustRes_height,CustRes_width,"waifu2x-converter",true,VideoClipsFolderPath,VideoClipsFolderName);
            //=======
            emit Send_TextBrowser_NewMessage(tr("The previous video cache file was detected, but because you changed the settings about the video resolution or denoise level, the previous cache will be deleted and processing of the video:[")+SourceFile_fullPath+tr("] will restart."));
        }
    }
    else
    {
        isCacheExists=false;
        //========
        QFile::remove(VideoConfiguration_fullPath);
        file_DelDir(SplitFramesFolderPath);
        file_DelDir(VideoClipsFolderPath);
        QFile::remove(AudioPath);
        if(file_ext!="mp4")
        {
            if(SourceFile_fullPath!=video_mp4_fullpath)QFile::remove(video_mp4_fullpath);
        }
        emit Send_video_write_VideoConfiguration(VideoConfiguration_fullPath,ScaleRatio,DenoiseLevel,CustRes_isEnabled,CustRes_height,CustRes_width,"waifu2x-converter",true,VideoClipsFolderPath,VideoClipsFolderName);
        //========
    }
    /*====================================
                将视频转换为MP4
    ======================================*/
    if(!isCacheExists)
    {
        video_2mp4(SourceFile_fullPath);
        if(!file_isFileExist(video_mp4_fullpath))//检查是否成功生成mp4
        {
            emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Cannot convert video format to mp4.]"));
            status = "Failed";
            emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
            emit Send_progressbar_Add();
            mutex_ThreadNumRunning.lock();
            ThreadNumRunning--;
            mutex_ThreadNumRunning.unlock();//线程数量统计-1s
            return 0;//如果启用stop位,则直接return
        }
    }
    /*====================================
                  提取音频
    ======================================*/
    if(!isCacheExists)
    {
        video_get_audio(video_mp4_fullpath,AudioPath);
    }
    //================================== 开始分段处理视频 =================================================
    int StartTime = 0;//起始时间(秒)
    int VideoDuration = video_get_duration(video_mp4_fullpath);
    if(ui->checkBox_ShowInterPro->checkState()&&VideoDuration>SegmentDuration)
    {
        emit Send_CurrentFileProgress_Start(file_name+"."+file_ext,VideoDuration);
    }
    bool isSplitComplete = false;
    bool isScaleComplete = false;
    /*
    ============================================
                  开始之前先读取进度
    ============================================
    */
    if(file_isFileExist(VideoConfiguration_fullPath))
    {
        QSettings *configIniRead = new QSettings(VideoConfiguration_fullPath, QSettings::IniFormat);
        configIniRead->setIniCodec(QTextCodec::codecForName("UTF-8"));
        //=================== 加载进度 =========================
        StartTime = configIniRead->value("/Progress/StartTime").toInt();
        isSplitComplete = configIniRead->value("/Progress/isSplitComplete").toBool();
        isScaleComplete = configIniRead->value("/Progress/isScaleComplete").toBool();
    }
    /*
    ============================================
                    正式开始处理
    ============================================
    */
    while(VideoDuration>StartTime)
    {
        /*==========================
                计算视频片段时间
        ==========================*/
        int SegmentDuration_tmp=0;
        int TimeLeft_tmp = VideoDuration-StartTime;
        if((TimeLeft_tmp)>=SegmentDuration)
        {
            SegmentDuration_tmp = SegmentDuration;
        }
        else
        {
            SegmentDuration_tmp = TimeLeft_tmp;
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
                    status = "Failed";
                    emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
                    emit Send_progressbar_Add();
                    mutex_ThreadNumRunning.lock();
                    ThreadNumRunning--;
                    mutex_ThreadNumRunning.unlock();//线程数量统计-1s
                    return 0;//如果启用stop位,则直接return
                }
            }
            /*
            记录进度
            帧拆分成功
            */
            emit Send_video_write_Progress_ProcessBySegment(VideoConfiguration_fullPath,StartTime,true,false);
            //============================== 放大 =======================================
            //===========建立存储放大后frame的文件夹===========
            if(isSplitComplete==false)
            {
                if(file_isDirExist(ScaledFramesFolderPath))
                {
                    file_DelDir(ScaledFramesFolderPath);
                    file_mkDir(ScaledFramesFolderPath);
                }
                else
                {
                    file_mkDir(ScaledFramesFolderPath);
                }
            }
            //==========开始放大==========================
            int InterPro_total = 0;
            int InterPro_now = 0;
            if(ui->checkBox_ShowInterPro->checkState())
            {
                InterPro_total = Frame_fileName_list.size();
            }
            //===============
            QMap<QString,QString> Sub_Thread_info;
            Sub_Thread_info["SplitFramesFolderPath"]=SplitFramesFolderPath;
            Sub_Thread_info["ScaledFramesFolderPath"]=ScaledFramesFolderPath;
            Sub_Thread_info["SourceFile_fullPath"]=SourceFile_fullPath;
            //===============
            bool Frame_failed = false;
            //===============
            for(int i = 0; i < Frame_fileName_list.size(); i++)
            {
                if(ui->checkBox_ShowInterPro->checkState())
                {
                    InterPro_now++;
                    emit Send_TextBrowser_NewMessage(tr("File name:[")+SourceFile_fullPath+tr("]  Scale and Denoise progress:[")+QString::number(InterPro_now,10)+"/"+QString::number(InterPro_total,10)+tr("] Duration progress:[")+QString::number(StartTime,10)+"s/"+QString::number(VideoDuration,10)+"s]");
                }
                int Sub_video_ThreadNumMax = ui->spinBox_ThreadNum_video_internal->value();
                if(waifu2x_STOP)
                {
                    while (Sub_video_ThreadNumRunning > 0)
                    {
                        Delay_msec_sleep(500);
                    }
                    status = "Interrupted";
                    emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
                    mutex_ThreadNumRunning.lock();
                    ThreadNumRunning--;
                    mutex_ThreadNumRunning.unlock();//线程数量统计-1s
                    return 0;//如果启用stop位,则直接return
                }
                mutex_SubThreadNumRunning.lock();
                Sub_video_ThreadNumRunning++;
                mutex_SubThreadNumRunning.unlock();
                QString Frame_fileName = Frame_fileName_list.at(i);
                Sub_Thread_info["Frame_fileName"]=Frame_fileName;
                QtConcurrent::run(this,&MainWindow::Waifu2x_Converter_Video_scale,Sub_Thread_info,&Sub_video_ThreadNumRunning,&Frame_failed);
                while (Sub_video_ThreadNumRunning >= Sub_video_ThreadNumMax)
                {
                    Delay_msec_sleep(500);
                }
                if(Frame_failed)
                {
                    emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Unable to scale all frames.]"));
                    status = "Failed";
                    emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
                    emit Send_progressbar_Add();
                    mutex_ThreadNumRunning.lock();
                    ThreadNumRunning--;
                    mutex_ThreadNumRunning.unlock();//线程数量统计-1s
                    return 0;//如果启用stop位,则直接return
                }
            }
            while (Sub_video_ThreadNumRunning!=0)
            {
                Delay_msec_sleep(500);
            }
            //================ 扫描放大后的帧文件数量,判断是否放大成功 =======================
            QStringList Frame_fileName_list_scaled = file_getFileNames_in_Folder_nofilter(ScaledFramesFolderPath);
            if(Frame_fileName_list_scaled.count()<Frame_fileName_list.count())
            {
                emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Unable to scale all frames.]"));
                status = "Failed";
                emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
                emit Send_progressbar_Add();
                mutex_ThreadNumRunning.lock();
                ThreadNumRunning--;
                mutex_ThreadNumRunning.unlock();//线程数量统计-1s
                return 0;//如果启用stop位,则直接return
            }
            /*
            记录进度
            帧处理成功
            */
            emit Send_video_write_Progress_ProcessBySegment(VideoConfiguration_fullPath,StartTime,true,true);
        }
        /*==========================
            组装视频片段(由帧到视频)
        ==========================*/
        if(!file_isDirExist(VideoClipsFolderPath))
        {
            file_mkDir(VideoClipsFolderPath);
        }
        int VideoClipNo = (StartTime+SegmentDuration)/SegmentDuration;
        QString video_mp4_scaled_clip_fullpath = VideoClipsFolderPath+"/"+QString::number(VideoClipNo,10)+".mp4";
        QFile::remove(video_mp4_scaled_clip_fullpath);
        video_images2video(video_mp4_fullpath,video_mp4_scaled_clip_fullpath,ScaledFramesFolderPath,"",CustRes_isEnabled,CustRes_height,CustRes_width);
        if(!file_isFileExist(video_mp4_scaled_clip_fullpath))//检查是否成功成功生成视频
        {
            emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Unable to assemble pictures into videos.]"));
            status = "Failed";
            emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
            emit Send_progressbar_Add();
            mutex_ThreadNumRunning.lock();
            ThreadNumRunning--;
            mutex_ThreadNumRunning.unlock();//线程数量统计-1s
            return 0;//如果启用stop位,则直接return
        }
        /*==========================
        处理完当前片段,时间后移,记录起始时间
        ==========================*/
        if(ui->checkBox_ShowInterPro->checkState())
        {
            emit Send_CurrentFileProgress_progressbar_Add_SegmentDuration(SegmentDuration_tmp);
        }
        StartTime+=SegmentDuration_tmp;
        isSplitComplete = false;
        isScaleComplete = false;
        emit Send_video_write_Progress_ProcessBySegment(VideoConfiguration_fullPath,StartTime,false,false);
    }
    emit Send_CurrentFileProgress_Stop();
    //======================================================
    //                    组装(片段到成片)
    //======================================================
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
    video_AssembleVideoClips(VideoClipsFolderPath,VideoClipsFolderName,video_mp4_scaled_fullpath,AudioPath);
    if(!file_isFileExist(video_mp4_scaled_fullpath))//检查是否成功生成视频
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Unable to assemble video clips.]"));
        status = "Failed";
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        emit Send_progressbar_Add();
        mutex_ThreadNumRunning.lock();
        ThreadNumRunning--;
        mutex_ThreadNumRunning.unlock();//线程数量统计-1s
        return 0;//如果启用stop位,则直接return
    }
    OutPutPath_Final = video_mp4_scaled_fullpath;
    //============================== 删除缓存文件 ====================================================
    if(!ui->checkBox_KeepVideoCache->checkState())
    {
        QFile::remove(VideoConfiguration_fullPath);
        file_DelDir(SplitFramesFolderPath);
        file_DelDir(VideoClipsFolderPath);
        QFile::remove(AudioPath);
        if(file_ext!="mp4")
        {
            if(SourceFile_fullPath!=video_mp4_fullpath)QFile::remove(video_mp4_fullpath);
        }
    }
    else
    {
        DelOriginal=false;
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
        status = "Finished, original file deleted";
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    }
    else
    {
        status = "Finished";
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    }
    //========== 移动到输出路径 =========
    if(ui->checkBox_OutPath_isEnabled->checkState())
    {
        QFileInfo fileinfo_final(OutPutPath_Final);
        QString Final_fileName = fileinfo_final.fileName();
        file_MoveFile(OutPutPath_Final,OutPutFolder_main+"/"+Final_fileName);
    }
    //============================ 更新进度条 =================================
    emit Send_progressbar_Add();
    //=========================== 更新filelist ==============================
    mutex_ThreadNumRunning.lock();
    ThreadNumRunning--;
    mutex_ThreadNumRunning.unlock();//线程数量统计-1s
    return 0;
}


int MainWindow::Waifu2x_Converter_Video_scale(QMap<QString,QString> Sub_Thread_info,int *Sub_video_ThreadNumRunning,bool *Frame_failed)
{
    QString SplitFramesFolderPath = Sub_Thread_info["SplitFramesFolderPath"];
    QString ScaledFramesFolderPath = Sub_Thread_info["ScaledFramesFolderPath"];
    QString SourceFile_fullPath = Sub_Thread_info["SourceFile_fullPath"];
    QString Frame_fileName = Sub_Thread_info["Frame_fileName"];
    //===========
    int ScaleRatio = ui->spinBox_ScaleRatio_video->value();
    int DenoiseLevel = ui->spinBox_DenoiseLevel_video->value();
    int BlockSize = ui->spinBox_BlockSize_converter->value();
    bool DisableGPU = ui->checkBox_DisableGPU_converter->checkState();
    bool ForceOpenCL = ui->checkBox_ForceOpenCL_converter->checkState();
    bool TTA_isEnabled = ui->checkBox_TTA_converter->checkState();
    QString Frame_fileFullPath = SplitFramesFolderPath+"/"+Frame_fileName;
    //========================================================================
    QString Waifu2x_folder_path = Current_Path + "/waifu2x-converter";
    QString program = Waifu2x_folder_path + "/waifu2x-converter-cpp_waifu2xEX.exe";
    QString model_path= Waifu2x_folder_path + "/models_rgb";
    QString InputPath = SplitFramesFolderPath+"/"+Frame_fileName;
    QString OutputPath = ScaledFramesFolderPath+"/"+Frame_fileName;
    QString Denoise_cmd = " --noise-level " + QString::number(DenoiseLevel, 10);
    QString DisableGPU_cmd ="";
    if(DisableGPU)DisableGPU_cmd =" --disable-gpu ";
    QString ForceOpenCL_cmd ="";
    if(ForceOpenCL)ForceOpenCL_cmd =" --force-OpenCL ";
    //======
    if(CustRes_isContained(SourceFile_fullPath))
    {
        QMap<QString, QString> Res_map = CustRes_getResMap(SourceFile_fullPath);//res_map["fullpath"],["height"],["width"]
        ScaleRatio = CustRes_CalNewScaleRatio(InputPath,Res_map["height"].toInt(),Res_map["width"].toInt());
        if(ScaleRatio==0)
        {
            emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+InputPath+tr("]. Error: [The resolution of the source file cannot be read, so the image cannot be scaled to a custom resolution.]"));
            mutex_SubThreadNumRunning.lock();
            *Sub_video_ThreadNumRunning=*Sub_video_ThreadNumRunning-1;
            mutex_SubThreadNumRunning.unlock();
            return 0;
        }
    }
    //=======
    QString TTA_cmd = " -t 0 ";
    if(TTA_isEnabled)
    {
        TTA_cmd = " -t 1 ";
    }
    //=======
    for(int retry=0; retry<(ui->spinBox_retry->value()); retry++)
    {
        QString cmd = "\"" + program + "\"" + " -i " + "\"" + InputPath + "\"" + " -o " + "\"" + OutputPath + "\"" + " --scale-ratio " + QString::number(ScaleRatio, 10) + Denoise_cmd + " --model-dir " + "\"" + model_path + "\" --block-size "+QString::number(BlockSize, 10)+DisableGPU_cmd+ForceOpenCL_cmd+Processor_converter_STR+TTA_cmd;
        QProcess *Waifu2x = new QProcess();
        Waifu2x->start(cmd);
        while(!Waifu2x->waitForStarted(100)&&!QProcess_stop) {}
        while(!Waifu2x->waitForFinished(500)&&!QProcess_stop)
        {
            if(waifu2x_STOP)
            {
                Waifu2x->close();
                mutex_SubThreadNumRunning.lock();
                *Sub_video_ThreadNumRunning=*Sub_video_ThreadNumRunning-1;
                mutex_SubThreadNumRunning.unlock();
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
            emit Send_TextBrowser_NewMessage(tr("Automatic retry, please wait."));
        }
    }
    //========
    QFile::remove(Frame_fileFullPath);
    if(file_isFileExist(OutputPath)==false)*Frame_failed=true;
    //========
    mutex_SubThreadNumRunning.lock();
    *Sub_video_ThreadNumRunning=*Sub_video_ThreadNumRunning-1;
    mutex_SubThreadNumRunning.unlock();
    //========
    return 0;
}
/*
==========================================================================================================
                        WAIFU2X CONVERTER 检测GPU ID
==========================================================================================================
*/

void MainWindow::on_pushButton_DumpProcessorList_converter_clicked()
{
    ui->pushButton_Start->setEnabled(0);
    ui->pushButton_DetectGPU->setEnabled(0);
    ui->pushButton_DetectGPUID_srmd->setEnabled(0);
    ui->pushButton_DumpProcessorList_converter->setEnabled(0);
    ui->pushButton_compatibilityTest->setEnabled(0);
    QtConcurrent::run(this, &MainWindow::Waifu2x_DumpProcessorList_converter);
}

int MainWindow::Waifu2x_DumpProcessorList_converter()
{
    //============================
    //     调用converter输出列表
    //============================
    Core_num = 0;
    emit Send_TextBrowser_NewMessage(tr("Detecting available Processor, please wait."));
    QString Waifu2x_folder_path = Current_Path + "/waifu2x-converter";
    QString program = Waifu2x_folder_path + "/waifu2x-converter-cpp_waifu2xEX.exe";
    QProcess *Waifu2x = new QProcess();
    QString cmd = "\"" + program + "\"" + " -l ";
    Waifu2x->start(cmd);
    while(!Waifu2x->waitForStarted(100)&&!QProcess_stop) {}
    while(!Waifu2x->waitForFinished(100)&&!QProcess_stop) {}
    QString waifu2x_stdOut = Waifu2x->readAllStandardOutput();
    Core_num = waifu2x_stdOut.count("num_core");
    emit Send_TextBrowser_NewMessage("\n-------------\n"+waifu2x_stdOut+"\n-------------");
    //====================================================================
    //               获取到列表后, 对列表内处理器执行测试确认是否真的可用
    //====================================================================
    emit Send_TextBrowser_NewMessage(tr("Please wait while testing the processor."));
    //========
    Available_ProcessorList_converter.clear();
    //========
    QString InputPath = Current_Path + "/Compatibility_Test/Compatibility_Test.jpg";
    QString OutputPath = Current_Path + "/Compatibility_Test/res.jpg";
    QFile::remove(OutputPath);
    //==============
    QString model_path= Waifu2x_folder_path + "/models_rgb";
    //=========
    int Processor_ID=0;
    //=========
    for(int i = 0; i<Core_num; i++)
    {
        QFile::remove(OutputPath);
        QProcess *Waifu2x = new QProcess();
        QString Processor_str = " -p "+QString::number(Processor_ID,10)+" ";
        QString cmd = "\"" + program + "\"" + " -i " + "\"" + InputPath + "\"" + " -o " + "\"" + OutputPath + "\"" + " --scale-ratio 2 --noise-level 1 --model-dir " + "\"" + model_path + "\""+Processor_str;
        Waifu2x->start(cmd);
        while(!Waifu2x->waitForStarted(100)&&!QProcess_stop) {}
        while(!Waifu2x->waitForFinished(100)&&!QProcess_stop) {}
        if(file_isFileExist(OutputPath))
        {
            Available_ProcessorList_converter.append(QString::number(Processor_ID,10));
            Processor_ID++;
            QFile::remove(OutputPath);
        }
    }
    QFile::remove(OutputPath);
    //====================
    emit Send_TextBrowser_NewMessage(tr("Detection is complete!"));
    emit Send_Waifu2x_DumpProcessorList_converter_finished();
    return 0;
}
int MainWindow::Waifu2x_DumpProcessorList_converter_finished()
{
    ui->pushButton_Start->setEnabled(1);
    ui->pushButton_DetectGPU->setEnabled(1);
    ui->pushButton_compatibilityTest->setEnabled(1);
    ui->pushButton_DetectGPUID_srmd->setEnabled(1);
    ui->pushButton_DumpProcessorList_converter->setEnabled(1);
    //===========================
    ui->comboBox_TargetProcessor_converter->clear();
    ui->comboBox_TargetProcessor_converter->addItem("auto");
    if(!Available_ProcessorList_converter.isEmpty())
    {
        for(int i=0; i<Available_ProcessorList_converter.size(); i++)
        {
            ui->comboBox_TargetProcessor_converter->addItem(Available_ProcessorList_converter.at(i));
        }
    }
    return 0;
}

void MainWindow::on_comboBox_TargetProcessor_converter_currentIndexChanged(int index)
{
    if(ui->comboBox_TargetProcessor_converter->currentText()!="auto")
    {
        Processor_converter_STR = " -p "+ui->comboBox_TargetProcessor_converter->currentText()+" ";
    }
    else
    {
        Processor_converter_STR="";
    }
}
