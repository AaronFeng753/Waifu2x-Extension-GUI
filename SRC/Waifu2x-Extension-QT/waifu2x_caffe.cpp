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

int MainWindow::Waifu2x_Caffe_Image(int rowNum,bool ReProcess_MissingAlphaChannel)
{
    //============================= 读取设置 ================================
    int DenoiseLevel = ui->spinBox_DenoiseLevel_image->value();
    bool DelOriginal = (ui->checkBox_DelOriginal->isChecked()||ui->checkBox_ReplaceOriginalFile->isChecked());
    QString OutPutPath_Final ="";
    //========================= 拆解map得到参数 =============================
    emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, "Processing");
    QString SourceFile_fullPath = Table_model_image->item(rowNum,2)->text();
    if(!QFile::exists(SourceFile_fullPath))
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [File does not exist.]"));
        emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed");
        emit Send_progressbar_Add();
        mutex_ThreadNumRunning.lock();
        ThreadNumRunning--;
        mutex_ThreadNumRunning.unlock();//线程数量统计-1
        return 0;
    }
    //========= 转换到 PNG =========
    QString SourceFile_fullPath_Original = SourceFile_fullPath;
    SourceFile_fullPath = Imgae_PreProcess(SourceFile_fullPath_Original,ReProcess_MissingAlphaChannel);
    //=======================================================
    int ScaleRatio = 2;
    bool CustRes_isEnabled = false;
    int CustRes_height=0;
    int CustRes_width=0;
    if(CustRes_isContained(SourceFile_fullPath_Original))
    {
        CustRes_isEnabled=true;
        QMap<QString, QString> Res_map = CustRes_getResMap(SourceFile_fullPath_Original);//res_map["fullpath"],["height"],["width"]
        ScaleRatio = CustRes_CalNewScaleRatio(SourceFile_fullPath,Res_map["height"].toInt(),Res_map["width"].toInt());
        if(ScaleRatio==0)
        {
            emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath_Original+tr("]. Error: [The resolution of the source file cannot be read, so the image cannot be scaled to a custom resolution.]"));
            emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed");
            emit Send_progressbar_Add();
            mutex_ThreadNumRunning.lock();
            ThreadNumRunning--;
            mutex_ThreadNumRunning.unlock();//线程数量统计-1
            return 0;
        }
        CustRes_height=Res_map["height"].toInt();
        CustRes_width=Res_map["width"].toInt();
    }
    else
    {
        double ScaleRatio_double_tmp = ui->doubleSpinBox_ScaleRatio_image->value();
        if(ScaleRatio_double_tmp == (int)ScaleRatio_double_tmp)
        {
            ScaleRatio = qRound(ScaleRatio_double_tmp);
        }
        else
        {
            CustRes_isEnabled=true;
            QMap<QString, QString> Res_map = DoubleScaleRatio_Cal_NewScaleRatio_NewHW(SourceFile_fullPath,ScaleRatio_double_tmp);
            //====
            if(Res_map.isEmpty())
            {
                emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Unable to read the resolution of the source file.]"));
                emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed");
                emit Send_progressbar_Add();
                mutex_ThreadNumRunning.lock();
                ThreadNumRunning--;
                mutex_ThreadNumRunning.unlock();//线程数量统计-1
                return 0;
            }
            //====
            ScaleRatio = Res_map["ScaleRatio"].toInt();
            CustRes_height = Res_map["Height_new"].toInt();
            CustRes_width = Res_map["width_new"].toInt();
        }
    }
    //=======================================================
    QFileInfo fileinfo(SourceFile_fullPath);
    QString file_name = file_getBaseName(SourceFile_fullPath);
    QString file_ext = fileinfo.suffix();
    QString file_path = file_getFolderPath(fileinfo);
    QString OutPut_Path = file_path + "/" + file_name + "_waifu2x_"+QString::number(ScaleRatio, 10)+"x_"+QString::number(DenoiseLevel, 10)+"n_"+file_ext+".png";
    //============================== 放大 =======================================
    QString Waifu2x_folder_path = Current_Path + "/waifu2x-caffe";
    QString program = Waifu2x_folder_path + "/waifu2x-caffe_waifu2xEX.exe";
    //====
    QString ImageProcessingModeCMD = "";
    if(DenoiseLevel==-1)
    {
        ImageProcessingModeCMD = " -m scale -s " + QString::number(ScaleRatio, 10)+" ";
    }
    else
    {
        int DenoiseLevel_tmp =0;
        if((ui->comboBox_Model_2D_Waifu2xCaffe->currentIndex()==0)&&(DenoiseLevel==0)&&(ui->comboBox_ImageStyle_Waifu2xCaffe->currentIndex()==0))
        {
            DenoiseLevel_tmp=1;
        }
        else
        {
            DenoiseLevel_tmp = DenoiseLevel;
        }
        ImageProcessingModeCMD = " -m noise_scale -s " + QString::number(ScaleRatio, 10)+ " -n " + QString::number(DenoiseLevel_tmp, 10)+" ";
    }
    //====
    QString cmd = "\"" + program + "\"" + " -i " + "\"" + SourceFile_fullPath + "\"" + " -o " + "\"" + OutPut_Path + "\"" + ImageProcessingModeCMD + Waifu2x_Caffe_ReadSettings();
    //========
    for(int retry=0; retry<(ui->spinBox_retry->value()+ForceRetryCount); retry++)
    {
        QProcess *Waifu2x = new QProcess();
        Waifu2x->start(cmd);
        while(!Waifu2x->waitForStarted(100)&&!QProcess_stop) {}
        while(!Waifu2x->waitForFinished(500)&&!QProcess_stop)
        {
            if(waifu2x_STOP)
            {
                Waifu2x->close();
                QFile::remove(OutPut_Path);
                emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, "Interrupted");
                mutex_ThreadNumRunning.lock();
                ThreadNumRunning--;
                mutex_ThreadNumRunning.unlock();
                return 0;
            }
        }
        if(QFile::exists(OutPut_Path))
        {
            break;
        }
        else
        {
            if(retry==ui->spinBox_retry->value()+(ForceRetryCount-1))break;
            Delay_sec_sleep(5);
            emit Send_TextBrowser_NewMessage(tr("Automatic retry, please wait."));
        }
    }
    //========
    if(!QFile::exists(OutPut_Path))
    {
        if(SourceFile_fullPath_Original!=SourceFile_fullPath)
        {
            QFile::remove(SourceFile_fullPath);
        }
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath_Original+tr("]. Error: [Unable to scale the picture.]"));
        emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed");
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
        do
        {
            QImage qimage_adj(OutPut_Path);
            //读取放大后的图片并调整大小
            QImage qimage_adj_scaled = qimage_adj.scaled(CustRes_width,CustRes_height,CustRes_AspectRatioMode,Qt::SmoothTransformation);
            QImageWriter qimageW_adj;
            qimageW_adj.setFormat("png");
            qimageW_adj.setFileName(OutPut_Path);
            if(qimageW_adj.canWrite())
            {
                qimageW_adj.write(qimage_adj_scaled);
            }
            if(!QFile::exists(OutPut_Path))
            {
                emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Unable to resize the scaled picture to the target size]"));
                emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed");
                emit Send_progressbar_Add();
                mutex_ThreadNumRunning.lock();
                ThreadNumRunning--;
                mutex_ThreadNumRunning.unlock();//线程数量统计-1
                return 0;
            }
        }
        while(false);
        QString OutPut_Path_CustRes = file_path + "/" + file_name + "_waifu2x_"+QString::number(CustRes_width, 10)+"x"+QString::number(CustRes_height, 10)+"_"+QString::number(DenoiseLevel, 10)+"n_"+file_ext+".png";
        QFile::remove(OutPut_Path_CustRes);
        QFile::rename(OutPut_Path,OutPut_Path_CustRes);
        OutPutPath_Final = OutPut_Path_CustRes;
    }
    //=========================== 另存为JPG&压缩JPG ===========================================
    OutPutPath_Final = SaveImageAs_FormatAndQuality(SourceFile_fullPath_Original,OutPutPath_Final,true,DenoiseLevel);
    //================== 检查是否丢失了透明通道 =====================
    if(ReProcess_MissingAlphaChannel==false)
    {
        QImage QImage_source(SourceFile_fullPath_Original);
        QImage QImage_Final(OutPutPath_Final);
        if(QImage_source.hasAlphaChannel()==true && QImage_Final.hasAlphaChannel()==false && ui->checkBox_AutoDetectAlphaChannel->isChecked())
        {
            QFile::remove(OutPutPath_Final);
            emit Send_TextBrowser_NewMessage(tr("Since the Alpha channel in the resulting image is lost, this image will be reprocessed to fix it:[")+SourceFile_fullPath_Original+tr("].\nIf the reprocessing happens a lot, you should consider enable [Always pre-process images with Alpha Channel] in [Additional settings]."));
            return Waifu2x_Caffe_Image(rowNum,true);
        }
    }
    //============================= 删除原文件 &  & 更新table status ============================
    if(SourceFile_fullPath_Original!=SourceFile_fullPath)
    {
        QFile::remove(SourceFile_fullPath);
        SourceFile_fullPath = SourceFile_fullPath_Original;
    }
    if(DelOriginal)
    {
        if(ReplaceOriginalFile(SourceFile_fullPath,OutPutPath_Final)==false)
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
        emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, "Finished, original file deleted");
    }
    else
    {
        emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, "Finished");
    }
    //========== 移动到输出路径 =========
    if(ui->checkBox_OutPath_isEnabled->isChecked())
    {
        MoveFileToOutputPath(OutPutPath_Final,SourceFile_fullPath);
    }
    //============================ 更新进度条 =================================
    emit Send_progressbar_Add();
    //=========================== 更新线程数量统计==============================
    mutex_ThreadNumRunning.lock();
    ThreadNumRunning--;
    mutex_ThreadNumRunning.unlock();//线程数量统计-1
    return 0;
}


