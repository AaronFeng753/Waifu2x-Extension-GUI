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
处理图片
*/
int MainWindow::Anime4k_Image(int rowNum,bool ReProcess_MissingAlphaChannel)
{
    //============================= 读取设置 ================================
    int ScaleRatio = ui->spinBox_ScaleRatio_image->value();
    bool DelOriginal = (ui->checkBox_DelOriginal->isChecked()||ui->checkBox_ReplaceOriginalFile->isChecked());
    bool PreserveAlphaChannel = Imgae_hasAlphaChannel(rowNum);
    QString OutPutPath_Final ="";
    //========================= 拆解map得到参数 =============================
    QString status = "Processing";
    emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    QString SourceFile_fullPath = Table_model_image->item(rowNum,2)->text();
    if(!QFile::exists(SourceFile_fullPath))
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [File does not exist.]"));
        status = "Failed";
        emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, status);
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
    bool CustRes_isEnabled = false;
    int CustRes_height=0;
    int CustRes_width=0;
    if(CustRes_isContained(SourceFile_fullPath_Original))
    {
        CustRes_isEnabled=true;
        QMap<QString, QString> Res_map = CustRes_getResMap(SourceFile_fullPath_Original);//res_map["fullpath"],["height"],["width"]
        ScaleRatio = CustRes_CalNewScaleRatio(SourceFile_fullPath_Original,Res_map["height"].toInt(),Res_map["width"].toInt());
        if(ScaleRatio==0)
        {
            emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath_Original+tr("]. Error: [The resolution of the source file cannot be read, so the image cannot be scaled to a custom resolution.]"));
            status = "Failed";
            emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, status);
            emit Send_progressbar_Add();
            mutex_ThreadNumRunning.lock();
            ThreadNumRunning--;
            mutex_ThreadNumRunning.unlock();//线程数量统计-1
            return 0;
        }
        CustRes_height=Res_map["height"].toInt();
        CustRes_width=Res_map["width"].toInt();
    }
    //=======================================================
    QFileInfo fileinfo(SourceFile_fullPath);
    QString file_name = file_getBaseName(SourceFile_fullPath);
    QString file_ext = fileinfo.suffix();
    QString file_path = file_getFolderPath(fileinfo);
    QString OutPut_Path = file_path + "/" + file_name + "_waifu2x_"+QString::number(ScaleRatio, 10)+"x_"+file_ext+".png";
    //======
    QString HDNDenoiseLevel="";
    if(ui->checkBox_ACNet_Anime4K->isChecked()&&ui->checkBox_HDNMode_Anime4k->isChecked())
    {
        int HDNDenoiseLevel_value = ui->spinBox_DenoiseLevel_image->value();
        if(HDNDenoiseLevel_value>=1&&HDNDenoiseLevel_value<=3)
        {
            HDNDenoiseLevel = " -L "+QString::number(HDNDenoiseLevel_value,10)+" ";
        }
    }
    //============================== 放大 =======================================
    QString Anime4k_folder_path = Current_Path + "/Anime4K";
    QString program = Anime4k_folder_path + "/Anime4K_waifu2xEX.exe";
    //====
    QString cmd = "\"" + program + "\" -i \"" + SourceFile_fullPath + "\" -o \"" + OutPut_Path + "\" -z " + QString::number(ScaleRatio, 10) + HDNDenoiseLevel + Anime4k_ReadSettings(PreserveAlphaChannel);
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
                status = "Interrupted";
                emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, status);
                mutex_ThreadNumRunning.lock();
                ThreadNumRunning--;
                mutex_ThreadNumRunning.unlock();
                return 0;
            }
        }
        DelTrash_ForceRetry_Anime4k(OutPut_Path);//删除强制关闭导致生成的垃圾缓存文件
        if(QFile::exists(OutPut_Path))//判断是否成功输出目标文件
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
            SourceFile_fullPath = SourceFile_fullPath_Original;
        }
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
        if(!QFile::exists(OutPut_Path))
        {
            emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Unable to resize the scaled picture to the target size]"));
            status = "Failed";
            emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, status);
            emit Send_progressbar_Add();
            mutex_ThreadNumRunning.lock();
            ThreadNumRunning--;
            mutex_ThreadNumRunning.unlock();//线程数量统计-1
            return 0;
        }
        OutPutPath_Final = OutPut_Path;
    }
    if(CustRes_isEnabled)
    {
        QString OutPut_Path_CustRes = file_path + "/" + file_name + "_waifu2x_"+QString::number(CustRes_width, 10)+"x"+QString::number(CustRes_height, 10)+"_"+file_ext+".png";
        QFile::rename(OutPut_Path,OutPut_Path_CustRes);
        OutPut_Path = OutPut_Path_CustRes;
        OutPutPath_Final = OutPut_Path;
    }
    //=========================== 另存为JPG&压缩JPG ===========================================
    OutPutPath_Final = SaveImageAs_FormatAndQuality(SourceFile_fullPath_Original,OutPutPath_Final,ScaleRatio,false,0);
    //================== 检查是否丢失了透明通道 =====================
    if(ReProcess_MissingAlphaChannel==false)
    {
        QImage QImage_source(SourceFile_fullPath_Original);
        QImage QImage_Final(OutPutPath_Final);
        if(QImage_source.hasAlphaChannel()==true && QImage_Final.hasAlphaChannel()==false && ui->checkBox_AutoDetectAlphaChannel->isChecked())
        {
            QFile::remove(OutPutPath_Final);
            emit Send_TextBrowser_NewMessage(tr("Since the Alpha channel in the resulting image is lost, this image will be reprocessed to fix it:[")+SourceFile_fullPath_Original+tr("].\nIf the reprocessing happens a lot, you should consider enable [Always pre-process images with Alpha Channel] in [Additional settings]."));
            return Anime4k_Image(rowNum,true);
        }
    }
    //============================= 删除原文件 & 更新table status ============================
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
        status = "Finished, original file deleted";
        emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    }
    else
    {
        status = "Finished";
        emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, status);
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
/*
处理GIF
*/
int MainWindow::Anime4k_GIF(int rowNum)
{
    //============================= 读取设置 ================================
    int ScaleRatio = ui->spinBox_ScaleRatio_gif->value();
    bool DelOriginal = (ui->checkBox_DelOriginal->isChecked()||ui->checkBox_ReplaceOriginalFile->isChecked());
    bool OptGIF = ui->checkBox_OptGIF->isChecked();
    int Sub_gif_ThreadNumRunning = 0;
    QString OutPutPath_Final ="";
    //========================= 拆解map得到参数 =============================
    QString status = "Processing";
    emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    QString SourceFile_fullPath = Table_model_gif->item(rowNum,2)->text();
    if(!QFile::exists(SourceFile_fullPath))
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [File does not exist.]"));
        status = "Failed";
        emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        emit Send_progressbar_Add();
        mutex_ThreadNumRunning.lock();
        ThreadNumRunning--;
        mutex_ThreadNumRunning.unlock();//线程数量统计-1
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
    QString ResGIFPath = file_path + "/" + file_name + "_waifu2x_"+QString::number(ScaleRatio, 10)+"x.gif";
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
        mutex_ThreadNumRunning.unlock();//线程数量统计-1
        return 0;//如果启用stop位,则直接return
    }
    //============================== 拆分 ==========================================
    QString SplitFramesFolderPath = file_path+"/"+file_name+"_splitFrames_waifu2x";//拆分后存储frame的文件夹
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
        mutex_ThreadNumRunning.unlock();//线程数量统计-1
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
    if(ui->checkBox_ShowInterPro->isChecked())
    {
        emit Send_CurrentFileProgress_Start(file_name+"."+file_ext,Frame_fileName_list.size());
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
        if(ui->checkBox_ShowInterPro->isChecked())
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
            mutex_ThreadNumRunning.unlock();//线程数量统计-1
            return 0;//如果启用stop位,则直接return
        }
        mutex_SubThreadNumRunning.lock();
        Sub_gif_ThreadNumRunning++;
        mutex_SubThreadNumRunning.unlock();
        QString Frame_fileName = Frame_fileName_list.at(i);
        Sub_Thread_info["Frame_fileName"]=Frame_fileName;
        QtConcurrent::run(this,&MainWindow::Anime4k_GIF_scale,Sub_Thread_info,&Sub_gif_ThreadNumRunning,&Frame_failed);
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
            mutex_ThreadNumRunning.unlock();//线程数量统计-1
            return 0;//如果启用stop位,则直接return
        }
    }
    while (Sub_gif_ThreadNumRunning!=0)
    {
        Delay_msec_sleep(500);
    }
    emit Send_CurrentFileProgress_Stop();
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
        mutex_ThreadNumRunning.unlock();//线程数量统计-1
        return 0;//如果启用stop位,则直接return
    }
    //======================================== 组装 ======================================================
    if(CustRes_isEnabled)
    {
        ResGIFPath = file_path + "/" + file_name + "_waifu2x_"+QString::number(CustRes_width, 10)+"x"+QString::number(CustRes_height,10)+".gif";
    }
    Gif_assembleGif(ResGIFPath,ScaledFramesFolderPath,GIF_Duration,CustRes_isEnabled,CustRes_height,CustRes_width);
    if(!QFile::exists(ResGIFPath))
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Unable to assemble gif.]"));
        status = "Failed";
        emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        file_DelDir(SplitFramesFolderPath);
        emit Send_progressbar_Add();
        mutex_ThreadNumRunning.lock();
        ThreadNumRunning--;
        mutex_ThreadNumRunning.unlock();//线程数量统计-1
        return 0;//如果启用stop位,则直接return
    }
    OutPutPath_Final = ResGIFPath;
    //======================================= 优化gif ===================================================
    if(OptGIF)
    {
        QString ResGIFPath_compressed = "";
        if(CustRes_isEnabled)
        {
            ResGIFPath_compressed = file_path + "/" + file_name + "_waifu2x_"+QString::number(CustRes_width, 10)+"x"+QString::number(CustRes_height,10)+"_opt.gif";
        }
        else
        {
            ResGIFPath_compressed = file_path + "/" + file_name + "_waifu2x_"+QString::number(ScaleRatio, 10)+"x_opt.gif";
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
        status = "Finished, original file deleted";
        emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    }
    else
    {
        status = "Finished";
        emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    }
    //========== 移动到输出路径 =========
    if(ui->checkBox_OutPath_isEnabled->isChecked())
    {
        MoveFileToOutputPath(OutPutPath_Final,SourceFile_fullPath);
    }
    //============================ 更新进度条 =================================
    emit Send_progressbar_Add();
    //===========================  ==============================
    mutex_ThreadNumRunning.lock();
    ThreadNumRunning--;
    mutex_ThreadNumRunning.unlock();//线程数量统计-1
    return 0;
}
/*
Anime4k GIF放大子线程
放大,修改大小
*/
int MainWindow::Anime4k_GIF_scale(QMap<QString,QString> Sub_Thread_info,int *Sub_gif_ThreadNumRunning,bool *Frame_failed)
{
    QString SplitFramesFolderPath = Sub_Thread_info["SplitFramesFolderPath"];
    QString ScaledFramesFolderPath = Sub_Thread_info["ScaledFramesFolderPath"];
    QString SourceFile_fullPath = Sub_Thread_info["SourceFile_fullPath"];
    QString Frame_fileName = Sub_Thread_info["Frame_fileName"];
    //===========
    int ScaleRatio = ui->spinBox_ScaleRatio_gif->value();
    QString Frame_fileFullPath = SplitFramesFolderPath+"/"+Frame_fileName;
    //======
    QString HDNDenoiseLevel="";
    if(ui->checkBox_ACNet_Anime4K->isChecked()&&ui->checkBox_HDNMode_Anime4k->isChecked())
    {
        int HDNDenoiseLevel_value = ui->spinBox_DenoiseLevel_gif->value();
        if(HDNDenoiseLevel_value>=1&&HDNDenoiseLevel_value<=3)
        {
            HDNDenoiseLevel = " -L "+QString::number(HDNDenoiseLevel_value,10)+" ";
        }
    }
    //========================================================================
    QString Anime4k_folder_path = Current_Path + "/Anime4K";
    QString program = Anime4k_folder_path + "/Anime4K_waifu2xEX.exe";
    QString InputPath = SplitFramesFolderPath+"/"+Frame_fileName;
    QString OutputPath = ScaledFramesFolderPath+"/"+Frame_fileName;
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
    QString cmd = "\"" + program + "\" -i \"" + InputPath + "\" -o \"" + OutputPath + "\" -z " + QString::number(ScaleRatio, 10) + HDNDenoiseLevel + Anime4k_ReadSettings(false);
    //=======
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
                mutex_SubThreadNumRunning.lock();
                *Sub_gif_ThreadNumRunning=*Sub_gif_ThreadNumRunning-1;
                mutex_SubThreadNumRunning.unlock();
                return 0;
            }
        }
        DelTrash_ForceRetry_Anime4k(OutputPath);//删除强制关闭导致生成的垃圾缓存文件
        if(QFile::exists(OutputPath))
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
    //=========
    if(QFile::exists(OutputPath)==true)QFile::remove(Frame_fileFullPath);
    if(QFile::exists(OutputPath)==false)*Frame_failed=true;
    //========
    mutex_SubThreadNumRunning.lock();
    *Sub_gif_ThreadNumRunning=*Sub_gif_ThreadNumRunning-1;
    mutex_SubThreadNumRunning.unlock();
    //========
    return 0;
}
/*
#### 正常处理 ####
调用Anime4k处理视频
读取设置,拆分,创建放大子线程,组装视频
*/
int MainWindow::Anime4k_Video(int rowNum)
{
    //============================= 读取设置 ================================
    int ScaleRatio = ui->spinBox_ScaleRatio_video->value();
    bool DelOriginal = (ui->checkBox_DelOriginal->isChecked()||ui->checkBox_ReplaceOriginalFile->isChecked());
    bool isCacheExists = false;
    bool isVideoConfigChanged = true;
    int Sub_video_ThreadNumRunning = 0;
    QString OutPutPath_Final ="";
    //========================= 拆解map得到参数 =============================
    QString status = "Processing";
    emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    QString SourceFile_fullPath = Table_model_video->item(rowNum,2)->text();
    if(!QFile::exists(SourceFile_fullPath))
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [File does not exist.]"));
        status = "Failed";
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        emit Send_progressbar_Add();
        mutex_ThreadNumRunning.lock();
        ThreadNumRunning--;
        mutex_ThreadNumRunning.unlock();//线程数量统计-1
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
    if(QFile::exists(VideoConfiguration_fullPath))
    {
        QSettings *configIniRead = new QSettings(VideoConfiguration_fullPath, QSettings::IniFormat);
        configIniRead->setIniCodec(QTextCodec::codecForName("UTF-8"));
        //=================== 加载之前存储的视频信息 =========================
        int ScaleRatio_old = configIniRead->value("/VideoConfiguration/ScaleRatio").toInt();
        bool CustRes_isEnabled_old = configIniRead->value("/VideoConfiguration/CustRes_isEnabled").toBool();
        int CustRes_height_old = configIniRead->value("/VideoConfiguration/CustRes_height").toInt();
        int CustRes_width_old = configIniRead->value("/VideoConfiguration/CustRes_width").toInt();
        QString EngineName_old = configIniRead->value("/VideoConfiguration/EngineName").toString();
        bool isProcessBySegment_old = configIniRead->value("/VideoConfiguration/isProcessBySegment").toBool();
        //=================== 比对信息 ================================
        if(EngineName_old=="anime4k")
        {
            if(CustRes_isEnabled_old==false&&CustRes_isEnabled==false)
            {
                if(ScaleRatio_old!=ScaleRatio)
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
        emit Send_video_write_VideoConfiguration(VideoConfiguration_fullPath,ScaleRatio,0,CustRes_isEnabled,CustRes_height,CustRes_width,"anime4k",false,"","");
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
            if(file_ext!="mp4")
            {
                if(SourceFile_fullPath!=video_mp4_fullpath)QFile::remove(video_mp4_fullpath);
            }
            emit Send_video_write_VideoConfiguration(VideoConfiguration_fullPath,ScaleRatio,0,CustRes_isEnabled,CustRes_height,CustRes_width,"anime4k",false,"","");
            //=======
            emit Send_TextBrowser_NewMessage(tr("The previous video cache file was detected, but because you changed the settings about the video resolution, the previous cache will be deleted and processing of the video:[")+SourceFile_fullPath+tr("] will restart."));
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
        emit Send_video_write_VideoConfiguration(VideoConfiguration_fullPath,ScaleRatio,0,CustRes_isEnabled,CustRes_height,CustRes_width,"anime4k",false,"","");
        //========
    }
    //==========================================
    //                   拆分(正常)
    //==========================================
    if(!isCacheExists)
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
        QFile::remove(AudioPath);
        video_video2images(SourceFile_fullPath,SplitFramesFolderPath,AudioPath);
        if(!QFile::exists(video_mp4_fullpath))//检查是否成功生成mp4
        {
            emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Cannot convert video format to mp4.]"));
            status = "Failed";
            emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
            emit Send_progressbar_Add();
            mutex_ThreadNumRunning.lock();
            ThreadNumRunning--;
            mutex_ThreadNumRunning.unlock();//线程数量统计-1
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
            mutex_ThreadNumRunning.unlock();//线程数量统计-1
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
    if(ui->checkBox_ShowInterPro->isChecked())
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
        if(ui->checkBox_ShowInterPro->isChecked())
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
            mutex_ThreadNumRunning.unlock();//线程数量统计-1
            return 0;//如果启用stop位,则直接return
        }
        mutex_SubThreadNumRunning.lock();
        Sub_video_ThreadNumRunning++;
        mutex_SubThreadNumRunning.unlock();
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
            emit Send_progressbar_Add();
            mutex_ThreadNumRunning.lock();
            ThreadNumRunning--;
            mutex_ThreadNumRunning.unlock();//线程数量统计-1
            return 0;//如果启用stop位,则直接return
        }
    }
    while (Sub_video_ThreadNumRunning!=0)
    {
        Delay_msec_sleep(500);
    }
    emit Send_CurrentFileProgress_Stop();
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
        mutex_ThreadNumRunning.unlock();//线程数量统计-1
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
        video_mp4_scaled_fullpath = file_path+"/"+file_name+"_waifu2x_"+QString::number(ScaleRatio,10)+"x_"+file_ext+".mp4";
    }
    QFile::remove(video_mp4_scaled_fullpath);
    video_images2video(video_mp4_fullpath,video_mp4_scaled_fullpath,ScaledFramesFolderPath,AudioPath,CustRes_isEnabled,CustRes_height,CustRes_width);
    if(!QFile::exists(video_mp4_scaled_fullpath))//检查是否成功成功生成视频
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Unable to assemble pictures into videos.]"));
        status = "Failed";
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        emit Send_progressbar_Add();
        mutex_ThreadNumRunning.lock();
        ThreadNumRunning--;
        mutex_ThreadNumRunning.unlock();//线程数量统计-1
        return 0;//如果启用stop位,则直接return
    }
    OutPutPath_Final = video_mp4_scaled_fullpath;
    //============================== 删除缓存文件 ====================================================
    if(ui->checkBox_KeepVideoCache->isChecked()==false)
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
        status = "Finished, original file deleted";
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    }
    else
    {
        status = "Finished";
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    }
    //========== 移动到输出路径 =========
    if(ui->checkBox_OutPath_isEnabled->isChecked())
    {
        MoveFileToOutputPath(OutPutPath_Final,SourceFile_fullPath);
    }
    //============================ 更新进度条 =================================
    emit Send_progressbar_Add();
    //===========================  ==============================
    mutex_ThreadNumRunning.lock();
    ThreadNumRunning--;
    mutex_ThreadNumRunning.unlock();//线程数量统计-1
    return 0;
}
/*
#### 分段处理 ####
调用Anime4k处理视频
读取设置,拆分,创建放大子线程,组装视频
*/
int MainWindow::Anime4k_Video_BySegment(int rowNum)
{
    //============================= 读取设置 ================================
    int ScaleRatio = ui->spinBox_ScaleRatio_video->value();
    bool DelOriginal = (ui->checkBox_DelOriginal->isChecked()||ui->checkBox_ReplaceOriginalFile->isChecked());
    bool isCacheExists = false;
    bool isVideoConfigChanged = true;
    int Sub_video_ThreadNumRunning = 0;
    QString OutPutPath_Final ="";
    int SegmentDuration = ui->spinBox_SegmentDuration->value();
    //========================= 拆解map得到参数 =============================
    QString status = "Processing";
    emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    QString SourceFile_fullPath = Table_model_video->item(rowNum,2)->text();
    if(!QFile::exists(SourceFile_fullPath))
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [File does not exist.]"));
        status = "Failed";
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        emit Send_progressbar_Add();
        mutex_ThreadNumRunning.lock();
        ThreadNumRunning--;
        mutex_ThreadNumRunning.unlock();//线程数量统计-1
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
    if(QFile::exists(VideoConfiguration_fullPath))
    {
        QSettings *configIniRead = new QSettings(VideoConfiguration_fullPath, QSettings::IniFormat);
        configIniRead->setIniCodec(QTextCodec::codecForName("UTF-8"));
        //=================== 加载之前存储的视频信息 =========================
        int ScaleRatio_old = configIniRead->value("/VideoConfiguration/ScaleRatio").toInt();
        bool CustRes_isEnabled_old = configIniRead->value("/VideoConfiguration/CustRes_isEnabled").toBool();
        int CustRes_height_old = configIniRead->value("/VideoConfiguration/CustRes_height").toInt();
        int CustRes_width_old = configIniRead->value("/VideoConfiguration/CustRes_width").toInt();
        QString EngineName_old = configIniRead->value("/VideoConfiguration/EngineName").toString();
        bool isProcessBySegment_old = configIniRead->value("/VideoConfiguration/isProcessBySegment").toBool();
        //=================== 比对信息 ================================
        if(EngineName_old=="anime4k")
        {
            if(CustRes_isEnabled_old==false&&CustRes_isEnabled==false)
            {
                if(ScaleRatio_old!=ScaleRatio)
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
        file_mkDir(VideoClipsFolderPath_old);
        if(file_isDirExist(VideoClipsFolderPath_old)==true)
        {
            VideoClipsFolderPath = VideoClipsFolderPath_old;
            VideoClipsFolderName = VideoClipsFolderName_old;
        }
    }
    else
    {
        emit Send_video_write_VideoConfiguration(VideoConfiguration_fullPath,ScaleRatio,0,CustRes_isEnabled,CustRes_height,CustRes_width,"anime4k",true,VideoClipsFolderPath,VideoClipsFolderName);
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
            if(file_ext!="mp4")
            {
                if(SourceFile_fullPath!=video_mp4_fullpath)QFile::remove(video_mp4_fullpath);
            }
            emit Send_video_write_VideoConfiguration(VideoConfiguration_fullPath,ScaleRatio,0,CustRes_isEnabled,CustRes_height,CustRes_width,"anime4k",true,VideoClipsFolderPath,VideoClipsFolderName);
            //=======
            emit Send_TextBrowser_NewMessage(tr("The previous video cache file was detected, but because you changed the settings about the video resolution, the previous cache will be deleted and processing of the video:[")+SourceFile_fullPath+tr("] will restart."));
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
        emit Send_video_write_VideoConfiguration(VideoConfiguration_fullPath,ScaleRatio,0,CustRes_isEnabled,CustRes_height,CustRes_width,"anime4k",true,VideoClipsFolderPath,VideoClipsFolderName);
        //========
    }
    /*====================================
                将视频转换为MP4
    ======================================*/
    if(!QFile::exists(video_mp4_fullpath))
    {
        video_2mp4(SourceFile_fullPath);
        if(!QFile::exists(video_mp4_fullpath))//检查是否成功生成mp4
        {
            emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Cannot convert video format to mp4.]"));
            status = "Failed";
            emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
            emit Send_progressbar_Add();
            mutex_ThreadNumRunning.lock();
            ThreadNumRunning--;
            mutex_ThreadNumRunning.unlock();//线程数量统计-1
            return 0;//如果启用stop位,则直接return
        }
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
    if(QFile::exists(VideoConfiguration_fullPath))
    {
        QSettings *configIniRead = new QSettings(VideoConfiguration_fullPath, QSettings::IniFormat);
        configIniRead->setIniCodec(QTextCodec::codecForName("UTF-8"));
        //=================== 加载进度 =========================
        StartTime = configIniRead->value("/Progress/StartTime").toInt();
        isSplitComplete = configIniRead->value("/Progress/isSplitComplete").toBool();
        isScaleComplete = configIniRead->value("/Progress/isScaleComplete").toBool();
        OLD_SegmentDuration = configIniRead->value("/Progress/OLDSegmentDuration").toInt();
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
                    status = "Failed";
                    emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
                    emit Send_progressbar_Add();
                    mutex_ThreadNumRunning.lock();
                    ThreadNumRunning--;
                    mutex_ThreadNumRunning.unlock();//线程数量统计-1
                    return 0;//如果启用stop位,则直接return
                }
            }
            /*
            记录进度
            帧拆分成功
            */
            emit Send_video_write_Progress_ProcessBySegment(VideoConfiguration_fullPath,StartTime,true,false,SegmentDuration_tmp);
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
            if(ui->checkBox_ShowInterPro->isChecked())
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
                if(ui->checkBox_ShowInterPro->isChecked())
                {
                    InterPro_now++;
                    emit Send_TextBrowser_NewMessage(tr("File name:[")+SourceFile_fullPath+tr("]  Scale progress:[")+QString::number(InterPro_now,10)+"/"+QString::number(InterPro_total,10)+tr("] Duration progress:[")+QString::number(StartTime,10)+"s/"+QString::number(VideoDuration,10)+"s]");
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
                    mutex_ThreadNumRunning.unlock();//线程数量统计-1
                    return 0;//如果启用stop位,则直接return
                }
                mutex_SubThreadNumRunning.lock();
                Sub_video_ThreadNumRunning++;
                mutex_SubThreadNumRunning.unlock();
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
                    emit Send_progressbar_Add();
                    mutex_ThreadNumRunning.lock();
                    ThreadNumRunning--;
                    mutex_ThreadNumRunning.unlock();//线程数量统计-1
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
                mutex_ThreadNumRunning.unlock();//线程数量统计-1
                return 0;//如果启用stop位,则直接return
            }
            /*
            记录进度
            帧处理成功
            */
            emit Send_video_write_Progress_ProcessBySegment(VideoConfiguration_fullPath,StartTime,true,true,SegmentDuration_tmp);
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
        if(!QFile::exists(video_mp4_scaled_clip_fullpath))//检查是否成功成功生成视频
        {
            emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Unable to assemble pictures into videos.]"));
            status = "Failed";
            emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
            emit Send_progressbar_Add();
            mutex_ThreadNumRunning.lock();
            ThreadNumRunning--;
            mutex_ThreadNumRunning.unlock();//线程数量统计-1
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
        emit Send_video_write_Progress_ProcessBySegment(VideoConfiguration_fullPath,StartTime,false,false,-1);
    }
    emit Send_CurrentFileProgress_Stop();
    //======================================================
    //                    组装(片段到成片)
    //======================================================
    QString video_mp4_scaled_fullpath = "";
    if(CustRes_isEnabled)
    {
        video_mp4_scaled_fullpath = file_path+"/"+file_name+"_waifu2x_"+QString::number(CustRes_width,10)+"x"+QString::number(CustRes_height,10)+"_"+file_ext+".mp4";
    }
    else
    {
        video_mp4_scaled_fullpath = file_path+"/"+file_name+"_waifu2x_"+QString::number(ScaleRatio,10)+"x_"+file_ext+".mp4";
    }
    QFile::remove(video_mp4_scaled_fullpath);
    video_AssembleVideoClips(VideoClipsFolderPath,VideoClipsFolderName,video_mp4_scaled_fullpath,AudioPath);
    if(!QFile::exists(video_mp4_scaled_fullpath))//检查是否成功生成视频
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Unable to assemble video clips.]"));
        status = "Failed";
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        emit Send_progressbar_Add();
        mutex_ThreadNumRunning.lock();
        ThreadNumRunning--;
        mutex_ThreadNumRunning.unlock();//线程数量统计-1
        return 0;//如果启用stop位,则直接return
    }
    OutPutPath_Final = video_mp4_scaled_fullpath;
    //============================== 删除缓存文件 ====================================================
    if(ui->checkBox_KeepVideoCache->isChecked()==false)
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
        status = "Finished, original file deleted";
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    }
    else
    {
        status = "Finished";
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    }
    //========== 移动到输出路径 =========
    if(ui->checkBox_OutPath_isEnabled->isChecked())
    {
        MoveFileToOutputPath(OutPutPath_Final,SourceFile_fullPath);
    }
    //============================ 更新进度条 =================================
    emit Send_progressbar_Add();
    //===========================  ==============================
    mutex_ThreadNumRunning.lock();
    ThreadNumRunning--;
    mutex_ThreadNumRunning.unlock();//线程数量统计-1
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
    //======
    QString HDNDenoiseLevel="";
    if(ui->checkBox_ACNet_Anime4K->isChecked()&&ui->checkBox_HDNMode_Anime4k->isChecked())
    {
        int HDNDenoiseLevel_value = ui->spinBox_DenoiseLevel_video->value();
        if(HDNDenoiseLevel_value>=1&&HDNDenoiseLevel_value<=3)
        {
            HDNDenoiseLevel = " -L "+QString::number(HDNDenoiseLevel_value,10)+" ";
        }
    }
    //========================================================================
    QString Anime4k_folder_path = Current_Path + "/Anime4K";
    QString program = Anime4k_folder_path + "/Anime4K_waifu2xEX.exe";
    QString InputPath = SplitFramesFolderPath+"/"+Frame_fileName;
    QString OutputPath = ScaledFramesFolderPath+"/"+Frame_fileName;
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
    QString cmd = "\"" + program + "\" -i \"" + InputPath + "\" -o \"" + OutputPath + "\" -z " + QString::number(ScaleRatio, 10) + HDNDenoiseLevel + Anime4k_ReadSettings(false);
    //=======
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
                mutex_SubThreadNumRunning.lock();
                *Sub_video_ThreadNumRunning=*Sub_video_ThreadNumRunning-1;
                mutex_SubThreadNumRunning.unlock();
                return 0;
            }
        }
        DelTrash_ForceRetry_Anime4k(OutputPath);//删除强制关闭导致生成的垃圾缓存文件
        if(QFile::exists(OutputPath))
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
    //=========
    if(QFile::exists(OutputPath)==true)QFile::remove(Frame_fileFullPath);
    if(QFile::exists(OutputPath)==false)*Frame_failed=true;
    //========
    mutex_SubThreadNumRunning.lock();
    *Sub_video_ThreadNumRunning=*Sub_video_ThreadNumRunning-1;
    mutex_SubThreadNumRunning.unlock();
    //========
    return 0;
}
/*
Anime4k
读取配置生成配置string
*/
QString MainWindow::Anime4k_ReadSettings(bool PreserveAlphaChannel)
{
    QString Anime4k_Settings_str = " ";
    Anime4k_Settings_str.append("-M opencl ");//强制使用OpenCL(因为自行编译的版本中禁用了cuda,我没英伟达显卡
    //保留透明通道
    if(PreserveAlphaChannel)
    {
        Anime4k_Settings_str.append("-A ");//保留透明通道
    }
    //ACNet
    if(ui->checkBox_ACNet_Anime4K->isChecked())
    {
        Anime4k_Settings_str.append("-w ");//启用acnet
        if(ui->checkBox_GPUMode_Anime4K->isChecked())
        {
            Anime4k_Settings_str.append("-q ");//启用显卡
        }
        if(ui->checkBox_HDNMode_Anime4k->isChecked())
        {
            Anime4k_Settings_str.append("-H ");//启用HDN
        }
        if(ui->checkBox_SpecifyGPU_Anime4k->isChecked())
        {
            Anime4k_Settings_str.append(Anime4k_GetGPUInfo()+" ");//指定显卡
        }
        if(ui->checkBox_FastMode_Anime4K->isChecked())
        {
            Anime4k_Settings_str.append("-f ");//快速模式
        }
        return Anime4k_Settings_str;
    }
    //快速模式
    if(ui->checkBox_FastMode_Anime4K->isChecked())
    {
        Anime4k_Settings_str.append("-f ");
    }
    //GPU加速
    if(ui->checkBox_GPUMode_Anime4K->isChecked())
    {
        Anime4k_Settings_str.append("-q ");
    }
    //指定GPU
    if(ui->checkBox_SpecifyGPU_Anime4k->isChecked())
    {
        Anime4k_Settings_str.append(Anime4k_GetGPUInfo()+" ");
    }
    //Passes
    Anime4k_Settings_str.append("-p "+QString::number(ui->spinBox_Passes_Anime4K->value(),10)+" ");
    //Push color count
    Anime4k_Settings_str.append("-n "+QString::number(ui->spinBox_PushColorCount_Anime4K->value(),10)+" ");
    //Push color strength
    Anime4k_Settings_str.append("-c "+QString::number(ui->doubleSpinBox_PushColorStrength_Anime4K->value(),'f',2)+" ");
    //Push gradient strength
    Anime4k_Settings_str.append("-g "+QString::number(ui->doubleSpinBox_PushGradientStrength_Anime4K->value(),'f',2)+" ");
    //Pre-processing
    if(ui->checkBox_EnablePreProcessing_Anime4k->isChecked())
    {
        Anime4k_Settings_str.append("-b ");
        uint8_t PreProcessingFilters = 0;
        //读取滤镜选择情况
        if (ui->checkBox_MedianBlur_Pre_Anime4k->isChecked())
        {
            PreProcessingFilters|=1;
        }
        if (ui->checkBox_MeanBlur_Pre_Anime4k->isChecked())
        {
            PreProcessingFilters|=2;
        }
        if (ui->checkBox_CASSharping_Pre_Anime4k->isChecked())
        {
            PreProcessingFilters|=4;
        }
        if (ui->checkBox_GaussianBlurWeak_Pre_Anime4k->isChecked())
        {
            PreProcessingFilters|=8;
        }
        if (ui->checkBox_GaussianBlur_Pre_Anime4k->isChecked())
        {
            PreProcessingFilters|=16;
        }
        if (ui->checkBox_BilateralFilter_Pre_Anime4k->isChecked())
        {
            PreProcessingFilters|=32;
        }
        if (ui->checkBox_BilateralFilterFaster_Pre_Anime4k->isChecked())
        {
            PreProcessingFilters|=64;
        }
        //生成添加滤镜指令
        if(PreProcessingFilters!=0)
        {
            Anime4k_Settings_str.append("-r "+QString::number(PreProcessingFilters,10)+" ");
        }
        else
        {
            Anime4k_Settings_str.append("-r 4 ");
        }
    }
    //Post-processing
    if(ui->checkBox_EnablePostProcessing_Anime4k->isChecked())
    {
        Anime4k_Settings_str.append("-a ");
        uint8_t PostProcessingFilters = 0;
        //读取滤镜选择情况
        if (ui->checkBox_MedianBlur_Post_Anime4k->isChecked())
        {
            PostProcessingFilters|=1;
        }
        if (ui->checkBox_MeanBlur_Post_Anime4k->isChecked())
        {
            PostProcessingFilters|=2;
        }
        if (ui->checkBox_CASSharping_Post_Anime4k->isChecked())
        {
            PostProcessingFilters|=4;
        }
        if (ui->checkBox_GaussianBlurWeak_Post_Anime4k->isChecked())
        {
            PostProcessingFilters|=8;
        }
        if (ui->checkBox_GaussianBlur_Post_Anime4k->isChecked())
        {
            PostProcessingFilters|=16;
        }
        if (ui->checkBox_BilateralFilter_Post_Anime4k->isChecked())
        {
            PostProcessingFilters|=32;
        }
        if (ui->checkBox_BilateralFilterFaster_Post_Anime4k->isChecked())
        {
            PostProcessingFilters|=64;
        }
        //生成添加滤镜指令
        if(PostProcessingFilters!=0)
        {
            Anime4k_Settings_str.append("-e "+QString::number(PostProcessingFilters,10)+" ");
        }
        else
        {
            Anime4k_Settings_str.append("-e 4 ");
        }
    }
    return Anime4k_Settings_str;
}

