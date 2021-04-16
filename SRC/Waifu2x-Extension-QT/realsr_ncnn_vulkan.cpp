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

int MainWindow::Realsr_NCNN_Vulkan_Image(int rowNum,bool ReProcess_MissingAlphaChannel)
{
    //============================= 读取设置 ================================
    bool DelOriginal = (ui->checkBox_DelOriginal->isChecked()||ui->checkBox_ReplaceOriginalFile->isChecked());
    QString OutPutPath_Final ="";
    //========================= 拆解map得到参数 =============================
    //将状态设定到处理中
    emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, "Processing");
    //得到原文件路径
    QString SourceFile_fullPath = Table_model_image->item(rowNum,2)->text();
    if(!QFile::exists(SourceFile_fullPath))//判断源文件是否存在
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
    //===============
    int ScaleRatio=2;
    bool CustRes_isEnabled = false;
    int CustRes_height=0;
    int CustRes_width=0;
    //检查是是否有自定义分辨率
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
    //===============
    QFileInfo fileinfo(SourceFile_fullPath);
    QString file_name = file_getBaseName(SourceFile_fullPath);
    QString file_ext = fileinfo.suffix();
    QString file_path = file_getFolderPath(fileinfo);
    QString OutPut_Path = file_path + "/" + file_name + "_waifu2x_"+QString::number(ScaleRatio, 10)+"x_"+file_ext+".png";
    //============================== 放大 =======================================
    QProcess *Waifu2x = new QProcess();
    QString program = Current_Path+"/realsr-ncnn-vulkan/realsr-ncnn-vulkan_waifu2xEX.exe";
    //==========
    int ScaleRatio_tmp=Calculate_Temporary_ScaleRatio_RealsrNCNNVulkan(ScaleRatio);
    QString InputPath_tmp = SourceFile_fullPath;
    QString OutputPath_tmp ="";
    for(int retry=0; retry<(ui->spinBox_retry->value()+ForceRetryCount); retry++)
    {
        bool waifu2x_qprocess_failed = false;
        InputPath_tmp = SourceFile_fullPath;
        OutputPath_tmp ="";
        for(int i=4; i<=ScaleRatio_tmp; i*=4)
        {
            QString ErrorMSG="";
            QString StanderMSG="";
            //==========================
            OutputPath_tmp = file_path + "/" + file_name + "_waifu2x_"+QString::number(i, 10)+"x_"+file_ext+".png";
            QString cmd = "\"" + program + "\"" + " -i " + "\"" + InputPath_tmp + "\"" + " -o " + "\"" + OutputPath_tmp + "\"" + " -s " + "4" + Realsr_NCNN_Vulkan_ReadSettings();
            Waifu2x->start(cmd);
            while(!Waifu2x->waitForStarted(100)&&!QProcess_stop) {}
            while(!Waifu2x->waitForFinished(500)&&!QProcess_stop)
            {
                //判断用户是否暂停处理
                if(waifu2x_STOP)
                {
                    Waifu2x->close();
                    if(i>4)
                    {
                        QFile::remove(InputPath_tmp);
                    }
                    QFile::remove(OutputPath_tmp);
                    emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, "Interrupted");
                    mutex_ThreadNumRunning.lock();
                    ThreadNumRunning--;
                    mutex_ThreadNumRunning.unlock();
                    return 0;
                }
                //读取输出判断是否出错
                ErrorMSG.append(Waifu2x->readAllStandardError().toLower());
                StanderMSG.append(Waifu2x->readAllStandardOutput().toLower());
                if(ErrorMSG.contains("failed")||StanderMSG.contains("failed"))
                {
                    waifu2x_qprocess_failed = true;
                    Waifu2x->close();
                    if(i>4)
                    {
                        QFile::remove(InputPath_tmp);
                    }
                    QFile::remove(OutputPath_tmp);
                    break;
                }
            }
            //===============
            if(waifu2x_qprocess_failed)break;
            //===============
            ErrorMSG.append(Waifu2x->readAllStandardError().toLower());
            StanderMSG.append(Waifu2x->readAllStandardOutput().toLower());
            if(ErrorMSG.contains("failed")||StanderMSG.contains("failed"))
            {
                waifu2x_qprocess_failed = true;
                if(i>4)
                {
                    QFile::remove(InputPath_tmp);
                }
                QFile::remove(OutputPath_tmp);
                break;
            }
            //===============
            if(i>4)
            {
                QFile::remove(InputPath_tmp);
            }
            InputPath_tmp = OutputPath_tmp;
        }
        //========= 检测是否成功,是否需要重试 ============
        if(QFile::exists(OutputPath_tmp)&&!waifu2x_qprocess_failed)
        {
            break;
        }
        else
        {
            QFile::remove(OutputPath_tmp);
            if(retry==ui->spinBox_retry->value()+(ForceRetryCount-1))break;
            emit Send_TextBrowser_NewMessage(tr("Automatic retry, please wait."));
            Delay_sec_sleep(5);
        }
    }
    if(!QFile::exists(OutputPath_tmp))
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
    OutPutPath_Final = OutputPath_tmp;
    //============================ 调整大小 ====================================================
    if(ScaleRatio_tmp != ScaleRatio||CustRes_isEnabled)
    {
        int New_height=0;
        int New_width=0;
        if(CustRes_isEnabled)
        {
            New_height= CustRes_height;
            New_width= CustRes_width;
        }
        else
        {
            QMap<QString,int> res_map_OriginalFile = Image_Gif_Read_Resolution(SourceFile_fullPath);
            New_height = res_map_OriginalFile["height"]*ScaleRatio;
            New_width = res_map_OriginalFile["width"]*ScaleRatio;
        }
        QImage qimage_adj(OutputPath_tmp);
        //读取放大后的图片并调整大小
        QImage qimage_adj_scaled = qimage_adj.scaled(New_width,New_height,CustRes_AspectRatioMode,Qt::SmoothTransformation);
        QImageWriter qimageW_adj;
        qimageW_adj.setFormat("png");
        qimageW_adj.setFileName(OutPut_Path);
        if(qimageW_adj.canWrite())
        {
            qimageW_adj.write(qimage_adj_scaled);
        }
        QFile::remove(OutputPath_tmp);
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
        OutPutPath_Final = OutPut_Path;
    }
    if(CustRes_isEnabled)
    {
        QString OutPut_Path_CustRes = file_path + "/" + file_name + "_waifu2x_"+QString::number(CustRes_width, 10)+"x"+QString::number(CustRes_height, 10)+"_"+file_ext+".png";
        QFile::remove(OutPut_Path_CustRes);
        QFile::rename(OutPut_Path,OutPut_Path_CustRes);
        OutPutPath_Final = OutPut_Path_CustRes;
    }
    //=========================== 另存为JPG&压缩JPG ===========================================
    OutPutPath_Final = SaveImageAs_FormatAndQuality(SourceFile_fullPath_Original,OutPutPath_Final,false,0);
    //================== 检查是否丢失了透明通道 =====================
    if(ReProcess_MissingAlphaChannel==false)
    {
        QImage QImage_source(SourceFile_fullPath_Original);
        QImage QImage_Final(OutPutPath_Final);
        if(QImage_source.hasAlphaChannel()==true && QImage_Final.hasAlphaChannel()==false && ui->checkBox_AutoDetectAlphaChannel->isChecked())
        {
            QFile::remove(OutPutPath_Final);
            emit Send_TextBrowser_NewMessage(tr("Since the Alpha channel in the resulting image is lost, this image will be reprocessed to fix it:[")+SourceFile_fullPath_Original+tr("].\nIf the reprocessing happens a lot, you should consider enable [Always pre-process images with Alpha Channel] in [Additional settings]."));
            return Realsr_NCNN_Vulkan_Image(rowNum,true);
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

/*

  =============================================================================================
  =============================================================================================
  ============================= Waifu2x_NCNN_Vulkan_GIF =======================================
  =============================================================================================
  =============================================================================================

*/

int MainWindow::Realsr_NCNN_Vulkan_GIF(int rowNum)
{
    //============================= 读取设置 ================================
    int ScaleRatio = ui->doubleSpinBox_ScaleRatio_gif->value();
    bool DelOriginal = (ui->checkBox_DelOriginal->isChecked()||ui->checkBox_ReplaceOriginalFile->isChecked());
    bool OptGIF = ui->checkBox_OptGIF->isChecked();
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
    QString ResGIFPath = file_path + "/" + file_name + "_waifu2x_"+QString::number(ScaleRatio, 10)+"x.gif";
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
    //读取放大倍数
    int ScaleRatio_Original = 2;
    if(CustRes_isEnabled == true)
    {
        ScaleRatio_Original = CustRes_CalNewScaleRatio(SourceFile_fullPath,CustRes_height,CustRes_width);
    }
    else
    {
        ScaleRatio_Original = ui->doubleSpinBox_ScaleRatio_gif->value();
    }
    int ScaleRatio_Max=Calculate_Temporary_ScaleRatio_RealsrNCNNVulkan(ScaleRatio_Original);
    bool isOverScaled = (ScaleRatio_Max!=ScaleRatio_Original);
    //初始化进度讯息
    int NumOfSplitFrames = Frame_fileName_list.size();
    if(ui->checkBox_ShowInterPro->isChecked())
    {
        int NumOfRounds=0;
        for(int ScaleRatio_Current_tmp=4; ScaleRatio_Current_tmp<=ScaleRatio_Max; ScaleRatio_Current_tmp*=4)
        {
            NumOfRounds++;
        }
        emit Send_CurrentFileProgress_Start(file_name+"."+file_ext,NumOfSplitFrames*NumOfRounds);
    }
    //读取配置讯息
    QString Realsr_NCNN_Vulkan_Settings_str = Realsr_NCNN_Vulkan_ReadSettings_Video_GIF(ui->spinBox_ThreadNum_gif_internal->value());
    QProcess *Waifu2x = new QProcess();
    QString program = Current_Path+"/realsr-ncnn-vulkan/realsr-ncnn-vulkan_waifu2xEX.exe";
    bool waifu2x_qprocess_failed = false;
    int CountFinishedRounds=0;
    //====
    for(int ScaleRatio_Current_tmp=4; ScaleRatio_Current_tmp<=ScaleRatio_Max && Frame_fileName_list.isEmpty()==false; ScaleRatio_Current_tmp*=4)
    {
        for(int retry=0; retry<(ui->spinBox_retry->value()+ForceRetryCount); retry++)
        {
            QString ErrorMSG="";
            QString StanderMSG="";
            //==========
            waifu2x_qprocess_failed = false;
            QString cmd = "\"" + program + "\"" + " -i " + "\"" + SplitFramesFolderPath + "\"" + " -o " + "\"" + ScaledFramesFolderPath + "\"" + " -s 4 " + Realsr_NCNN_Vulkan_Settings_str;
            Waifu2x->start(cmd);
            while(!Waifu2x->waitForStarted(100)&&!QProcess_stop) {}
            while(!Waifu2x->waitForFinished(650)&&!QProcess_stop)
            {
                if(waifu2x_STOP)
                {
                    Waifu2x->close();
                    file_DelDir(SplitFramesFolderPath);
                    emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, "Interrupted");
                    mutex_ThreadNumRunning.lock();
                    ThreadNumRunning--;
                    mutex_ThreadNumRunning.unlock();
                    return 0;
                }
                ErrorMSG.append(Waifu2x->readAllStandardError().toLower());
                StanderMSG.append(Waifu2x->readAllStandardOutput().toLower());
                if(ErrorMSG.contains("failed")||StanderMSG.contains("failed"))
                {
                    waifu2x_qprocess_failed = true;
                    Waifu2x->close();
                    break;
                }
                if(ui->checkBox_ShowInterPro->isChecked())
                {
                    emit Send_CurrentFileProgress_progressbar_SetFinishedValue((NumOfSplitFrames*CountFinishedRounds)+file_getFileNames_in_Folder_nofilter(ScaledFramesFolderPath).size());
                }
            }
            //===============
            if(waifu2x_qprocess_failed==false)
            {
                ErrorMSG.append(Waifu2x->readAllStandardError().toLower());
                StanderMSG.append(Waifu2x->readAllStandardOutput().toLower());
                if(ErrorMSG.contains("failed")||StanderMSG.contains("failed"))
                {
                    waifu2x_qprocess_failed = true;
                }
            }
            //========= 检测是否成功,是否需要重试 ============
            if((NumOfSplitFrames == file_getFileNames_in_Folder_nofilter(ScaledFramesFolderPath).size())&&!waifu2x_qprocess_failed)
            {
                break;
            }
            else
            {
                //失败
                file_DelDir(ScaledFramesFolderPath);
                file_mkDir(ScaledFramesFolderPath);
                if(retry==ui->spinBox_retry->value()+(ForceRetryCount-1))break;
                emit Send_TextBrowser_NewMessage(tr("Automatic retry, please wait."));
                Delay_sec_sleep(5);
            }
        }
        //===========
        //根据轮数修改参数
        if(ScaleRatio_Current_tmp<ScaleRatio_Max)
        {
            for(int x = 0; x < NumOfSplitFrames; x++)
            {
                QFile::remove(SplitFramesFolderPath+"/"+Frame_fileName_list.at(x));
            }
            file_MoveFiles_Folder_NcnnVulkanFolderProcess(ScaledFramesFolderPath, SplitFramesFolderPath, true);
            file_mkDir(ScaledFramesFolderPath);
        }
        CountFinishedRounds++;
    }
    emit Send_CurrentFileProgress_Stop();//停止当前文件进度条
    //======================= 检查是否成功放大所有帧 ===========================
    QStringList Frame_fileName_list_scaled = file_getFileNames_in_Folder_nofilter(ScaledFramesFolderPath);
    if(NumOfSplitFrames!=Frame_fileName_list_scaled.size())
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
        ResGIFPath = file_path + "/" + file_name + "_waifu2x_"+QString::number(CustRes_width, 10)+"x"+QString::number(CustRes_height,10)+".gif";
    }
    Gif_assembleGif(ResGIFPath,ScaledFramesFolderPath,GIF_Duration,CustRes_isEnabled,CustRes_height,CustRes_width,isOverScaled,SourceFile_fullPath);
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



/*

  =============================================================================================
  =============================================================================================
  ============================= Waifu2x_NCNN_Vulkan_Video =======================================
  =============================================================================================
  =============================================================================================


*/

int MainWindow::Realsr_NCNN_Vulkan_Video(int rowNum)
{
    //============================= 读取设置 ================================
    int ScaleRatio = ui->doubleSpinBox_ScaleRatio_video->value();
    bool DelOriginal = (ui->checkBox_DelOriginal->isChecked()||ui->checkBox_ReplaceOriginalFile->isChecked());
    bool isCacheExists = false;
    bool isVideoConfigChanged = true;
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
    //============ 读取自定义分辨率设定 ==============
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
        bool CustRes_isEnabled_old = configIniRead->value("/VideoConfiguration/CustRes_isEnabled").toBool();
        int CustRes_height_old = configIniRead->value("/VideoConfiguration/CustRes_height").toInt();
        int CustRes_width_old = configIniRead->value("/VideoConfiguration/CustRes_width").toInt();
        QString EngineName_old = configIniRead->value("/VideoConfiguration/EngineName").toString();
        bool isProcessBySegment_old = configIniRead->value("/VideoConfiguration/isProcessBySegment").toBool();
        //=================== 比对信息 ================================
        if(EngineName_old=="realsr-ncnn-vulkan")
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
        emit Send_video_write_VideoConfiguration(VideoConfiguration_fullPath,ScaleRatio,0,CustRes_isEnabled,CustRes_height,CustRes_width,"realsr-ncnn-vulkan",false,"","",ui->groupBox_FrameInterpolation->isChecked(),0);
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
            emit Send_video_write_VideoConfiguration(VideoConfiguration_fullPath,ScaleRatio,0,CustRes_isEnabled,CustRes_height,CustRes_width,"realsr-ncnn-vulkan",false,"","",ui->groupBox_FrameInterpolation->isChecked(),0);
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
        DelVfiDir(video_mp4_fullpath);
        emit Send_video_write_VideoConfiguration(VideoConfiguration_fullPath,ScaleRatio,0,CustRes_isEnabled,CustRes_height,CustRes_width,"realsr-ncnn-vulkan",false,"","",ui->groupBox_FrameInterpolation->isChecked(),0);
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
    //读取放大倍数
    int ScaleRatio_Original = 2;
    if(CustRes_isEnabled == true)
    {
        ScaleRatio_Original = CustRes_CalNewScaleRatio(video_mp4_fullpath,CustRes_height,CustRes_width);
    }
    else
    {
        ScaleRatio_Original = ui->doubleSpinBox_ScaleRatio_video->value();
    }
    int ScaleRatio_Max=Calculate_Temporary_ScaleRatio_RealsrNCNNVulkan(ScaleRatio_Original);
    bool isOverScaled = (ScaleRatio_Max!=ScaleRatio_Original);
    //读取初始放大倍率
    int Initial_ScaleRatio = 4;
    if(!Frame_fileName_list.isEmpty())
    {
        QString Full_Path_File = "";
        for(int i = 0; i < Frame_fileName_list.size(); i++)
        {
            QString tmp = Frame_fileName_list.at(i);
            Full_Path_File = SplitFramesFolderPath + "/" + tmp;
            QFileInfo finfo(Full_Path_File);
            if(finfo.suffix()=="png")break;
        }
        QImage qimage_original;
        qimage_original.load(Full_Path_File);
        int Frame_height = qimage_original.height();
        QMap<QString,int> res_map = video_get_Resolution(SourceFile_fullPath);
        int Video_height = res_map["height"];
        if(Frame_height>0 && Video_height>0 && Frame_height>=Video_height)//判断是否读取失败
        {
            Initial_ScaleRatio = 4*(Frame_height/Video_height);
        }
    }
    //初始化进度讯息
    int NumOfSplitFrames = Frame_fileName_list.size()+file_getFileNames_in_Folder_nofilter(ScaledFramesFolderPath).size();
    if(ui->checkBox_ShowInterPro->isChecked())
    {
        int NumOfRounds=0;
        for(int ScaleRatio_Current_tmp=Initial_ScaleRatio; ScaleRatio_Current_tmp<=ScaleRatio_Max; ScaleRatio_Current_tmp*=4)
        {
            NumOfRounds++;
        }
        emit Send_CurrentFileProgress_Start(file_name+"."+file_ext,NumOfSplitFrames*NumOfRounds);
    }
    //读取配置讯息
    QString Realsr_NCNN_Vulkan_Settings_str = Realsr_NCNN_Vulkan_ReadSettings_Video_GIF(ui->spinBox_ThreadNum_video_internal->value());
    QProcess *Waifu2x = new QProcess();
    QString program = Current_Path+"/realsr-ncnn-vulkan/realsr-ncnn-vulkan_waifu2xEX.exe";
    bool waifu2x_qprocess_failed = false;
    int CountFinishedRounds=0;
    //====
    for(int ScaleRatio_Current_tmp=Initial_ScaleRatio; ScaleRatio_Current_tmp<=ScaleRatio_Max && Frame_fileName_list.isEmpty()==false; ScaleRatio_Current_tmp*=4)
    {
        for(int retry=0; retry<(ui->spinBox_retry->value()+ForceRetryCount); retry++)
        {
            QString ErrorMSG="";
            QString StanderMSG="";
            //==========
            waifu2x_qprocess_failed = false;
            QString cmd = "\"" + program + "\"" + " -i " + "\"" + SplitFramesFolderPath + "\"" + " -o " + "\"" + ScaledFramesFolderPath + "\"" + " -s 4 " + Realsr_NCNN_Vulkan_Settings_str;
            Waifu2x->start(cmd);
            while(!Waifu2x->waitForStarted(100)&&!QProcess_stop) {}
            while(!Waifu2x->waitForFinished(650)&&!QProcess_stop)
            {
                if(waifu2x_STOP)
                {
                    QStringList FinfishedFileList_FileNames = WaitForEngineIO_NcnnVulkan(ScaledFramesFolderPath);
                    Waifu2x->close();
                    //============
                    QString file_name_tmp;
                    QStringList FileNames_ScaledFramesFolderPath = file_getFileNames_in_Folder_nofilter(ScaledFramesFolderPath);
                    for(int i=0; i<FileNames_ScaledFramesFolderPath.size(); i++)
                    {
                        file_name_tmp = FileNames_ScaledFramesFolderPath.at(i);
                        if(FinfishedFileList_FileNames.contains(file_name_tmp))
                        {
                            QFile::remove(SplitFramesFolderPath + "/" + file_name_tmp);
                        }
                        else
                        {
                            QFile::remove(ScaledFramesFolderPath + "/" + file_name_tmp);
                        }
                    }
                    //=============
                    emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, "Interrupted");
                    mutex_ThreadNumRunning.lock();
                    ThreadNumRunning--;
                    mutex_ThreadNumRunning.unlock();
                    return 0;
                }
                ErrorMSG.append(Waifu2x->readAllStandardError().toLower());
                StanderMSG.append(Waifu2x->readAllStandardOutput().toLower());
                if(ErrorMSG.contains("failed")||StanderMSG.contains("failed"))
                {
                    waifu2x_qprocess_failed = true;
                    Waifu2x->close();
                    break;
                }
                if(ui->checkBox_ShowInterPro->isChecked())
                {
                    emit Send_CurrentFileProgress_progressbar_SetFinishedValue((NumOfSplitFrames*CountFinishedRounds)+file_getFileNames_in_Folder_nofilter(ScaledFramesFolderPath).size());
                }
            }
            //===============
            if(waifu2x_qprocess_failed==false)
            {
                ErrorMSG.append(Waifu2x->readAllStandardError().toLower());
                StanderMSG.append(Waifu2x->readAllStandardOutput().toLower());
                if(ErrorMSG.contains("failed")||StanderMSG.contains("failed"))
                {
                    waifu2x_qprocess_failed = true;
                }
            }
            //========= 检测是否成功,是否需要重试 ============
            if((NumOfSplitFrames == file_getFileNames_in_Folder_nofilter(ScaledFramesFolderPath).size())&&!waifu2x_qprocess_failed)
            {
                break;
            }
            else
            {
                //失败
                file_DelDir(ScaledFramesFolderPath);
                file_mkDir(ScaledFramesFolderPath);
                if(retry==ui->spinBox_retry->value()+(ForceRetryCount-1))break;
                emit Send_TextBrowser_NewMessage(tr("Automatic retry, please wait."));
                Delay_sec_sleep(5);
            }
        }
        //===========
        //根据轮数修改参数
        if(ScaleRatio_Current_tmp<ScaleRatio_Max)
        {
            QStringList SplitFolderList = file_getFileNames_in_Folder_nofilter(SplitFramesFolderPath);
            for(int x = 0; x < SplitFolderList.size(); x++)
            {
                QFile::remove(SplitFramesFolderPath+"/"+SplitFolderList.at(x));
            }
            file_MoveFiles_Folder_NcnnVulkanFolderProcess(ScaledFramesFolderPath, SplitFramesFolderPath, true);
            file_mkDir(ScaledFramesFolderPath);
        }
        CountFinishedRounds++;
    }
    emit Send_CurrentFileProgress_Stop();//停止当前文件进度条
    //======================= 检查是否成功放大所有帧 ===========================
    QStringList Frame_fileName_list_scaled = file_getFileNames_in_Folder_nofilter(ScaledFramesFolderPath);
    if(Frame_fileName_list_scaled.size()<NumOfSplitFrames)
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Unable to scale all frames.]"));
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed");
        emit Send_progressbar_Add();
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
    video_images2video(video_mp4_fullpath,video_mp4_scaled_fullpath,ScaledFramesFolderPath,AudioPath,CustRes_isEnabled,CustRes_height,CustRes_width,isOverScaled);
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
调用realser-ncnn-vulkan处理视频
读取设置,拆分,创建放大子线程,组装视频
*/
int MainWindow::Realsr_NCNN_Vulkan_Video_BySegment(int rowNum)
{
    //============================= 读取设置 ================================
    int ScaleRatio = ui->doubleSpinBox_ScaleRatio_video->value();
    bool DelOriginal = (ui->checkBox_DelOriginal->isChecked()||ui->checkBox_ReplaceOriginalFile->isChecked());
    bool isCacheExists = false;
    bool isVideoConfigChanged = true;
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
    //============ 读取自定义分辨率设定 ==============
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
        bool CustRes_isEnabled_old = configIniRead->value("/VideoConfiguration/CustRes_isEnabled").toBool();
        int CustRes_height_old = configIniRead->value("/VideoConfiguration/CustRes_height").toInt();
        int CustRes_width_old = configIniRead->value("/VideoConfiguration/CustRes_width").toInt();
        QString EngineName_old = configIniRead->value("/VideoConfiguration/EngineName").toString();
        bool isProcessBySegment_old = configIniRead->value("/VideoConfiguration/isProcessBySegment").toBool();
        bool isVideoFrameInterpolationEnabled_old = configIniRead->value("/VideoConfiguration/isVideoFrameInterpolationEnabled").toBool();
        int MultipleOfFPS_old = configIniRead->value("/VideoConfiguration/MultipleOfFPS").toInt();
        //=================== 比对信息 ================================
        if(EngineName_old=="realsr-ncnn-vulkan")
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
        emit Send_video_write_VideoConfiguration(VideoConfiguration_fullPath,ScaleRatio,0,CustRes_isEnabled,CustRes_height,CustRes_width,"realsr-ncnn-vulkan",true,VideoClipsFolderPath,VideoClipsFolderName,ui->groupBox_FrameInterpolation->isChecked(),ui->spinBox_MultipleOfFPS_VFI->value());
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
            emit Send_video_write_VideoConfiguration(VideoConfiguration_fullPath,ScaleRatio,0,CustRes_isEnabled,CustRes_height,CustRes_width,"realsr-ncnn-vulkan",true,VideoClipsFolderPath,VideoClipsFolderName,ui->groupBox_FrameInterpolation->isChecked(),ui->spinBox_MultipleOfFPS_VFI->value());
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
        DelVfiDir(video_mp4_fullpath);
        emit Send_video_write_VideoConfiguration(VideoConfiguration_fullPath,ScaleRatio,0,CustRes_isEnabled,CustRes_height,CustRes_width,"realsr-ncnn-vulkan",true,VideoClipsFolderPath,VideoClipsFolderName,ui->groupBox_FrameInterpolation->isChecked(),ui->spinBox_MultipleOfFPS_VFI->value());
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
        bool isOverScaled=false;
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
            //读取放大倍数
            int ScaleRatio_Original = 2;
            if(CustRes_isEnabled == true)
            {
                ScaleRatio_Original = CustRes_CalNewScaleRatio(video_mp4_fullpath,CustRes_height,CustRes_width);
            }
            else
            {
                ScaleRatio_Original = ui->doubleSpinBox_ScaleRatio_video->value();
            }
            int ScaleRatio_Max=Calculate_Temporary_ScaleRatio_RealsrNCNNVulkan(ScaleRatio_Original);
            isOverScaled = (ScaleRatio_Max!=ScaleRatio_Original);
            //读取初始放大倍率
            int Initial_ScaleRatio = 4;
            if(!Frame_fileName_list.isEmpty())
            {
                QString Full_Path_File = "";
                for(int i = 0; i < Frame_fileName_list.size(); i++)
                {
                    QString tmp = Frame_fileName_list.at(i);
                    Full_Path_File = SplitFramesFolderPath + "/" + tmp;
                    QFileInfo finfo(Full_Path_File);
                    if(finfo.suffix()=="png")break;
                }
                QImage qimage_original;
                qimage_original.load(Full_Path_File);
                int Frame_height = qimage_original.height();
                QMap<QString,int> res_map = video_get_Resolution(SourceFile_fullPath);
                int Video_height = res_map["height"];
                if(Frame_height>0 && Video_height>0 && Frame_height>=Video_height)//判断是否读取失败
                {
                    Initial_ScaleRatio = 4*(Frame_height/Video_height);
                }
            }
            //初始化进度讯息
            int NumOfSplitFrames = Frame_fileName_list.size()+file_getFileNames_in_Folder_nofilter(ScaledFramesFolderPath).size();
            int CurrentFileProgress_MAX = 0;
            int CurrentFileProgress_OLD = 0;
            int CurrentFileProgress_New = 0;
            if(ui->checkBox_ShowInterPro->isChecked())
            {
                int NumOfRounds=0;
                for(int ScaleRatio_Current_tmp=Initial_ScaleRatio; ScaleRatio_Current_tmp<=ScaleRatio_Max; ScaleRatio_Current_tmp*=4)
                {
                    NumOfRounds++;
                }
                CurrentFileProgress_MAX =NumOfSplitFrames*NumOfRounds;
            }
            //读取配置讯息
            QString Realsr_NCNN_Vulkan_Settings_str = Realsr_NCNN_Vulkan_ReadSettings_Video_GIF(ui->spinBox_ThreadNum_video_internal->value());
            QProcess *Waifu2x = new QProcess();
            QString program = Current_Path+"/realsr-ncnn-vulkan/realsr-ncnn-vulkan_waifu2xEX.exe";
            bool waifu2x_qprocess_failed = false;
            int CountFinishedRounds=0;
            //====
            for(int ScaleRatio_Current_tmp=Initial_ScaleRatio; ScaleRatio_Current_tmp<=ScaleRatio_Max && Frame_fileName_list.isEmpty()==false; ScaleRatio_Current_tmp*=4)
            {
                for(int retry=0; retry<(ui->spinBox_retry->value()+ForceRetryCount); retry++)
                {
                    QString ErrorMSG="";
                    QString StanderMSG="";
                    //==========
                    waifu2x_qprocess_failed = false;
                    QString cmd = "\"" + program + "\"" + " -i " + "\"" + SplitFramesFolderPath + "\"" + " -o " + "\"" + ScaledFramesFolderPath + "\"" + " -s 4 " + Realsr_NCNN_Vulkan_Settings_str;
                    Waifu2x->start(cmd);
                    while(!Waifu2x->waitForStarted(100)&&!QProcess_stop) {}
                    while(!Waifu2x->waitForFinished(650)&&!QProcess_stop)
                    {
                        if(waifu2x_STOP)
                        {
                            QStringList FinfishedFileList_FileNames = WaitForEngineIO_NcnnVulkan(ScaledFramesFolderPath);
                            Waifu2x->close();
                            //============
                            QString file_name_tmp;
                            QStringList FileNames_ScaledFramesFolderPath = file_getFileNames_in_Folder_nofilter(ScaledFramesFolderPath);
                            for(int i=0; i<FileNames_ScaledFramesFolderPath.size(); i++)
                            {
                                file_name_tmp = FileNames_ScaledFramesFolderPath.at(i);
                                if(FinfishedFileList_FileNames.contains(file_name_tmp))
                                {
                                    QFile::remove(SplitFramesFolderPath + "/" + file_name_tmp);
                                }
                                else
                                {
                                    QFile::remove(ScaledFramesFolderPath + "/" + file_name_tmp);
                                }
                            }
                            //=============
                            emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, "Interrupted");
                            mutex_ThreadNumRunning.lock();
                            ThreadNumRunning--;
                            mutex_ThreadNumRunning.unlock();
                            return 0;
                        }
                        ErrorMSG.append(Waifu2x->readAllStandardError().toLower());
                        StanderMSG.append(Waifu2x->readAllStandardOutput().toLower());
                        if(ErrorMSG.contains("failed")||StanderMSG.contains("failed"))
                        {
                            waifu2x_qprocess_failed = true;
                            Waifu2x->close();
                            break;
                        }
                        if(ui->checkBox_ShowInterPro->isChecked())
                        {
                            CurrentFileProgress_New = (NumOfSplitFrames*CountFinishedRounds)+file_getFileNames_in_Folder_nofilter(ScaledFramesFolderPath).size();
                            if(CurrentFileProgress_New!=CurrentFileProgress_OLD)
                            {
                                emit Send_TextBrowser_NewMessage(tr("File name:[")+SourceFile_fullPath+tr("]  Scale progress:[")+QString::number(CurrentFileProgress_New,10)+"/"+QString::number(CurrentFileProgress_MAX,10)+tr("] Duration progress:[")+QString::number(StartTime,10)+"s/"+QString::number(VideoDuration,10)+"s]");
                                CurrentFileProgress_OLD=CurrentFileProgress_New;
                            }
                        }
                    }
                    //===============
                    if(waifu2x_qprocess_failed==false)
                    {
                        ErrorMSG.append(Waifu2x->readAllStandardError().toLower());
                        StanderMSG.append(Waifu2x->readAllStandardOutput().toLower());
                        if(ErrorMSG.contains("failed")||StanderMSG.contains("failed"))
                        {
                            waifu2x_qprocess_failed = true;
                        }
                    }
                    //========= 检测是否成功,是否需要重试 ============
                    if((NumOfSplitFrames == file_getFileNames_in_Folder_nofilter(ScaledFramesFolderPath).size())&&!waifu2x_qprocess_failed)
                    {
                        break;
                    }
                    else
                    {
                        //失败
                        file_DelDir(ScaledFramesFolderPath);
                        file_mkDir(ScaledFramesFolderPath);
                        if(retry==ui->spinBox_retry->value()+(ForceRetryCount-1))break;
                        emit Send_TextBrowser_NewMessage(tr("Automatic retry, please wait."));
                        Delay_sec_sleep(5);
                    }
                }
                //===========
                //根据轮数修改参数
                if(ScaleRatio_Current_tmp<ScaleRatio_Max)
                {
                    QStringList SplitFolderList = file_getFileNames_in_Folder_nofilter(SplitFramesFolderPath);
                    for(int x = 0; x < SplitFolderList.size(); x++)
                    {
                        QFile::remove(SplitFramesFolderPath+"/"+SplitFolderList.at(x));
                    }
                    file_MoveFiles_Folder_NcnnVulkanFolderProcess(ScaledFramesFolderPath, SplitFramesFolderPath, true);
                    file_mkDir(ScaledFramesFolderPath);
                }
                CountFinishedRounds++;
            }
            //================ 扫描放大后的帧文件数量,判断是否放大成功 =======================
            QStringList Frame_fileName_list_scaled = file_getFileNames_in_Folder_nofilter(ScaledFramesFolderPath);
            if(Frame_fileName_list_scaled.size()<NumOfSplitFrames)
            {
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
        video_images2video(video_mp4_fullpath,video_mp4_scaled_clip_fullpath,ScaledFramesFolderPath,"",CustRes_isEnabled,CustRes_height,CustRes_width,isOverScaled);
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
              处理完当前片段
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
        video_mp4_scaled_fullpath = file_path+"/"+file_name+"_waifu2x_"+QString::number(CustRes_width,10)+"x"+QString::number(CustRes_height,10)+"_"+file_ext+".mp4";
    }
    else
    {
        video_mp4_scaled_fullpath = file_path+"/"+file_name+"_waifu2x_"+QString::number(ScaleRatio,10)+"x_"+file_ext+".mp4";
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
Realsr_NCNN_Vulkan
预读取引擎配置
*/
QString MainWindow::Realsr_NCNN_Vulkan_PreLoad_Settings()
{
    QString Realsr_NCNN_Vulkan_Settings_str = " ";
    if(ui->checkBox_MultiGPU_RealsrNcnnVulkan->isChecked()==false)
    {
        //==========单显卡==========
        //GPU ID
        if(ui->comboBox_GPUID_RealsrNCNNVulkan->currentText()!="auto")
        {
            Realsr_NCNN_Vulkan_Settings_str.append("-g "+ui->comboBox_GPUID_RealsrNCNNVulkan->currentText()+" ");
        }
        //Tile Size
        Realsr_NCNN_Vulkan_Settings_str.append("-t "+QString::number(ui->spinBox_TileSize_RealsrNCNNVulkan->value(),10)+" ");
    }
    //TTA
    if(ui->checkBox_TTA_RealsrNCNNVulkan->isChecked())
    {
        Realsr_NCNN_Vulkan_Settings_str.append("-x ");
    }
    //Model
    QString Waifu2x_folder_path = Current_Path+"/realsr-ncnn-vulkan";
    switch(ui->comboBox_Model_RealsrNCNNVulkan->currentIndex())
    {
        case 0:
            {
                Realsr_NCNN_Vulkan_Settings_str.append("-m \""+Waifu2x_folder_path+"/models-DF2K_JPEG\" ");
                break;
            }
        case 1:
            {
                Realsr_NCNN_Vulkan_Settings_str.append("-m \""+Waifu2x_folder_path+"/models-DF2K\" ");
                break;
            }
    }
    Realsr_NCNN_Vulkan_Settings_str.append("-j 1:1:1 ");
    //=======================================
    return Realsr_NCNN_Vulkan_Settings_str;
}
/*
Realsr_NCNN_Vulkan
读取配置生成配置QString
*/
QString MainWindow::Realsr_NCNN_Vulkan_ReadSettings()
{
    QString Realsr_NCNN_Vulkan_Settings_str = "";
    Realsr_NCNN_Vulkan_Settings_str.append(Realsr_NCNN_Vulkan_PreLoad_Settings_Str);
    if(ui->checkBox_MultiGPU_RealsrNcnnVulkan->isChecked())
    {
        //==========多显卡==========
        QMap<QString,QString> GPUInfo = RealsrNcnnVulkan_MultiGPU();
        //GPU ID
        Realsr_NCNN_Vulkan_Settings_str.append("-g "+GPUInfo["ID"]+" ");
        //Tile Size
        Realsr_NCNN_Vulkan_Settings_str.append("-t "+GPUInfo["TileSize"]+" ");
    }
    //=======================================
    return Realsr_NCNN_Vulkan_Settings_str;
}

/*
================================================================================
                    Realsr NCNN Vulkan 检测可用GPU
=================================================================================
*/
/*
点击检测gpu按键时:
*/
void MainWindow::on_pushButton_DetectGPU_RealsrNCNNVulkan_clicked()
{
    //====
    ui->pushButton_DetectGPU_RealsrNCNNVulkan->setText(tr("Detecting, please wait..."));
    //====
    pushButton_Start_setEnabled_self(0);
    ui->pushButton_DetectGPU->setEnabled(0);
    ui->pushButton_DetectGPUID_srmd->setEnabled(0);
    ui->pushButton_DumpProcessorList_converter->setEnabled(0);
    ui->pushButton_ListGPUs_Anime4k->setEnabled(0);
    ui->pushButton_compatibilityTest->setEnabled(0);
    ui->pushButton_DetectGPU_RealsrNCNNVulkan->setEnabled(0);
    Available_GPUID_Realsr_ncnn_vulkan.clear();
    QtConcurrent::run(this, &MainWindow::Realsr_ncnn_vulkan_DetectGPU);
}
/*
检测可用GPU ID中:
*/
int MainWindow::Realsr_ncnn_vulkan_DetectGPU()
{
    emit Send_TextBrowser_NewMessage(tr("Detecting available GPU, please wait."));
    //===============
    QString InputPath = Current_Path + "/Compatibility_Test/Compatibility_Test.jpg";
    QString OutputPath = Current_Path + "/Compatibility_Test/res.png";
    QFile::remove(OutputPath);
    //==============
    QString Waifu2x_folder_path = Current_Path + "/realsr-ncnn-vulkan";
    QString program = Waifu2x_folder_path + "/realsr-ncnn-vulkan_waifu2xEX.exe";
    QString model_path = Waifu2x_folder_path+"/models-DF2K_JPEG";
    //===========
    int GPU_ID=-1;
    //=========
    while(true)
    {
        QFile::remove(OutputPath);
        QProcess *Waifu2x = new QProcess();
        QString gpu_str = " -g "+QString::number(GPU_ID,10)+" ";
        QString cmd = "\"" + program + "\"" + " -i " + "\"" + InputPath + "\"" + " -o " + "\"" + OutputPath + "\"" + " -s 4 -t 32 -m " + "\"" + model_path + "\" -g "+QString::number(GPU_ID,10);
        Waifu2x->start(cmd);
        while(!Waifu2x->waitForStarted(100)&&!QProcess_stop) {}
        while(!Waifu2x->waitForFinished(100)&&!QProcess_stop) {}
        if(QFile::exists(OutputPath) && (Waifu2x->readAllStandardError().toLower().contains("failed")||Waifu2x->readAllStandardOutput().toLower().contains("failed"))==false)
        {
            Available_GPUID_Realsr_ncnn_vulkan.append(QString::number(GPU_ID,10));
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
    if(Available_GPUID_Realsr_ncnn_vulkan.isEmpty())
    {
        Send_TextBrowser_NewMessage(tr("No available GPU ID detected!"));
    }
    emit Send_Realsr_ncnn_vulkan_DetectGPU_finished();
    return 0;
}
/*
检测完成,向可用gpu列表和多显卡列表内填充数据:
*/
int MainWindow::Realsr_ncnn_vulkan_DetectGPU_finished()
{
    pushButton_Start_setEnabled_self(1);
    ui->pushButton_DetectGPU->setEnabled(1);
    ui->pushButton_compatibilityTest->setEnabled(1);
    ui->pushButton_DetectGPUID_srmd->setEnabled(1);
    ui->pushButton_DumpProcessorList_converter->setEnabled(1);
    on_checkBox_SpecifyGPU_Anime4k_stateChanged(1);
    ui->pushButton_DetectGPU_RealsrNCNNVulkan->setEnabled(1);
    //====
    GPUIDs_List_MultiGPU_RealsrNcnnVulkan.clear();
    ui->comboBox_GPUIDs_MultiGPU_RealsrNcnnVulkan->clear();
    //===
    ui->comboBox_GPUID_RealsrNCNNVulkan->clear();
    ui->comboBox_GPUID_RealsrNCNNVulkan->addItem("auto");
    if(!Available_GPUID_Realsr_ncnn_vulkan.isEmpty())
    {
        for(int i=0; i<Available_GPUID_Realsr_ncnn_vulkan.size(); i++)
        {
            ui->comboBox_GPUID_RealsrNCNNVulkan->addItem(Available_GPUID_Realsr_ncnn_vulkan.at(i));
            AddGPU_MultiGPU_RealsrNcnnVulkan(Available_GPUID_Realsr_ncnn_vulkan.at(i));
        }
    }
    //====
    ui->pushButton_DetectGPU_RealsrNCNNVulkan->setText(tr("Detect available GPU ID"));
    //====
    return 0;
}
/*
RealsrNcnnVulkan_MultiGPU
向Realsr_NCNN_Vulkan_ReadSettings()提交显卡信息.
通过 轮流提交显卡信息+多线程 一起实现多显卡
*/
QMap<QString,QString> MainWindow::RealsrNcnnVulkan_MultiGPU()
{
    MultiGPU_QMutex_RealsrNcnnVulkan.lock();
    //====
    int MAX_GPU_ID_RealsrNcnnVulkan = GPUIDs_List_MultiGPU_RealsrNcnnVulkan.size()-1;
    if(GPU_ID_RealsrNcnnVulkan_MultiGPU>MAX_GPU_ID_RealsrNcnnVulkan)
    {
        GPU_ID_RealsrNcnnVulkan_MultiGPU=0;
    }
    //======
    QMap<QString,QString> GPUInfo;
    do
    {
        GPUInfo = GPUIDs_List_MultiGPU_RealsrNcnnVulkan.at(GPU_ID_RealsrNcnnVulkan_MultiGPU);
        if(GPUInfo["isEnabled"] != "true")
        {
            GPU_ID_RealsrNcnnVulkan_MultiGPU++;
            if(GPU_ID_RealsrNcnnVulkan_MultiGPU>MAX_GPU_ID_RealsrNcnnVulkan)
            {
                GPU_ID_RealsrNcnnVulkan_MultiGPU=0;
            }
        }
        else
        {
            break;
        }
    }
    while(true);
    //======
    GPU_ID_RealsrNcnnVulkan_MultiGPU++;
    if(GPU_ID_RealsrNcnnVulkan_MultiGPU>MAX_GPU_ID_RealsrNcnnVulkan)
    {
        GPU_ID_RealsrNcnnVulkan_MultiGPU=0;
    }
    //======
    MultiGPU_QMutex_RealsrNcnnVulkan.unlock();
    return GPUInfo;
}
/*
向多显卡的显卡信息列表中添加显卡信息
*/
void MainWindow::AddGPU_MultiGPU_RealsrNcnnVulkan(QString GPUID)
{
    QMap<QString,QString> GPUInfo;
    GPUInfo["ID"] = GPUID;
    GPUInfo["isEnabled"] = "true";
    GPUInfo["TileSize"] = "100";
    GPUIDs_List_MultiGPU_RealsrNcnnVulkan.append(GPUInfo);
    ui->comboBox_GPUIDs_MultiGPU_RealsrNcnnVulkan->addItem(GPUID);
    ui->comboBox_GPUIDs_MultiGPU_RealsrNcnnVulkan->setCurrentIndex(0);
}

void MainWindow::on_checkBox_MultiGPU_RealsrNcnnVulkan_stateChanged(int arg1)
{
    if(ui->checkBox_MultiGPU_RealsrNcnnVulkan->isChecked())
    {
        ui->comboBox_GPUID_RealsrNCNNVulkan->setEnabled(0);
        ui->frame_TileSize_RealsrNcnnVulkan->setEnabled(0);
        ui->groupBox_GPUSettings_MultiGPU_RealsrNcnnVulkan->setEnabled(1);
    }
    else
    {
        ui->comboBox_GPUID_RealsrNCNNVulkan->setEnabled(1);
        ui->frame_TileSize_RealsrNcnnVulkan->setEnabled(1);
        ui->groupBox_GPUSettings_MultiGPU_RealsrNcnnVulkan->setEnabled(0);
    }
}

void MainWindow::on_checkBox_MultiGPU_RealsrNcnnVulkan_clicked()
{
    if(ui->checkBox_MultiGPU_RealsrNcnnVulkan->isChecked())
    {
        if(GPUIDs_List_MultiGPU_RealsrNcnnVulkan.size()==0)
        {
            QMessageBox *MSG = new QMessageBox();
            MSG->setWindowTitle(tr("Notification"));
            MSG->setText(tr("Please detect available GPU ID before enable Multi-GPU."));
            MSG->setIcon(QMessageBox::Information);
            MSG->setModal(true);
            MSG->show();
            ui->checkBox_MultiGPU_RealsrNcnnVulkan->setChecked(0);
            return;
        }
        if(GPUIDs_List_MultiGPU_RealsrNcnnVulkan.size()<2)
        {
            QMessageBox *MSG = new QMessageBox();
            MSG->setWindowTitle(tr("Error"));
            MSG->setText(tr("Insufficient number of available GPUs."));
            MSG->setIcon(QMessageBox::Warning);
            MSG->setModal(true);
            MSG->show();
            ui->checkBox_MultiGPU_RealsrNcnnVulkan->setChecked(0);
            return;
        }
    }
}

void MainWindow::on_comboBox_GPUIDs_MultiGPU_RealsrNcnnVulkan_currentIndexChanged(int index)
{
    if(ui->comboBox_GPUIDs_MultiGPU_RealsrNcnnVulkan->count()==0)
    {
        return;
    }
    QMap<QString,QString> GPUInfo=GPUIDs_List_MultiGPU_RealsrNcnnVulkan.at(ui->comboBox_GPUIDs_MultiGPU_RealsrNcnnVulkan->currentIndex());
    ui->checkBox_isEnable_CurrentGPU_MultiGPU_RealsrNcnnVulkan->setChecked(GPUInfo["isEnabled"] == "true");
    ui->spinBox_TileSize_CurrentGPU_MultiGPU_RealsrNcnnVulkan->setValue(GPUInfo["TileSize"].toInt());
}

void MainWindow::on_checkBox_isEnable_CurrentGPU_MultiGPU_RealsrNcnnVulkan_clicked()
{
    QMap<QString,QString> GPUInfo=GPUIDs_List_MultiGPU_RealsrNcnnVulkan.at(ui->comboBox_GPUIDs_MultiGPU_RealsrNcnnVulkan->currentIndex());
    if(ui->checkBox_isEnable_CurrentGPU_MultiGPU_RealsrNcnnVulkan->isChecked())
    {
        GPUInfo["isEnabled"] = "true";
    }
    else
    {
        GPUInfo["isEnabled"] = "false";
    }
    GPUIDs_List_MultiGPU_RealsrNcnnVulkan.replace(ui->comboBox_GPUIDs_MultiGPU_RealsrNcnnVulkan->currentIndex(),GPUInfo);
    int enabledGPUs = 0;
    for (int i=0; i<GPUIDs_List_MultiGPU_RealsrNcnnVulkan.size(); i++)
    {
        QMap<QString,QString> GPUInfo_tmp = GPUIDs_List_MultiGPU_RealsrNcnnVulkan.at(i);
        if(GPUInfo_tmp["isEnabled"] == "true")
        {
            enabledGPUs++;
        }
    }
    if(enabledGPUs<2)
    {
        QMap<QString,QString> GPUInfo=GPUIDs_List_MultiGPU_RealsrNcnnVulkan.at(ui->comboBox_GPUIDs_MultiGPU_RealsrNcnnVulkan->currentIndex());
        GPUInfo["isEnabled"] = "true";
        GPUIDs_List_MultiGPU_RealsrNcnnVulkan.replace(ui->comboBox_GPUIDs_MultiGPU_RealsrNcnnVulkan->currentIndex(),GPUInfo);
        ui->checkBox_isEnable_CurrentGPU_MultiGPU_RealsrNcnnVulkan->setChecked(1);
        QMessageBox *MSG = new QMessageBox();
        MSG->setWindowTitle(tr("Warning"));
        MSG->setText(tr("At least 2 GPUs need to be enabled !!"));
        MSG->setIcon(QMessageBox::Warning);
        MSG->setModal(true);
        MSG->show();
    }
}

void MainWindow::on_spinBox_TileSize_CurrentGPU_MultiGPU_RealsrNcnnVulkan_valueChanged(int arg1)
{
    QMap<QString,QString> GPUInfo=GPUIDs_List_MultiGPU_RealsrNcnnVulkan.at(ui->comboBox_GPUIDs_MultiGPU_RealsrNcnnVulkan->currentIndex());
    GPUInfo["TileSize"] = QString::number(ui->spinBox_TileSize_CurrentGPU_MultiGPU_RealsrNcnnVulkan->value(),10);
    GPUIDs_List_MultiGPU_RealsrNcnnVulkan.replace(ui->comboBox_GPUIDs_MultiGPU_RealsrNcnnVulkan->currentIndex(),GPUInfo);
}

void MainWindow::on_pushButton_ShowMultiGPUSettings_RealsrNcnnVulkan_clicked()
{
    int Max_GPU_ID = GPUIDs_List_MultiGPU_RealsrNcnnVulkan.size();
    QString MultiGPUSettings_str="";
    for(int GPU_ID=0; GPU_ID<Max_GPU_ID; GPU_ID++)
    {
        QMap<QString,QString> GPUInfo = GPUIDs_List_MultiGPU_RealsrNcnnVulkan.at(GPU_ID);
        if(GPUInfo["isEnabled"] != "true")
        {
            continue;
        }
        else
        {
            MultiGPUSettings_str.append("GPU ID:["+GPUInfo["ID"]+"] | "+tr("Tile size:")+"["+GPUInfo["TileSize"]+"]\n\n");
        }
    }
    //=========
    QMessageBox *MSG = new QMessageBox();
    MSG->setWindowTitle(tr("Current Multi-GPU settings"));
    MSG->setText(MultiGPUSettings_str);
    MSG->setIcon(QMessageBox::Information);
    MSG->setModal(true);
    MSG->show();
}
/*
RealSR 块大小调整按钮
*/
void MainWindow::on_pushButton_Add_TileSize_RealsrNCNNVulkan_clicked()
{
    ui->spinBox_TileSize_RealsrNCNNVulkan->setValue(AddTileSize_NCNNVulkan_Converter(ui->spinBox_TileSize_RealsrNCNNVulkan->value()));
}

void MainWindow::on_pushButton_Minus_TileSize_RealsrNCNNVulkan_clicked()
{
    ui->spinBox_TileSize_RealsrNCNNVulkan->setValue(MinusTileSize_NCNNVulkan_Converter(ui->spinBox_TileSize_RealsrNCNNVulkan->value()));
}

/*
计算 临时放大倍率
*/
int MainWindow::Calculate_Temporary_ScaleRatio_RealsrNCNNVulkan(int ScaleRatio)
{
    int ScaleRatio_tmp=0;
    //如果设定的scaleRatio不是偶数,则+1,并输出到tmp
    if((ScaleRatio%2)==0)
    {
        ScaleRatio_tmp = ScaleRatio;
    }
    else
    {
        ScaleRatio_tmp = ScaleRatio+1;
    }
    //判断是否为4的幂数
    if((((ScaleRatio_tmp&(ScaleRatio_tmp-1))==0)&&(ScaleRatio_tmp&0x555555555))!=true)
    {
        for(int i=1; true; i++)
        {
            int pow_ =pow(4,i);
            if(pow_>=ScaleRatio_tmp)
            {
                ScaleRatio_tmp=pow_;
                break;
            }
        }
    }
    return ScaleRatio_tmp;
}

/*
Realsr_NCNN_Vulkan
读取引擎配置
*/
QString MainWindow::Realsr_NCNN_Vulkan_ReadSettings_Video_GIF(int ThreadNum)
{
    QString Realsr_NCNN_Vulkan_Settings_str = " ";
    if(ui->checkBox_MultiGPU_RealsrNcnnVulkan->isChecked()==false)
    {
        //==========单显卡==========
        //GPU ID
        if(ui->comboBox_GPUID_RealsrNCNNVulkan->currentText()!="auto")
        {
            Realsr_NCNN_Vulkan_Settings_str.append("-g "+ui->comboBox_GPUID_RealsrNCNNVulkan->currentText()+" ");
        }
        //Tile Size
        Realsr_NCNN_Vulkan_Settings_str.append("-t "+QString::number(ui->spinBox_TileSize_RealsrNCNNVulkan->value(),10)+" ");
        //线程数量
        QString jobs_num_str = QString("%1").arg(ThreadNum);
        Realsr_NCNN_Vulkan_Settings_str.append("-j "+jobs_num_str+":"+jobs_num_str+":"+jobs_num_str+" ");
    }
    else
    {
        //==============多显卡================
        int NumOfGPUs_Available = GPUIDs_List_MultiGPU_RealsrNcnnVulkan.size();//获取显卡总数
        int NumOfGPUs_Enabled = 0;
        QString GPU_ID_cmd = "-g ";
        QString TileSize_cmd = "-t ";
        for(int i=0; i<NumOfGPUs_Available; i++)
        {
            QMap<QString,QString> GPUInfo = GPUIDs_List_MultiGPU_RealsrNcnnVulkan.at(i);
            if(GPUInfo["isEnabled"] != "true")continue;
            NumOfGPUs_Enabled++;
            if(i==0)
            {
                GPU_ID_cmd.append(GPUInfo["ID"]);
                TileSize_cmd.append(GPUInfo["TileSize"]);
            }
            else
            {
                GPU_ID_cmd.append(","+GPUInfo["ID"]);
                TileSize_cmd.append(","+GPUInfo["TileSize"]);
            }
        }
        Realsr_NCNN_Vulkan_Settings_str.append(GPU_ID_cmd+" ");
        Realsr_NCNN_Vulkan_Settings_str.append(TileSize_cmd+" ");
        //线程数量
        int NumOfThreads_PerGPU = ThreadNum/NumOfGPUs_Enabled;
        if(NumOfThreads_PerGPU<1)NumOfThreads_PerGPU=1;
        int LoadAndWrite_tnum = NumOfGPUs_Enabled*NumOfThreads_PerGPU;
        QString Jobs_cmd = "";
        for(int i=0; i<NumOfGPUs_Enabled; i++)
        {
            if(i==0)
            {
                Jobs_cmd.append(QString("%1").arg(NumOfThreads_PerGPU));
            }
            else
            {
                Jobs_cmd.append(QString(",%1").arg(NumOfThreads_PerGPU));
            }
        }
        Realsr_NCNN_Vulkan_Settings_str.append(QString("-j %1:").arg(LoadAndWrite_tnum)+Jobs_cmd+QString(":%1 ").arg(LoadAndWrite_tnum));
    }
    //TTA
    if(ui->checkBox_TTA_RealsrNCNNVulkan->isChecked())
    {
        Realsr_NCNN_Vulkan_Settings_str.append("-x ");
    }
    //Model
    QString Waifu2x_folder_path = Current_Path+"/realsr-ncnn-vulkan";
    switch(ui->comboBox_Model_RealsrNCNNVulkan->currentIndex())
    {
        case 0:
            {
                Realsr_NCNN_Vulkan_Settings_str.append("-m \""+Waifu2x_folder_path+"/models-DF2K_JPEG\" ");
                break;
            }
        case 1:
            {
                Realsr_NCNN_Vulkan_Settings_str.append("-m \""+Waifu2x_folder_path+"/models-DF2K\" ");
                break;
            }
    }
    //=======================================
    return Realsr_NCNN_Vulkan_Settings_str;
}