int MainWindow::Waifu2x_Caffe_GIF(int rowNum)
{
    //============================= 读取设置 ================================
    int ScaleRatio = ui->doubleSpinBox_ScaleRatio_gif->value();
    int DenoiseLevel = ui->spinBox_DenoiseLevel_gif->value();
    bool DelOriginal = (ui->checkBox_DelOriginal->isChecked()||ui->checkBox_ReplaceOriginalFile->isChecked());
    bool OptGIF = ui->checkBox_OptGIF->isChecked();
    int Sub_gif_ThreadNumRunning = 0;
    QString OutPutPath_Final ="";
    //========================= 拆解map得到参数 =============================
    emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, "Processing");
    QString SourceFile_fullPath = Table_model_gif->item(rowNum,2)->text();
    if(!QFile::exists(SourceFile_fullPath))
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [File does not exist.]"));
        emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed");
        emit Send_progressbar_Add();
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
    QString file_name = file_getBaseName(SourceFile_fullPath);
    QString file_ext = fileinfo.suffix();
    QString file_path = file_getFolderPath(fileinfo);
    QString ResGIFPath = file_path + "/" + file_name + "_waifu2x_"+QString::number(ScaleRatio, 10)+"x_"+QString::number(DenoiseLevel, 10)+"n.gif";
    //=========================== 获取帧时间 ====================================
    int GIF_Duration = Gif_getDuration(SourceFile_fullPath);
    if(GIF_Duration==0)//检查是否成功获取duration
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Can't get Duration value of GIF file.]"));
        emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed");
        //file_DelDir(SplitFramesFolderPath);
        emit Send_progressbar_Add();
        return 0;//如果启用stop位,则直接return
    }
    //============================== 拆分 ==========================================
    QString SplitFramesFolderPath = file_path+"/"+file_name+"_SplitFrames_W2xEX";//拆分后存储frame的文件夹
    Gif_splitGif(SourceFile_fullPath,SplitFramesFolderPath);
    //============================== 扫描获取文件名 ===============================
    QStringList Frame_fileName_list = file_getFileNames_in_Folder_nofilter(SplitFramesFolderPath);
    if(Frame_fileName_list.isEmpty())//检查是否成功拆分gif
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Can't split GIF into frames.]"));
        emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed");
        file_DelDir(SplitFramesFolderPath);
        emit Send_progressbar_Add();
        return 0;//如果启用stop位,则直接return
    }
    //===========建立存储放大后frame的文件夹===========
    QString ScaledFramesFolderPath = SplitFramesFolderPath+"/ScaledFrames_W2xEX";
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
    emit Send_CurrentFileProgress_Start(file_name+"."+file_ext,Frame_fileName_list.size());
    FileProgressWatch_isEnabled=true;
    QFuture<void> FileProgressWatch_QFuture = QtConcurrent::run(this, &MainWindow::CurrentFileProgress_WatchFolderFileNum, ScaledFramesFolderPath);//启动waifu2x 主线程
    if(ui->checkBox_ShowInterPro->isChecked()==false)
    {
        FileProgressWatch_isEnabled=false;
        FileProgressWatch_QFuture.cancel();
        emit Send_CurrentFileProgress_Stop();
    }
    //=======获取显卡信息========
    int NumOfGPU = 1;
    if(ui->checkBox_EnableMultiGPU_Waifu2xCaffe->isChecked()==true)
    {
        NumOfGPU = Get_NumOfGPU_W2xCaffe();
    }
    //============创建显卡文件夹===========
    QStringList GPU_SplitFramesFolderPath_List;
    for(int i = 0; i < NumOfGPU; i++)
    {
        QString GPU_SplitFramesFolderPath = SplitFramesFolderPath+"/"+"GPU_"+QString::number(i);
        if(file_isDirExist(GPU_SplitFramesFolderPath))
        {
            file_DelDir(GPU_SplitFramesFolderPath);
            file_mkDir(GPU_SplitFramesFolderPath);
        }
        else
        {
            file_mkDir(GPU_SplitFramesFolderPath);
        }
        GPU_SplitFramesFolderPath_List.append(GPU_SplitFramesFolderPath);
    }
    int TotalFramesNum = Frame_fileName_list.size();
    int FramesNumForEachGPU = TotalFramesNum/NumOfGPU;
    if(FramesNumForEachGPU<1)FramesNumForEachGPU=1;
    int start_num=0;
    for(int x = 0; x < GPU_SplitFramesFolderPath_List.size(); x++)
    {
        if(x==GPU_SplitFramesFolderPath_List.size()-1)FramesNumForEachGPU=TotalFramesNum;
        QStringList file_waitformove = Frame_fileName_list.mid(start_num,FramesNumForEachGPU);
        for(int i = 0; i < file_waitformove.size(); i++)
        {
            QString FileName = file_waitformove.at(i);
            QFile::rename(SplitFramesFolderPath+"/"+FileName,GPU_SplitFramesFolderPath_List.at(x)+"/"+FileName);
        }
        start_num+=FramesNumForEachGPU;
    }
    //=========================
    bool Frame_failed = false;//放大失败标志
    //====
    QMap<QString,QString> Sub_Thread_info;
    Sub_Thread_info["ScaledFramesFolderPath"]= ScaledFramesFolderPath;
    if(CustRes_isEnabled == true)
    {
        Sub_Thread_info["ScaleRatio"] = QString("%1").arg(CustRes_CalNewScaleRatio(SourceFile_fullPath,CustRes_height,CustRes_width));
    }
    else
    {
        Sub_Thread_info["ScaleRatio"] = QString("%1").arg(ui->doubleSpinBox_ScaleRatio_gif->value());
    }
    //=========================
    for(int i = 0; i < GPU_SplitFramesFolderPath_List.size(); i++)
    {
        Sub_Thread_info["SplitFramesFolderPath"]=GPU_SplitFramesFolderPath_List.at(i);
        mutex_SubThreadNumRunning.lock();
        Sub_gif_ThreadNumRunning++;
        mutex_SubThreadNumRunning.unlock();
        QtConcurrent::run(this,&MainWindow::Waifu2x_Caffe_GIF_scale,Sub_Thread_info,&Sub_gif_ThreadNumRunning,&Frame_failed);
        while (Sub_gif_ThreadNumRunning >= NumOfGPU)
        {
            Delay_msec_sleep(500);
        }
        if(waifu2x_STOP)
        {
            while (Sub_gif_ThreadNumRunning > 0)
            {
                Delay_msec_sleep(500);
            }
            FileProgressWatch_isEnabled=false;
            FileProgressWatch_QFuture.cancel();
            file_DelDir(SplitFramesFolderPath);
            emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, "Interrupted");
            return 0;//如果启用stop位,则直接return
        }
        if(Frame_failed)
        {
            FileProgressWatch_isEnabled=false;
            FileProgressWatch_QFuture.cancel();
            emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Failed to scale frames.]"));
            emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed");
            file_DelDir(SplitFramesFolderPath);
            emit Send_progressbar_Add();
            return 0;//如果启用stop位,则直接return
        }
    }
    while (Sub_gif_ThreadNumRunning!=0)
    {
        Delay_msec_sleep(500);
    }
    FileProgressWatch_isEnabled=false;
    FileProgressWatch_QFuture.cancel();
    emit Send_CurrentFileProgress_Stop();
    //======================= 检查是否成功放大所有帧 ===========================
    QStringList Frame_fileName_list_scaled = file_getFileNames_in_Folder_nofilter(ScaledFramesFolderPath);
    if(Frame_fileName_list.size()!=Frame_fileName_list_scaled.size())
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Failed to scale frames.]"));
        emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed");
        file_DelDir(SplitFramesFolderPath);
        emit Send_progressbar_Add();
        return 0;//如果启用stop位,则直接return
    }
    //======================================== 组装 ======================================================
    if(CustRes_isEnabled)
    {
        ResGIFPath = file_path + "/" + file_name + "_waifu2x_"+QString::number(CustRes_width, 10)+"x"+QString::number(CustRes_height,10)+"_"+QString::number(DenoiseLevel, 10)+"n.gif";
    }
    Gif_assembleGif(ResGIFPath,ScaledFramesFolderPath,GIF_Duration,CustRes_isEnabled,CustRes_height,CustRes_width,false,"");
    if(!QFile::exists(ResGIFPath))
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Unable to assemble gif.]"));
        emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed");
        file_DelDir(SplitFramesFolderPath);
        emit Send_progressbar_Add();
        return 0;//如果启用stop位,则直接return
    }
    OutPutPath_Final = ResGIFPath;
    //======================================= 优化gif ===================================================
    if(OptGIF)
    {
        QString ResGIFPath_compressed = "";
        if(CustRes_isEnabled)
        {
            ResGIFPath_compressed = file_path + "/" + file_name + "_waifu2x_"+QString::number(CustRes_width, 10)+"x"+QString::number(CustRes_height,10)+"_"+QString::number(DenoiseLevel, 10)+"n_opt.gif";
        }
        else
        {
            ResGIFPath_compressed = file_path + "/" + file_name + "_waifu2x_"+QString::number(ScaleRatio, 10)+"x_"+QString::number(DenoiseLevel, 10)+"n_opt.gif";
        }
        OutPutPath_Final = Gif_compressGif(ResGIFPath,ResGIFPath_compressed);
    }
    //============================== 删除缓存文件 ====================================================
    file_DelDir(SplitFramesFolderPath);
    //============================= 删除原文件 &  & 更新table status ============================
    if(DelOriginal)
    {
        if(ReplaceOriginalFile(SourceFile_fullPath,OutPutPath_Final)==false)
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
        emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, "Finished, original file deleted");
    }
    else
    {
        emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, "Finished");
    }
    //========== 移动到输出路径 =========
    if(ui->checkBox_OutPath_isEnabled->isChecked())
    {
        MoveFileToOutputPath(OutPutPath_Final,SourceFile_fullPath);
    }
    //============================ 更新进度条 =================================
    emit Send_progressbar_Add();
    //===========================  ==============================
    return 0;
}