QString MainWindow::Anime4k_GetGPUInfo()
{
    GetGPUInfo_QMutex_Anime4k.lock();
    //====
    QStringList GPU_List = ui->lineEdit_GPUs_Anime4k->text().trimmed().remove(" ").remove("　").split(":");
    GPU_List.removeDuplicates();
    GPU_List.removeAll("");
    //====
    int MAX_GPU_ID_Anime4k = GPU_List.count()-1;
    if(GPU_ID_Anime4k_GetGPUInfo>MAX_GPU_ID_Anime4k)
    {
        GPU_ID_Anime4k_GetGPUInfo=0;
    }
    //======
    QString GPUInfo="";
    QStringList PID_DID = GPU_List.at(GPU_ID_Anime4k_GetGPUInfo).split(",");
    if(PID_DID.count()==2)
    {
        GPUInfo = "-h "+PID_DID.at(0).trimmed()+" -d "+PID_DID.at(1).trimmed();
    }
    else
    {
        GPUInfo = "-h 0 -d 0";
    }
    //======
    GPU_ID_Anime4k_GetGPUInfo++;
    if(GPU_ID_Anime4k_GetGPUInfo>MAX_GPU_ID_Anime4k)
    {
        GPU_ID_Anime4k_GetGPUInfo=0;
    }
    //======
    GetGPUInfo_QMutex_Anime4k.unlock();
    return GPUInfo;
}
/*
列出可用的GPUs
*/
void MainWindow::on_pushButton_ListGPUs_Anime4k_clicked()
{
    QtConcurrent::run(this, &MainWindow::ListGPUs_Anime4k);
}

void MainWindow::ListGPUs_Anime4k()
{
    QString cmd = "\"" + Current_Path + "/Anime4K/Anime4K_waifu2xEX.exe" + "\" -l";
    ExecuteCMD_batFile(cmd.replace("%","%%")+"\n title = GPU List for Anime4K @ Waifu2x-Extension-GUI\n pause");
}

void MainWindow::on_lineEdit_GPUs_Anime4k_editingFinished()
{
    QString Format_str="0123456789:,";
    QString GPUs_str = ui->lineEdit_GPUs_Anime4k->text().trimmed().replace("：",":").replace("，",",").remove(" ").remove("　");
    for (int i=0; i<GPUs_str.length(); i++)
    {
        if(Format_str.contains(GPUs_str.at(i))==false)
        {
            GPUs_str.remove(GPUs_str.at(i));
        }
    }
    ui->lineEdit_GPUs_Anime4k->setText(GPUs_str);
}

void MainWindow::on_pushButton_VerifyGPUsConfig_Anime4k_clicked()
{
    on_lineEdit_GPUs_Anime4k_editingFinished();
    QStringList GPU_List = ui->lineEdit_GPUs_Anime4k->text().trimmed().remove(" ").remove("　").split(":");
    GPU_List.removeDuplicates();
    GPU_List.removeAll("");
    //======
    QString VerRes = "";
    //======
    for (int i=0; i<GPU_List.count(); i++)
    {
        QStringList PID_DID = GPU_List.at(i).split(",");
        if(PID_DID.count()==2)
        {
            VerRes.append("GPU ["+QString::number(i,10)+tr("]: Platform ID:[")+PID_DID.at(0).trimmed()+"]"+tr(" Device ID:[")+PID_DID.at(1).trimmed()+"]\n\n");
        }
        else
        {
            VerRes.append("Error\n\n");
        }
    }
    //=====
    if(VerRes=="")
    {
        VerRes=tr("Current configuration is wrong or empty.");
    }
    //======
    QMessageBox *MSG = new QMessageBox();
    MSG->setWindowTitle(tr("GPUs List according to your configuration"));
    MSG->setText(VerRes);
    MSG->setIcon(QMessageBox::Information);
    MSG->setModal(true);
    MSG->show();
    //======
    emit Send_TextBrowser_NewMessage(tr("\nAnime4k GPUs List(user configuration):\n")+VerRes.trimmed());
}