int MainWindow::Waifu2x_Caffe_GIF_scale(QMap<QString, QString> Sub_Thread_info,int *Sub_gif_ThreadNumRunning,bool *Frame_failed)
{
    bool AllFinished=true;
    QString SplitFramesFolderPath = Sub_Thread_info["SplitFramesFolderPath"];
    QString ScaledFramesFolderPath = Sub_Thread_info["ScaledFramesFolderPath"];
    //===========
    int ScaleRatio = Sub_Thread_info["ScaleRatio"].toInt();
    int DenoiseLevel = ui->spinBox_DenoiseLevel_gif->value();
    //===========
    QString program = Current_Path + "/waifu2x-caffe/waifu2x-caffe_waifu2xEX.exe";
    //====
    QString ImageProcessingModeCMD = "";
    if(DenoiseLevel==-1)
    {
        ImageProcessingModeCMD = " -m scale -s " + QString::number(ScaleRatio, 10)+" ";
    }
    else
    {
        int DenoiseLevel_tmp =0;
        if((ui->comboBox_Model_2D_Waifu2xCaffe->currentIndex()==0)&&(DenoiseLevel==0)&&(ui->comboBox_ImageStyle_Waifu2xCaffe->currentIndex()==0))
        {
            DenoiseLevel_tmp=1;
        }
        else
        {
            DenoiseLevel_tmp = DenoiseLevel;
        }
        ImageProcessingModeCMD = " -m noise_scale -s " + QString::number(ScaleRatio, 10)+ " -n " + QString::number(DenoiseLevel_tmp, 10)+" ";
    }
    //=======
    //========================================================================
    QStringList InputFilesNameList = file_getFileNames_in_Folder_nofilter(SplitFramesFolderPath);
    QStringList OutPutFilesFullPathList;
    for(int i=0; i<InputFilesNameList.size(); i++)
    {
        OutPutFilesFullPathList.append(ScaledFramesFolderPath+"/"+InputFilesNameList.at(i));
    }
    int OutPutFilesFullPathList_size = OutPutFilesFullPathList.size();
    //=======
    QString cmd = "\"" + program + "\"" + " -i " + "\"" + SplitFramesFolderPath + "\"" + " -o " + "\"" + ScaledFramesFolderPath + "\"" + ImageProcessingModeCMD+ Waifu2x_Caffe_ReadSettings();
    QProcess *Waifu2x = new QProcess();
    //=======
    for(int retry=0; retry<(ui->spinBox_retry->value()+ForceRetryCount); retry++)
    {
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
        AllFinished = true;
        for(int i=0; i<OutPutFilesFullPathList_size; i++)
        {
            if(QFile::exists(OutPutFilesFullPathList.at(i))==false)
            {
                if(retry==ui->spinBox_retry->value()+(ForceRetryCount-1))break;
                Delay_sec_sleep(5);
                emit Send_TextBrowser_NewMessage(tr("Automatic retry, please wait."));
                AllFinished=false;
                break;
            }
        }
        if(AllFinished)break;
    }
    //=========
    AllFinished = true;
    for(int i=0; i<OutPutFilesFullPathList_size; i++)
    {
        if(QFile::exists(OutPutFilesFullPathList.at(i))==false)
        {
            AllFinished=false;
            break;
        }
    }
    if(AllFinished)
    {
        file_DelDir(SplitFramesFolderPath);
    }
    else
    {
        *Frame_failed=true;
    }
    //========
    mutex_SubThreadNumRunning.lock();
    *Sub_gif_ThreadNumRunning=*Sub_gif_ThreadNumRunning-1;
    mutex_SubThreadNumRunning.unlock();
    //========
    return 0;
}
/*
正常處理視頻(不分段)
*/
int MainWindow::Waifu2x_Caffe_Video(int rowNum)
{
    //============================= 读取设置 ================================
    int ScaleRatio = ui->doubleSpinBox_ScaleRatio_video->value();
    int DenoiseLevel = ui->spinBox_DenoiseLevel_video->value();
    bool DelOriginal = (ui->checkBox_DelOriginal->isChecked()||ui->checkBox_ReplaceOriginalFile->isChecked());
    bool isCacheExists = false;
    bool isVideoConfigChanged = true;
    int Sub_video_ThreadNumRunning = 0;
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
    //============ 读取自定义分辨率 ==============
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
    //=================
    QString AudioPath = file_path+"/Audio_"+file_name+"_"+file_ext+"_W2xEX.wav";//音频
    QString SplitFramesFolderPath = file_path+"/"+file_name+"_"+file_ext+"_SplitFrames_W2xEX";//拆分后存储frame的文件夹
    QString ScaledFramesFolderPath = SplitFramesFolderPath+"/ScaledFrames_W2xEX";//存储放大后的帧
    //==========================
    //   检测之前的视频配置文件
    //==========================
    QString VideoConfiguration_fullPath = file_path+"/VideoConfiguration_"+file_name+"_"+file_ext+"_Waifu2xEX.ini";
    if(QFile::exists(VideoConfiguration_fullPath))
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
        if(EngineName_old=="waifu2x-caffe")
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
        emit Send_video_write_VideoConfiguration(VideoConfiguration_fullPath,ScaleRatio,DenoiseLevel,CustRes_isEnabled,CustRes_height,CustRes_width,"waifu2x-caffe",false,"","",ui->groupBox_FrameInterpolation->isChecked(),0);
    }
    //=======================
    //   检测缓存是否存在
    //=======================
    if(QFile::exists(video_mp4_fullpath)&&file_isDirExist(SplitFramesFolderPath)&&file_isDirExist(ScaledFramesFolderPath)&&QFile::exists(VideoConfiguration_fullPath))
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
            DelVfiDir(video_mp4_fullpath);
            emit Send_video_write_VideoConfiguration(VideoConfiguration_fullPath,ScaleRatio,DenoiseLevel,CustRes_isEnabled,CustRes_height,CustRes_width,"waifu2x-caffe",false,"","",ui->groupBox_FrameInterpolation->isChecked(),0);
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
        DelVfiDir(video_mp4_fullpath);
        emit Send_video_write_VideoConfiguration(VideoConfiguration_fullPath,ScaleRatio,DenoiseLevel,CustRes_isEnabled,CustRes_height,CustRes_width,"waifu2x-caffe",false,"","",ui->groupBox_FrameInterpolation->isChecked(),0);
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
        video_video2images(video_mp4_fullpath,SplitFramesFolderPath,AudioPath);
        if(waifu2x_STOP)
        {
            emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, "Interrupted");
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
            emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed");
            emit Send_progressbar_Add();
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
    emit Send_CurrentFileProgress_Start(file_name+"."+file_ext,(Frame_fileName_list.size()+file_getFileNames_in_Folder_nofilter(ScaledFramesFolderPath).size()));
    FileProgressWatch_isEnabled=true;
    QFuture<void> FileProgressWatch_QFuture = QtConcurrent::run(this, &MainWindow::CurrentFileProgress_WatchFolderFileNum, ScaledFramesFolderPath);//启动waifu2x 主线程
    if(ui->checkBox_ShowInterPro->isChecked()==false)
    {
        FileProgressWatch_isEnabled=false;
        FileProgressWatch_QFuture.cancel();
        emit Send_CurrentFileProgress_Stop();
    }
    //=======获取显卡信息========
    int NumOfGPU = 1;
    if(ui->checkBox_EnableMultiGPU_Waifu2xCaffe->isChecked()==true)
    {
        NumOfGPU = Get_NumOfGPU_W2xCaffe();
    }
    //============创建显卡文件夹===========
    QStringList GPU_SplitFramesFolderPath_List;
    for(int i = 0; i < NumOfGPU; i++)
    {
        QString GPU_SplitFramesFolderPath = SplitFramesFolderPath+"/"+"GPU_"+QString::number(i);
        if(file_isDirExist(GPU_SplitFramesFolderPath)==false)
        {
            file_DelDir(GPU_SplitFramesFolderPath);
            file_mkDir(GPU_SplitFramesFolderPath);
        }
        else
        {
            file_mkDir(GPU_SplitFramesFolderPath);
        }
        GPU_SplitFramesFolderPath_List.append(GPU_SplitFramesFolderPath);
    }
    int TotalFramesNum = Frame_fileName_list.size();
    int FramesNumForEachGPU = TotalFramesNum/NumOfGPU;
    if(FramesNumForEachGPU<1)FramesNumForEachGPU=1;
    int start_num=0;
    for(int x = 0; x < GPU_SplitFramesFolderPath_List.size(); x++)
    {
        if(x==GPU_SplitFramesFolderPath_List.size()-1)FramesNumForEachGPU=TotalFramesNum;
        QStringList file_waitformove = Frame_fileName_list.mid(start_num,FramesNumForEachGPU);
        for(int i = 0; i < file_waitformove.size(); i++)
        {
            QString FileName = file_waitformove.at(i);
            QFile::rename(SplitFramesFolderPath+"/"+FileName,GPU_SplitFramesFolderPath_List.at(x)+"/"+FileName);
        }
        start_num+=FramesNumForEachGPU;
    }
    //=========================
    bool Frame_failed = false;//放大失败标志
    QMap<QString,QString> Sub_Thread_info;
    Sub_Thread_info["ScaledFramesFolderPath"]=ScaledFramesFolderPath;
    if(CustRes_isEnabled == true)
    {
        Sub_Thread_info["ScaleRatio"] = QString("%1").arg(CustRes_CalNewScaleRatio(video_mp4_fullpath,CustRes_height,CustRes_width));
    }
    else
    {
        Sub_Thread_info["ScaleRatio"] = QString("%1").arg(ScaleRatio);
    }
    //=========================
    for(int i = 0; i < GPU_SplitFramesFolderPath_List.size(); i++)
    {
        Sub_Thread_info["SplitFramesFolderPath"]=GPU_SplitFramesFolderPath_List.at(i);
        mutex_SubThreadNumRunning.lock();
        Sub_video_ThreadNumRunning++;
        mutex_SubThreadNumRunning.unlock();
        QtConcurrent::run(this,&MainWindow::Waifu2x_Caffe_Video_scale,Sub_Thread_info,&Sub_video_ThreadNumRunning,&Frame_failed);
        while (Sub_video_ThreadNumRunning >= NumOfGPU)
        {
            Delay_msec_sleep(500);
        }
        if(waifu2x_STOP)
        {
            while (Sub_video_ThreadNumRunning > 0)
            {
                Delay_msec_sleep(500);
            }
            FileProgressWatch_isEnabled=false;
            FileProgressWatch_QFuture.cancel();
            Restore_SplitFramesFolderPath(SplitFramesFolderPath,GPU_SplitFramesFolderPath_List);
            emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, "Interrupted");
            return 0;//如果启用stop位,则直接return
        }
        if(Frame_failed)
        {
            FileProgressWatch_isEnabled=false;
            FileProgressWatch_QFuture.cancel();
            Restore_SplitFramesFolderPath(SplitFramesFolderPath,GPU_SplitFramesFolderPath_List);
            emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Failed to scale frames.]"));
            emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed");
            emit Send_progressbar_Add();
            return 0;//如果启用stop位,则直接return
        }
    }
    while (Sub_video_ThreadNumRunning!=0)
    {
        Delay_msec_sleep(500);
    }
    FileProgressWatch_isEnabled=false;
    FileProgressWatch_QFuture.cancel();
    emit Send_CurrentFileProgress_Stop();
    //================ 扫描放大后的帧文件数量,判断是否放大成功 =======================
    QStringList Frame_fileName_list_scaled = file_getFileNames_in_Folder_nofilter(ScaledFramesFolderPath);
    if(Frame_fileName_list_scaled.size()<Frame_fileName_list.size())
    {
        Restore_SplitFramesFolderPath(SplitFramesFolderPath,GPU_SplitFramesFolderPath_List);
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Unable to scale all frames.]"));
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed");
        emit Send_progressbar_Add();
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
    video_images2video(video_mp4_fullpath,video_mp4_scaled_fullpath,ScaledFramesFolderPath,AudioPath,CustRes_isEnabled,CustRes_height,CustRes_width,false);
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
        QFile::remove(VideoConfiguration_fullPath);
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
#### 分段处理 ####
调用Waifu2x-caffe处理视频
读取设置,拆分,创建放大子线程,组装视频
*/
int MainWindow::Waifu2x_Caffe_Video_BySegment(int rowNum)
{
    //============================= 读取设置 ================================
    int ScaleRatio = ui->doubleSpinBox_ScaleRatio_video->value();
    int DenoiseLevel = ui->spinBox_DenoiseLevel_video->value();
    bool DelOriginal = (ui->checkBox_DelOriginal->isChecked()||ui->checkBox_ReplaceOriginalFile->isChecked());
    bool isCacheExists = false;
    bool isVideoConfigChanged = true;
    int Sub_video_ThreadNumRunning = 0;
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
    //============== 读取自定义分辨率 ============
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
    //=================
    QString AudioPath = file_path+"/Audio_"+file_name+"_"+file_ext+"_W2xEX.wav";//音频
    QString SplitFramesFolderPath = file_path+"/"+file_name+"_"+file_ext+"_SplitFrames_W2xEX";//拆分后存储frame的文件夹
    QString ScaledFramesFolderPath = SplitFramesFolderPath+"/ScaledFrames_W2xEX";//存储放大后的帧
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
    if(QFile::exists(VideoConfiguration_fullPath))
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
        bool isVideoFrameInterpolationEnabled_old = configIniRead->value("/VideoConfiguration/isVideoFrameInterpolationEnabled").toBool();
        int MultipleOfFPS_old = configIniRead->value("/VideoConfiguration/MultipleOfFPS").toInt();
        //=================== 比对信息 ================================
        if(EngineName_old=="waifu2x-caffe")
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
            if(isVideoFrameInterpolationEnabled_old != ui->groupBox_FrameInterpolation->isChecked())
            {
                isVideoConfigChanged=true;
            }
            if(MultipleOfFPS_old != ui->spinBox_MultipleOfFPS_VFI->value() && ui->groupBox_FrameInterpolation->isChecked())
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
        file_mkDir(VideoClipsFolderPath_old);
        if(file_isDirExist(VideoClipsFolderPath_old)==true)
        {
            VideoClipsFolderPath = VideoClipsFolderPath_old;
            VideoClipsFolderName = VideoClipsFolderName_old;
        }
    }
    else
    {
        emit Send_video_write_VideoConfiguration(VideoConfiguration_fullPath,ScaleRatio,DenoiseLevel,CustRes_isEnabled,CustRes_height,CustRes_width,"waifu2x-caffe",true,VideoClipsFolderPath,VideoClipsFolderName,ui->groupBox_FrameInterpolation->isChecked(),ui->spinBox_MultipleOfFPS_VFI->value());
    }
    //=======================
    //   检测缓存是否存在
    //=======================
    if(file_isDirExist(SplitFramesFolderPath))
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
            DelVfiDir(video_mp4_fullpath);
            emit Send_video_write_VideoConfiguration(VideoConfiguration_fullPath,ScaleRatio,DenoiseLevel,CustRes_isEnabled,CustRes_height,CustRes_width,"waifu2x-caffe",true,VideoClipsFolderPath,VideoClipsFolderName,ui->groupBox_FrameInterpolation->isChecked(),ui->spinBox_MultipleOfFPS_VFI->value());
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
        DelVfiDir(video_mp4_fullpath);
        emit Send_video_write_VideoConfiguration(VideoConfiguration_fullPath,ScaleRatio,DenoiseLevel,CustRes_isEnabled,CustRes_height,CustRes_width,"waifu2x-caffe",true,VideoClipsFolderPath,VideoClipsFolderName,ui->groupBox_FrameInterpolation->isChecked(),ui->spinBox_MultipleOfFPS_VFI->value());
        //========
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
        emit Send_CurrentFileProgress_Start(file_name+"."+file_ext,VideoDuration);
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
            if(waifu2x_STOP)
            {
                emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, "Interrupted");
                return 0;//如果启用stop位,则直接return
            }
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
            emit Send_video_write_Progress_ProcessBySegment(VideoConfiguration_fullPath,StartTime,true,false,SegmentDuration_tmp,LastVideoClipNo);
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
            FileProgressWatch_isEnabled=true;
            QFuture<void> FileProgressWatch_QFuture = QtConcurrent::run(this, &MainWindow::CurrentFileProgress_WatchFolderFileNum_Textbrower, SourceFile_fullPath,ScaledFramesFolderPath,Frame_fileName_list.size()+file_getFileNames_in_Folder_nofilter(ScaledFramesFolderPath).size());
            if(ui->checkBox_ShowInterPro->isChecked()==false)
            {
                FileProgressWatch_isEnabled=false;
                FileProgressWatch_QFuture.cancel();
            }
            //=======获取显卡信息========
            int NumOfGPU = 1;
            if(ui->checkBox_EnableMultiGPU_Waifu2xCaffe->isChecked()==true)
            {
                NumOfGPU = Get_NumOfGPU_W2xCaffe();
            }
            //============创建显卡文件夹===========
            QStringList GPU_SplitFramesFolderPath_List;
            for(int i = 0; i < NumOfGPU; i++)
            {
                QString GPU_SplitFramesFolderPath = SplitFramesFolderPath+"/"+"GPU_"+QString::number(i);
                if(file_isDirExist(GPU_SplitFramesFolderPath)==false)
                {
                    file_DelDir(GPU_SplitFramesFolderPath);
                    file_mkDir(GPU_SplitFramesFolderPath);
                }
                else
                {
                    file_mkDir(GPU_SplitFramesFolderPath);
                }
                GPU_SplitFramesFolderPath_List.append(GPU_SplitFramesFolderPath);
            }
            int TotalFramesNum = Frame_fileName_list.size();
            int FramesNumForEachGPU = TotalFramesNum/NumOfGPU;
            if(FramesNumForEachGPU<1)FramesNumForEachGPU=1;
            int start_num=0;
            for(int x = 0; x < GPU_SplitFramesFolderPath_List.size(); x++)
            {
                if(x==GPU_SplitFramesFolderPath_List.size()-1)FramesNumForEachGPU=TotalFramesNum;
                QStringList file_waitformove = Frame_fileName_list.mid(start_num,FramesNumForEachGPU);
                for(int i = 0; i < file_waitformove.size(); i++)
                {
                    QString FileName = file_waitformove.at(i);
                    QFile::rename(SplitFramesFolderPath+"/"+FileName,GPU_SplitFramesFolderPath_List.at(x)+"/"+FileName);
                }
                start_num+=FramesNumForEachGPU;
            }
            //=========================
            bool Frame_failed = false;//放大失败标志
            QMap<QString,QString> Sub_Thread_info;
            Sub_Thread_info["ScaledFramesFolderPath"]=ScaledFramesFolderPath;
            if(CustRes_isEnabled == true)
            {
                Sub_Thread_info["ScaleRatio"] = QString("%1").arg(CustRes_CalNewScaleRatio(video_mp4_fullpath,CustRes_height,CustRes_width));
            }
            else
            {
                Sub_Thread_info["ScaleRatio"] = QString("%1").arg(ScaleRatio);
            }
            //=========================
            for(int i = 0; i < GPU_SplitFramesFolderPath_List.size(); i++)
            {
                Sub_Thread_info["SplitFramesFolderPath"]=GPU_SplitFramesFolderPath_List.at(i);
                mutex_SubThreadNumRunning.lock();
                Sub_video_ThreadNumRunning++;
                mutex_SubThreadNumRunning.unlock();
                QtConcurrent::run(this,&MainWindow::Waifu2x_Caffe_Video_scale,Sub_Thread_info,&Sub_video_ThreadNumRunning,&Frame_failed);
                while (Sub_video_ThreadNumRunning >= NumOfGPU)
                {
                    Delay_msec_sleep(500);
                }
                if(waifu2x_STOP)
                {
                    while (Sub_video_ThreadNumRunning > 0)
                    {
                        Delay_msec_sleep(500);
                    }
                    FileProgressWatch_isEnabled=false;
                    FileProgressWatch_QFuture.cancel();
                    Restore_SplitFramesFolderPath(SplitFramesFolderPath,GPU_SplitFramesFolderPath_List);
                    emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, "Interrupted");
                    return 0;//如果启用stop位,则直接return
                }
                if(Frame_failed)
                {
                    FileProgressWatch_isEnabled=false;
                    FileProgressWatch_QFuture.cancel();
                    Restore_SplitFramesFolderPath(SplitFramesFolderPath,GPU_SplitFramesFolderPath_List);
                    emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Failed to scale frames.]"));
                    emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed");
                    emit Send_progressbar_Add();
                    return 0;//如果启用stop位,则直接return
                }
            }
            while (Sub_video_ThreadNumRunning!=0)
            {
                Delay_msec_sleep(500);
            }
            FileProgressWatch_isEnabled=false;
            FileProgressWatch_QFuture.cancel();
            //================ 扫描放大后的帧文件数量,判断是否放大成功 =======================
            QStringList Frame_fileName_list_scaled = file_getFileNames_in_Folder_nofilter(ScaledFramesFolderPath);
            if(Frame_fileName_list_scaled.size()<Frame_fileName_list.size())
            {
                Restore_SplitFramesFolderPath(SplitFramesFolderPath,GPU_SplitFramesFolderPath_List);
                emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Unable to scale all frames.]"));
                emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed");
                emit Send_progressbar_Add();
                return 0;//如果启用stop位,则直接return
            }
            /*
            记录进度
            帧处理成功
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
        video_images2video(video_mp4_fullpath,video_mp4_scaled_clip_fullpath,ScaledFramesFolderPath,"",CustRes_isEnabled,CustRes_height,CustRes_width,false);
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
    //===========================  ==============================
    return 0;
}
/*
视频帧放大函数
*/
int MainWindow::Waifu2x_Caffe_Video_scale(QMap<QString,QString> Sub_Thread_info,int *Sub_video_ThreadNumRunning,bool *Frame_failed)
{
    bool AllFinished=true;
    QString SplitFramesFolderPath = Sub_Thread_info["SplitFramesFolderPath"];
    QString ScaledFramesFolderPath = Sub_Thread_info["ScaledFramesFolderPath"];
    //===========
    int ScaleRatio = Sub_Thread_info["ScaleRatio"].toInt();
    int DenoiseLevel = ui->spinBox_DenoiseLevel_video->value();
    //===========
    QString program = Current_Path + "/waifu2x-caffe/waifu2x-caffe_waifu2xEX.exe";
    //====
    QString ImageProcessingModeCMD = "";
    if(DenoiseLevel==-1)
    {
        ImageProcessingModeCMD = " -m scale -s " + QString::number(ScaleRatio, 10)+" ";
    }
    else
    {
        int DenoiseLevel_tmp =0;
        if((ui->comboBox_Model_2D_Waifu2xCaffe->currentIndex()==0)&&(DenoiseLevel==0)&&(ui->comboBox_ImageStyle_Waifu2xCaffe->currentIndex()==0))
        {
            DenoiseLevel_tmp=1;
        }
        else
        {
            DenoiseLevel_tmp = DenoiseLevel;
        }
        ImageProcessingModeCMD = " -m noise_scale -s " + QString::number(ScaleRatio, 10)+ " -n " + QString::number(DenoiseLevel_tmp, 10)+" ";
    }
    //=======
    //========================================================================
    QStringList InputFilesNameList = file_getFileNames_in_Folder_nofilter(SplitFramesFolderPath);
    QStringList OutPutFilesFullPathList;
    for(int i=0; i<InputFilesNameList.size(); i++)
    {
        OutPutFilesFullPathList.append(ScaledFramesFolderPath+"/"+InputFilesNameList.at(i));
    }
    int OutPutFilesFullPathList_size = OutPutFilesFullPathList.size();
    //=======
    QString cmd = "\"" + program + "\"" + " -i " + "\"" + SplitFramesFolderPath + "\"" + " -o " + "\"" + ScaledFramesFolderPath + "\"" + ImageProcessingModeCMD+ Waifu2x_Caffe_ReadSettings();
    QProcess *Waifu2x = new QProcess();
    //=======
    for(int retry=0; retry<(ui->spinBox_retry->value()+ForceRetryCount); retry++)
    {
        Waifu2x->start(cmd);
        while(!Waifu2x->waitForStarted(100)&&!QProcess_stop) {}
        while(!Waifu2x->waitForFinished(500)&&!QProcess_stop)
        {
            if(waifu2x_STOP)
            {
                QStringList FinfishedFileList = WaitForEngineIO(OutPutFilesFullPathList);
                Waifu2x->close();
                //============
                QString file_fullpath_tmp;
                for(int i=0; i<OutPutFilesFullPathList_size; i++)
                {
                    file_fullpath_tmp = OutPutFilesFullPathList.at(i);
                    if(FinfishedFileList.contains(file_fullpath_tmp))
                    {
                        QFileInfo fileinfo(file_fullpath_tmp);
                        QString file_name = file_getBaseName(file_fullpath_tmp);
                        QString file_ext = fileinfo.suffix();
                        QFile::remove(SplitFramesFolderPath + "/" + file_name + "." + file_ext);
                    }
                    else
                    {
                        QFile::remove(file_fullpath_tmp);
                    }
                }
                //=============
                mutex_SubThreadNumRunning.lock();
                *Sub_video_ThreadNumRunning=*Sub_video_ThreadNumRunning-1;
                mutex_SubThreadNumRunning.unlock();
                return 0;
            }
        }
        AllFinished = true;
        for(int i=0; i<OutPutFilesFullPathList_size; i++)
        {
            if(QFile::exists(OutPutFilesFullPathList.at(i))==false)
            {
                if(retry==ui->spinBox_retry->value()+(ForceRetryCount-1))break;
                Delay_sec_sleep(5);
                emit Send_TextBrowser_NewMessage(tr("Automatic retry, please wait."));
                AllFinished=false;
                break;
            }
        }
        if(AllFinished)break;
    }
    //=========
    AllFinished = true;
    for(int i=0; i<OutPutFilesFullPathList_size; i++)
    {
        if(QFile::exists(OutPutFilesFullPathList.at(i))==false)
        {
            AllFinished=false;
            break;
        }
    }
    if(AllFinished)
    {
        file_DelDir(SplitFramesFolderPath);
    }
    else
    {
        *Frame_failed=true;
    }
    //========
    mutex_SubThreadNumRunning.lock();
    *Sub_video_ThreadNumRunning=*Sub_video_ThreadNumRunning-1;
    mutex_SubThreadNumRunning.unlock();
    //========
    return 0;
}
/*
Waifu2x_Caffe
预读取引擎设定
*/
QString MainWindow::Waifu2xCaffe_PreLoad_Settings()
{
    QString Waifu2x_Caffe_Settings_str = " ";
    //处理模式
    switch(ui->comboBox_ProcessMode_Waifu2xCaffe->currentIndex())
    {
        case 0:
            {
                Waifu2x_Caffe_Settings_str.append("-p cpu ");
                break;
            }
        case 1:
            {
                Waifu2x_Caffe_Settings_str.append("-p gpu ");
                break;
            }
        case 2:
            {
                Waifu2x_Caffe_Settings_str.append("-p cudnn ");
                break;
            }
    }
    //模型
    QString model_folder_path = Current_Path + "/waifu2x-caffe/models/";
    int ImageStyle = ui->comboBox_ImageStyle_Waifu2xCaffe->currentIndex();
    switch(ImageStyle)
    {
        case 0:
            {
                switch(ui->comboBox_Model_2D_Waifu2xCaffe->currentIndex())
                {
                    case 0:
                        {
                            Waifu2x_Caffe_Settings_str.append("--model_dir \""+model_folder_path+"anime_style_art\" ");
                            break;
                        }
                    case 1:
                        {
                            Waifu2x_Caffe_Settings_str.append("--model_dir \""+model_folder_path+"anime_style_art_rgb\" ");
                            break;
                        }
                    case 2:
                        {
                            Waifu2x_Caffe_Settings_str.append("--model_dir \""+model_folder_path+"cunet\" ");
                            break;
                        }
                    case 3:
                        {
                            Waifu2x_Caffe_Settings_str.append("--model_dir \""+model_folder_path+"photo\" ");
                            break;
                        }
                    case 4:
                        {
                            Waifu2x_Caffe_Settings_str.append("--model_dir \""+model_folder_path+"upconv_7_anime_style_art_rgb\" ");
                            break;
                        }
                    case 5:
                        {
                            Waifu2x_Caffe_Settings_str.append("--model_dir \""+model_folder_path+"upconv_7_photo\" ");
                            break;
                        }
                    case 6:
                        {
                            Waifu2x_Caffe_Settings_str.append("--model_dir \""+model_folder_path+"upresnet10\" ");
                            break;
                        }
                }
                break;
            }
        case 1:
            {
                switch(ui->comboBox_Model_3D_Waifu2xCaffe->currentIndex())
                {
                    case 0:
                        {
                            Waifu2x_Caffe_Settings_str.append("--model_dir \""+model_folder_path+"upconv_7_photo\" ");
                            break;
                        }
                    case 1:
                        {
                            Waifu2x_Caffe_Settings_str.append("--model_dir \""+model_folder_path+"photo\" ");
                            break;
                        }
                }
                break;
            }
    }
    //TTA
    if(ui->checkBox_TTA_Waifu2xCaffe->isChecked())
    {
        Waifu2x_Caffe_Settings_str.append("-t 1 ");
    }
    else
    {
        Waifu2x_Caffe_Settings_str.append("-t 0 ");
    }
    //显卡设定
    if(ui->checkBox_EnableMultiGPU_Waifu2xCaffe->isChecked()==false)
    {
        //单个显卡
        //GPU ID
        if(ui->comboBox_ProcessMode_Waifu2xCaffe->currentIndex()>0)
        {
            Waifu2x_Caffe_Settings_str.append("--gpu "+QString::number(ui->spinBox_GPUID_Waifu2xCaffe->value(),10)+" ");
        }
        //Batch size
        Waifu2x_Caffe_Settings_str.append("-b "+QString::number(ui->spinBox_BatchSize_Waifu2xCaffe->value(),10)+" ");
        //Split size
        Waifu2x_Caffe_Settings_str.append("-c "+QString::number(ui->spinBox_SplitSize_Waifu2xCaffe->value(),10)+" ");
    }
    return Waifu2x_Caffe_Settings_str;
}
/*
Waifu2x_Caffe
读取配置生成配置string
*/
QString MainWindow::Waifu2x_Caffe_ReadSettings()
{
    QString Waifu2x_Caffe_Settings_str = "";
    Waifu2x_Caffe_Settings_str.append(Waifu2xCaffe_PreLoad_Settings_Str);
    //显卡设定
    if(ui->checkBox_EnableMultiGPU_Waifu2xCaffe->isChecked())
    {
        //多显卡
        Waifu2x_Caffe_Settings_str.append(Waifu2xCaffe_GetGPUInfo()+" ");
    }
    return Waifu2x_Caffe_Settings_str;
}
//判断是否启用了caffe引擎
bool MainWindow::isWaifu2xCaffeEnabled()
{
    if(ui->comboBox_Engine_Image->currentIndex()!=4&&ui->comboBox_Engine_GIF->currentIndex()!=4&&ui->comboBox_Engine_Video->currentIndex()!=4)
    {
        ui->label_ImageStyle_W2xCaffe->setVisible(0);
        ui->comboBox_ImageStyle_Waifu2xCaffe->setVisible(0);
        ui->comboBox_ImageStyle_Waifu2xCaffe->setEnabled(0);
        return false;
    }
    else
    {
        ui->label_ImageStyle_W2xCaffe->setVisible(1);
        ui->comboBox_ImageStyle_Waifu2xCaffe->setVisible(1);
        ui->comboBox_ImageStyle_Waifu2xCaffe->setEnabled(1);
        return true;
    }
}
/*
删除caffe引擎生成的错误日志
*/
void MainWindow::DeleteErrorLog_Waifu2xCaffe()
{
    QStringList FileNameList = file_getFileNames_in_Folder_nofilter(Current_Path);//读取合法的文件名
    QString Full_Path_File = "";
    if(!FileNameList.isEmpty())
    {
        for(int i = 0; i < FileNameList.size(); i++)
        {
            QString tmp = FileNameList.at(i);
            Full_Path_File = Current_Path + "/" + tmp;
            if(Full_Path_File.contains("error_log_"))
            {
                QFile::remove(Full_Path_File);
            }
        }
    }
}