/*
删除强制重试时生成的垃圾缓存文件
*/
void MainWindow::DelTrash_ForceRetry_Anime4k(QString OutPut_Path)
{
    if(isForceRetryClicked)//判断是否触发强制重试,若触发,直接删除垃圾缓存
    {
        QFile::remove(OutPut_Path);
        isForceRetryClicked_QMutex.lock();
        isForceRetryClicked=false;
        isForceRetryClicked_QMutex.unlock();
    }
    return;
}
//阻断其他线程重置是否强制重试的标记,并检测anime4k是否正在运行
void MainWindow::isForceRetryClicked_SetTrue_Block_Anime4k()
{
    isForceRetryClicked_QMutex.lock();
    isForceRetryClicked=true;
    Delay_sec_sleep(5);
    QProcess Get_tasklist;
    do
    {
        Get_tasklist.start("tasklist");
        while(!Get_tasklist.waitForStarted(100)) {}
        while(!Get_tasklist.waitForFinished(100)) {}
        if(Get_tasklist.readAllStandardOutput().contains("Anime4K_waifu2xEX.exe")==false)
        {
            break;
        }
        Delay_sec_sleep(1);
    }
    while(true);
    isForceRetryClicked_QMutex.unlock();
    emit Send_SetEnable_pushButton_ForceRetry_self();
}