void MainWindow::on_checkBox_EnableMultiGPU_Waifu2xCaffe_stateChanged(int arg1)
{
    if(ui->checkBox_EnableMultiGPU_Waifu2xCaffe->isChecked())
    {
        ui->lineEdit_MultiGPUInfo_Waifu2xCaffe->setEnabled(1);
        ui->pushButton_VerifyGPUsConfig_Waifu2xCaffe->setEnabled(1);
        //===
        ui->pushButton_SplitSize_Add_Waifu2xCaffe->setEnabled(0);
        ui->pushButton_SplitSize_Minus_Waifu2xCaffe->setEnabled(0);
        ui->spinBox_BatchSize_Waifu2xCaffe->setEnabled(0);
        ui->spinBox_GPUID_Waifu2xCaffe->setEnabled(0);
        ui->spinBox_SplitSize_Waifu2xCaffe->setEnabled(0);
    }
    else
    {
        ui->lineEdit_MultiGPUInfo_Waifu2xCaffe->setEnabled(0);
        ui->pushButton_VerifyGPUsConfig_Waifu2xCaffe->setEnabled(0);
        //===
        ui->pushButton_SplitSize_Add_Waifu2xCaffe->setEnabled(1);
        ui->pushButton_SplitSize_Minus_Waifu2xCaffe->setEnabled(1);
        ui->spinBox_BatchSize_Waifu2xCaffe->setEnabled(1);
        ui->spinBox_GPUID_Waifu2xCaffe->setEnabled(1);
        ui->spinBox_SplitSize_Waifu2xCaffe->setEnabled(1);
    }
}