void MainWindow::DenoiseLevelSpinboxSetting_Anime4k()
{
    if(ui->checkBox_ACNet_Anime4K->isChecked()&&ui->checkBox_HDNMode_Anime4k->isChecked())
    {
        if(ui->comboBox_Engine_Image->currentIndex()==3)
        {
            ui->spinBox_DenoiseLevel_image->setRange(1,3);
            ui->spinBox_DenoiseLevel_image->setValue(1);
            ui->spinBox_DenoiseLevel_image->setEnabled(1);
            ui->spinBox_DenoiseLevel_image->setToolTip(tr("Range:1~3"));
            ui->label_ImageDenoiseLevel->setToolTip(tr("Range:1~3"));
        }
        if(ui->comboBox_Engine_GIF->currentIndex()==3)
        {
            ui->spinBox_DenoiseLevel_gif->setRange(1,3);
            ui->spinBox_DenoiseLevel_gif->setValue(1);
            ui->spinBox_DenoiseLevel_gif->setEnabled(1);
            ui->spinBox_DenoiseLevel_gif->setToolTip(tr("Range:1~3"));
            ui->label_GIFDenoiseLevel->setToolTip(tr("Range:1~3"));
        }
        if(ui->comboBox_Engine_Video->currentIndex()==2)
        {
            ui->spinBox_DenoiseLevel_video->setRange(1,3);
            ui->spinBox_DenoiseLevel_video->setValue(1);
            ui->spinBox_DenoiseLevel_video->setEnabled(1);
            ui->spinBox_DenoiseLevel_video->setToolTip(tr("Range:1~3"));
            ui->label_VideoDenoiseLevel->setToolTip(tr("Range:1~3"));
        }
    }
    else
    {
        if(ui->comboBox_Engine_Image->currentIndex()==3)
        {
            ui->spinBox_DenoiseLevel_image->setRange(-1,0);
            ui->spinBox_DenoiseLevel_image->setValue(-1);
            ui->spinBox_DenoiseLevel_image->setEnabled(0);
            ui->spinBox_DenoiseLevel_image->setToolTip(tr("Due to current settings, you can\'t adjust denoise level."));
            ui->label_ImageDenoiseLevel->setToolTip(tr("Due to current settings, you can\'t adjust denoise level."));
        }
        if(ui->comboBox_Engine_GIF->currentIndex()==3)
        {
            ui->spinBox_DenoiseLevel_gif->setRange(-1,0);
            ui->spinBox_DenoiseLevel_gif->setValue(-1);
            ui->spinBox_DenoiseLevel_gif->setEnabled(0);
            ui->spinBox_DenoiseLevel_gif->setToolTip(tr("Due to current settings, you can\'t adjust denoise level."));
            ui->label_GIFDenoiseLevel->setToolTip(tr("Due to current settings, you can\'t adjust denoise level."));
        }
        if(ui->comboBox_Engine_Video->currentIndex()==2)
        {
            ui->spinBox_DenoiseLevel_video->setRange(-1,0);
            ui->spinBox_DenoiseLevel_video->setValue(-1);
            ui->spinBox_DenoiseLevel_video->setEnabled(0);
            ui->spinBox_DenoiseLevel_video->setToolTip(tr("Due to current settings, you can\'t adjust denoise level."));
            ui->label_VideoDenoiseLevel->setToolTip(tr("Due to current settings, you can\'t adjust denoise level."));
        }
    }
}