void MainWindow::on_comboBox_ProcessMode_Waifu2xCaffe_currentIndexChanged(int index)
{
    if(ui->comboBox_ProcessMode_Waifu2xCaffe->currentIndex()==0)
    {
        ui->checkBox_EnableMultiGPU_Waifu2xCaffe->setEnabled(0);
        ui->checkBox_EnableMultiGPU_Waifu2xCaffe->setChecked(0);
    }
    else
    {
        ui->checkBox_EnableMultiGPU_Waifu2xCaffe->setEnabled(1);
    }
}
//读取多显卡设定
QString MainWindow::Waifu2xCaffe_GetGPUInfo()
{
    GetGPUInfo_QMutex_Waifu2xCaffe.lock();
    //====
    QStringList GPU_List = ui->lineEdit_MultiGPUInfo_Waifu2xCaffe->text().trimmed().remove(" ").remove("　").split(":");
    GPU_List.removeDuplicates();
    GPU_List.removeAll("");
    //====
    int MAX_GPU_ID_Waifu2xCaffe = GPU_List.size()-1;
    if(GPU_ID_Waifu2xCaffe_GetGPUInfo>MAX_GPU_ID_Waifu2xCaffe)
    {
        GPU_ID_Waifu2xCaffe_GetGPUInfo=0;
    }
    //======
    QString GPUInfo="";
    QStringList GPUID_BatchSize_SplitSize = GPU_List.at(GPU_ID_Waifu2xCaffe_GetGPUInfo).split(",");
    GPUID_BatchSize_SplitSize.removeAll("");
    if(GPUID_BatchSize_SplitSize.size()==3)
    {
        GPUInfo = "--gpu "+GPUID_BatchSize_SplitSize.at(0).trimmed()+" -b "+GPUID_BatchSize_SplitSize.at(1).trimmed()+" -c "+GPUID_BatchSize_SplitSize.at(2).trimmed();
    }
    else
    {
        GPUInfo = "--gpu 0 -b 1 -c 64";
    }
    //======
    GPU_ID_Waifu2xCaffe_GetGPUInfo++;
    if(GPU_ID_Waifu2xCaffe_GetGPUInfo>MAX_GPU_ID_Waifu2xCaffe)
    {
        GPU_ID_Waifu2xCaffe_GetGPUInfo=0;
    }
    //======
    GetGPUInfo_QMutex_Waifu2xCaffe.unlock();
    return GPUInfo;
}

void MainWindow::on_lineEdit_MultiGPUInfo_Waifu2xCaffe_editingFinished()
{
    QString Format_str="0123456789:,";
    QString GPUs_str = ui->lineEdit_MultiGPUInfo_Waifu2xCaffe->text().trimmed().replace("：",":").replace("，",",").remove(" ").remove("　");
    for (int i=0; i<GPUs_str.length(); i++)
    {
        if(Format_str.contains(GPUs_str.at(i))==false)
        {
            GPUs_str.remove(GPUs_str.at(i));
        }
    }
    ui->lineEdit_MultiGPUInfo_Waifu2xCaffe->setText(GPUs_str);
}

void MainWindow::on_pushButton_VerifyGPUsConfig_Waifu2xCaffe_clicked()
{
    on_lineEdit_MultiGPUInfo_Waifu2xCaffe_editingFinished();
    QStringList GPU_List = ui->lineEdit_MultiGPUInfo_Waifu2xCaffe->text().trimmed().remove(" ").remove("　").split(":");
    GPU_List.removeDuplicates();
    GPU_List.removeAll("");
    //======
    QString VerRes = "";
    //======
    for (int i=0; i<GPU_List.size(); i++)
    {
        QStringList GPUID_BatchSize_SplitSize = GPU_List.at(i).split(",");
        GPUID_BatchSize_SplitSize.removeAll("");
        if(GPUID_BatchSize_SplitSize.size()==3)
        {
            VerRes.append("GPU ["+QString::number(i,10)+"]: ID:["+GPUID_BatchSize_SplitSize.at(0).trimmed()+"]"+tr(" Batch Size:[")+GPUID_BatchSize_SplitSize.at(1).trimmed()+"]"+tr(" Split Size:[")+GPUID_BatchSize_SplitSize.at(2).trimmed()+"]\n\n");
        }
        else
        {
            VerRes.append("Error:["+GPU_List.at(i)+"]\n\n");
        }
    }
    //=====
    if(VerRes=="")
    {
        VerRes="Current configuration is wrong or empty.";
    }
    //======
    QMessageBox *MSG = new QMessageBox();
    MSG->setWindowTitle(tr("GPUs List according to your configuration"));
    MSG->setText(VerRes);
    MSG->setIcon(QMessageBox::Information);
    MSG->setModal(true);
    MSG->show();
    //======
    emit Send_TextBrowser_NewMessage("\nWaifu2x-caffe GPUs List(user configuration):\n"+VerRes);
}

int MainWindow::Get_NumOfGPU_W2xCaffe()
{
    QStringList GPU_List = ui->lineEdit_MultiGPUInfo_Waifu2xCaffe->text().trimmed().remove(" ").remove("　").split(":");
    GPU_List.removeDuplicates();
    GPU_List.removeAll("");
    int GPUNum = 0;
    QStringList GPUID_BatchSize_SplitSize;
    //======
    for (int i=0; i<GPU_List.size(); i++)
    {
        GPUID_BatchSize_SplitSize = GPU_List.at(i).split(",");
        GPUID_BatchSize_SplitSize.removeAll("");
        if(GPUID_BatchSize_SplitSize.size()==3)
        {
            GPUNum++;
        }
    }
    //=====
    if(GPUNum<1)GPUNum=1;
    return GPUNum;
}
