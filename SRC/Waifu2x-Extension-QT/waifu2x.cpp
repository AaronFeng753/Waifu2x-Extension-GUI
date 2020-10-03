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

int MainWindow::Waifu2xMainThread()
{
    //在table中的状态修改完成前一直block,防止偶发的多线程错误
    QMutex_Table_ChangeAllStatusToWaiting.lock();
    NULL;
    QMutex_Table_ChangeAllStatusToWaiting.unlock();
    //=======================
    int rowCount_image = Table_model_image->rowCount();
    int rowCount_gif = Table_model_gif->rowCount();
    int rowCount_video = Table_model_video->rowCount();
    //=======================
    Progressbar_MaxVal = rowCount_image + rowCount_gif + rowCount_video;
    Progressbar_CurrentVal = 0;
    TaskNumFinished=0;
    emit Send_PrograssBar_Range_min_max(0, Progressbar_MaxVal);
    if(rowCount_image>0)
    {
        int ImageEngine = ui->comboBox_Engine_Image->currentIndex();
        for ( int i = 0; i < rowCount_image; i++ )
        {
            if(waifu2x_STOP)
            {
                while (ThreadNumRunning > 0)
                {
                    Delay_msec_sleep(750);
                }
                waifu2x_STOP_confirm = true;
                return 0;//如果启用stop位,则直接return
            }
            //=============== 判断状态 ================
            if(!ui->checkBox_ReProcFinFiles->isChecked())
            {
                if(Table_model_image->item(i,1)->text().toLower().contains("finished"))
                {
                    emit Send_progressbar_Add();
                    continue;
                }
            }
            if(Table_model_image->item(i,1)->text().toLower().contains("deleted"))
            {
                emit Send_progressbar_Add();
                continue;
            }
            //============== 判断权限 =====================
            if(file_isFilesFolderWritable_row_image(i)==false)
            {
                emit Send_progressbar_Add();
                continue;
            }
            //=========
            ThreadNumMax = ui->spinBox_ThreadNum_image->value();//获取image线程数量最大值
            //====================================================================================
            switch(ImageEngine)
            {
                case 0:
                    {
                        mutex_ThreadNumRunning.lock();
                        ThreadNumRunning++;//线程数量统计+1
                        mutex_ThreadNumRunning.unlock();
                        QtConcurrent::run(this, &MainWindow::Waifu2x_NCNN_Vulkan_Image, i);
                        while (ThreadNumRunning >= ThreadNumMax)
                        {
                            Delay_msec_sleep(750);
                        }
                        break;
                    }
                case 1:
                    {
                        mutex_ThreadNumRunning.lock();
                        ThreadNumRunning++;//线程数量统计+1
                        mutex_ThreadNumRunning.unlock();
                        QtConcurrent::run(this, &MainWindow::Waifu2x_Converter_Image, i);
                        while (ThreadNumRunning >= ThreadNumMax)
                        {
                            Delay_msec_sleep(750);
                        }
                        break;
                    }
                case 2:
                    {
                        mutex_ThreadNumRunning.lock();
                        ThreadNumRunning++;//线程数量统计+1
                        mutex_ThreadNumRunning.unlock();
                        QtConcurrent::run(this, &MainWindow::SRMD_NCNN_Vulkan_Image, i);
                        while (ThreadNumRunning >= ThreadNumMax)
                        {
                            Delay_msec_sleep(750);
                        }
                        break;
                    }
                case 3:
                    {
                        mutex_ThreadNumRunning.lock();
                        ThreadNumRunning++;//线程数量统计+1
                        mutex_ThreadNumRunning.unlock();
                        QtConcurrent::run(this, &MainWindow::Anime4k_Image, i);
                        while (ThreadNumRunning >= ThreadNumMax)
                        {
                            Delay_msec_sleep(750);
                        }
                        break;
                    }
                case 4:
                    {
                        mutex_ThreadNumRunning.lock();
                        ThreadNumRunning++;//线程数量统计+1
                        mutex_ThreadNumRunning.unlock();
                        QtConcurrent::run(this, &MainWindow::Waifu2x_Caffe_Image, i);
                        while (ThreadNumRunning >= ThreadNumMax)
                        {
                            Delay_msec_sleep(750);
                        }
                        break;
                    }
                case 5:
                    {
                        mutex_ThreadNumRunning.lock();
                        ThreadNumRunning++;//线程数量统计+1
                        mutex_ThreadNumRunning.unlock();
                        QtConcurrent::run(this, &MainWindow::Realsr_NCNN_Vulkan_Image, i);
                        while (ThreadNumRunning >= ThreadNumMax)
                        {
                            Delay_msec_sleep(750);
                        }
                        break;
                    }
            }
        }
    }
    while (ThreadNumRunning>0)
    {
        Delay_msec_sleep(750);
    }
    //=========================================================
    //                   GIF 线程调度
    //===========================================================
    if(rowCount_gif>0)
    {
        int GIFEngine = ui->comboBox_Engine_GIF->currentIndex();
        for ( int i = 0; i < rowCount_gif; i++ )
        {
            if(waifu2x_STOP)
            {
                while (ThreadNumRunning > 0)
                {
                    Delay_msec_sleep(750);
                }
                waifu2x_STOP_confirm = true;
                return 0;//如果启用stop位,则直接return
            }
            //=============== 判断状态 ================
            if(!ui->checkBox_ReProcFinFiles->isChecked())
            {
                if(Table_model_gif->item(i,1)->text().toLower().contains("finished"))
                {
                    emit Send_progressbar_Add();
                    continue;
                }
            }
            if(Table_model_gif->item(i,1)->text().toLower().contains("deleted"))
            {
                emit Send_progressbar_Add();
                continue;
            }
            //============== 判断权限 =====================
            if(file_isFilesFolderWritable_row_gif(i)==false)
            {
                emit Send_progressbar_Add();
                continue;
            }
            //=========
            switch(GIFEngine)
            {
                case 0:
                    {
                        ThreadNumRunning=1;
                        Waifu2x_NCNN_Vulkan_GIF(i);
                        ThreadNumRunning=0;
                        break;
                    }
                case 1:
                    {
                        ThreadNumRunning=1;
                        Waifu2x_Converter_GIF(i);
                        ThreadNumRunning=0;
                        break;
                    }
                case 2:
                    {
                        ThreadNumRunning=1;
                        SRMD_NCNN_Vulkan_GIF(i);
                        ThreadNumRunning=0;
                        break;
                    }
                case 3:
                    {
                        ThreadNumRunning=1;
                        Anime4k_GIF(i);
                        ThreadNumRunning=0;
                        break;
                    }
                case 4:
                    {
                        ThreadNumRunning=1;
                        Waifu2x_Caffe_GIF(i);
                        ThreadNumRunning=0;
                        break;
                    }
                case 5:
                    {
                        ThreadNumRunning=1;
                        Realsr_NCNN_Vulkan_GIF(i);
                        ThreadNumRunning=0;
                        break;
                    }
            }
        }
    }
    //=========================================================
    //                   视频 线程调度
    //===========================================================
    if(rowCount_video>0)
    {
        int VideoEngine = ui->comboBox_Engine_Video->currentIndex();
        for ( int i = 0; i<rowCount_video; i++ )
        {
            if(waifu2x_STOP)
            {
                while (ThreadNumRunning > 0)
                {
                    Delay_msec_sleep(750);
                }
                waifu2x_STOP_confirm = true;
                return 0;//如果启用stop位,则直接return
            }
            //=============== 判断状态 ================
            if(!ui->checkBox_ReProcFinFiles->isChecked())
            {
                if(Table_model_video->item(i,1)->text().toLower().contains("finished"))
                {
                    emit Send_progressbar_Add();
                    continue;
                }
            }
            if(Table_model_video->item(i,1)->text().toLower().contains("deleted"))
            {
                emit Send_progressbar_Add();
                continue;
            }
            //============== 判断权限 =====================
            if(file_isFilesFolderWritable_row_video(i)==false)
            {
                emit Send_progressbar_Add();
                continue;
            }
            //============= 获取时长, 判断是否需要分段处理 =================
            //=========
            switch(VideoEngine)
            {
                case 0:
                    {
                        ThreadNumRunning=1;
                        if(video_isNeedProcessBySegment(i))
                        {
                            Waifu2x_NCNN_Vulkan_Video_BySegment(i);
                        }
                        else
                        {
                            Waifu2x_NCNN_Vulkan_Video(i);
                        }
                        ThreadNumRunning=0;
                        break;
                    }
                case 1:
                    {
                        ThreadNumRunning=1;
                        if(video_isNeedProcessBySegment(i))
                        {
                            Waifu2x_Converter_Video_BySegment(i);
                        }
                        else
                        {
                            Waifu2x_Converter_Video(i);
                        }
                        ThreadNumRunning=0;
                        break;
                    }
                case 2:
                    {
                        ThreadNumRunning=1;
                        if(video_isNeedProcessBySegment(i))
                        {
                            Anime4k_Video_BySegment(i);
                        }
                        else
                        {
                            Anime4k_Video(i);
                        }
                        ThreadNumRunning=0;
                        break;
                    }
                case 3:
                    {
                        ThreadNumRunning=1;
                        if(video_isNeedProcessBySegment(i))
                        {
                            SRMD_NCNN_Vulkan_Video_BySegment(i);
                        }
                        else
                        {
                            SRMD_NCNN_Vulkan_Video(i);
                        }
                        ThreadNumRunning=0;
                        break;
                    }
                case 4:
                    {
                        ThreadNumRunning=1;
                        if(video_isNeedProcessBySegment(i))
                        {
                            Waifu2x_Caffe_Video_BySegment(i);
                        }
                        else
                        {
                            Waifu2x_Caffe_Video(i);
                        }
                        ThreadNumRunning=0;
                        break;
                    }
                case 5:
                    {
                        ThreadNumRunning=1;
                        if(video_isNeedProcessBySegment(i))
                        {
                            Realsr_NCNN_Vulkan_Video_BySegment(i);
                        }
                        else
                        {
                            Realsr_NCNN_Vulkan_Video(i);
                        }
                        ThreadNumRunning=0;
                        break;
                    }
            }
        }
    }
    if(waifu2x_STOP)
    {
        waifu2x_STOP_confirm = true;
        return 0;//如果启用stop位,则直接return
    }
    emit Send_Waifu2x_Finished();
    return 0;
}

void MainWindow::Waifu2x_Finished()
{
    //=================== 提示音 =================================
    QtConcurrent::run(this, &MainWindow::Play_NFSound);
    //===================== 关机 ==============================
    //if(ui->checkBox_AutoTurnOFF->isChecked())
    if(ui->comboBox_FinishAction->currentIndex()>0)
    {
        AutoFinishAction_Message();
        //关机前自动保存设置
        if(ui->checkBox_AutoSaveSettings->isChecked())
        {
            emit Send_Settings_Save();
        }
        QtConcurrent::run(this, &MainWindow::SystemShutDown_Countdown);
    }
    //==================== 进度条 =================================
    progressbar_SetToMax(Progressbar_MaxVal);
    //=============================================================
    emit Send_TextBrowser_NewMessage(tr("Process finished."));
    emit Send_SystemTray_NewMessage(tr("Process finished."));
    //============================================================
    Waifu2x_Finished_manual();
}

void MainWindow::Waifu2x_Finished_manual()
{
    TimeCostTimer->stop();
    //================== 界面恢复 ===============================
    RemoveFile_FilesList->setEnabled(1);
    this->setAcceptDrops(1);
    ui->pushButton_Stop->setVisible(0);
    ui->pushButton_Start->setVisible(1);
    if(ui->checkBox_ReplaceOriginalFile->isChecked()==false)
    {
        ui->groupBox_OutPut->setEnabled(1);
        ui->checkBox_DelOriginal->setEnabled(1);
    }
    ui->pushButton_ClearList->setEnabled(1);
    ui->pushButton_RemoveItem->setEnabled(1);
    ui->groupBox_Engine->setEnabled(1);
    ui->groupBox_ScaleRaton_DenoiseLevel->setEnabled(1);
    ui->checkBox_OptGIF->setEnabled(1);
    ui->checkBox_SaveAsJPG->setEnabled(1);
    on_checkBox_SaveAsJPG_stateChanged(0);
    on_checkBox_CompressJPG_stateChanged(0);
    ui->checkBox_ReProcFinFiles->setEnabled(1);
    ui->pushButton_compatibilityTest->setEnabled(1);
    ui->pushButton_CustRes_cancel->setEnabled(1);
    ui->pushButton_CustRes_apply->setEnabled(1);
    ui->pushButton_ReadFileList->setEnabled(1);
    ui->comboBox_AspectRatio_custRes->setEnabled(1);
    ui->groupBox_video_settings->setEnabled(1);
    if(ui->checkBox_DelOriginal->isChecked())ui->checkBox_Move2RecycleBin->setEnabled(1);
    ui->pushButton_ForceRetry->setVisible(0);
    ui->groupBox_AudioDenoise->setEnabled(1);
    ui->checkBox_ProcessVideoBySegment->setEnabled(1);
    if(ui->checkBox_ProcessVideoBySegment->isChecked())
    {
        ui->spinBox_SegmentDuration->setEnabled(1);
    }
    on_comboBox_model_vulkan_currentIndexChanged(0);
    if(isWaifu2xCaffeEnabled())
    {
        ui->comboBox_ImageStyle_Waifu2xCaffe->setEnabled(1);
    }
    else
    {
        ui->comboBox_ImageStyle_Waifu2xCaffe->setEnabled(0);
    }
    emit Send_CurrentFileProgress_Stop();
    ui->checkBox_PreProcessImage->setEnabled(1);
    checkBox_ReplaceOriginalFile_setEnabled_True_Self();
    //=================== 数值恢复 ================================
    ThreadNumMax = 0;
    ThreadNumRunning = 0;
    Progressbar_MaxVal = 0;
    Progressbar_CurrentVal = 0;
    TaskNumTotal=0;
    TaskNumFinished=0;
    NewTaskFinished=false;
    ETA=0;
    TimeCost=0;
}



/*
判断图片是否含有透明通道
*/
bool MainWindow::Imgae_hasAlphaChannel(int rowNum)
{
    /*
    ======== 如果没开启检测, 直接返回false =============
    */
    if(ui->checkBox_AutoDetectAlphaChannel->isChecked()==false)return false;
    //======
    QString SourceFile_fullPath = Table_model_image->item(rowNum,2)->text();
    if(QFile::exists(SourceFile_fullPath)==false)return false;
    //======
    QImage img(SourceFile_fullPath);
    if(img.hasAlphaChannel())
    {
        if(ui->checkBox_SaveAsJPG->isChecked())
        {
            emit Send_TextBrowser_NewMessage(tr("It is detected that the image [")+SourceFile_fullPath+tr("] contains the Alpha channel, so the result image will be forcibly saved as PNG."));
        }
        return true;
    }
    else
    {
        return false;
    }
}
/*
判断图片格式并转换
*/
QString MainWindow::Imgae_Convert2PNG(QString ImagePath)
{
    QFileInfo fileinfo(ImagePath);
    QString file_ext = fileinfo.suffix();
    //判断是否已经是PNG
    if((file_ext.trimmed().toLower()=="png")||(ui->checkBox_PreProcessImage->isChecked()==false))
    {
        return ImagePath;
    }
    //不是PNG则开始转换
    QString file_name = file_getBaseName(ImagePath);
    QString file_Folder = file_getFolderPath(fileinfo);
    QString OutPut_Path = file_Folder + "/" + file_name + "_W2xEX_"+file_ext+".png";//输出的png图片的完整路径
    //======
    QString program = Current_Path+"/convert_waifu2xEX.exe";
    QFile::remove(OutPut_Path);
    QProcess Convert2PNG;
    Convert2PNG.start("\""+program+"\" \""+ImagePath+"\" \""+OutPut_Path+"\"");
    while(!Convert2PNG.waitForStarted(100)&&!QProcess_stop) {}
    while(!Convert2PNG.waitForFinished(100)&&!QProcess_stop) {}
    //======
    if(QFile::exists(OutPut_Path)==false)
    {
        emit Send_TextBrowser_NewMessage(tr("Error: Can\'t convert [")+ImagePath+tr("] to PNG. The pre-process will be skipped and try to process the original image directly."));
        return ImagePath;
    }
    //======
    return OutPut_Path;
}
/*
移动文件到输出路径
*/
void MainWindow::MoveFileToOutputPath(QString Orginal,QString SourceFilePath)
{
    MoveFile_QMutex.lock();
    if(QFile::exists(Orginal)==false)
    {
        emit Send_TextBrowser_NewMessage(tr("Error! Original file [")+Orginal+tr("] does not exists."));
        MoveFile_QMutex.unlock();
        return;
    }
    QString Target_fullpPath="";
    QString Target_fileName="";
    QString Target_folder="";
    //确定文件名
    if(ui->checkBox_OutPath_KeepOriginalFileName->isChecked())
    {
        QFileInfo fileinfo_source(SourceFilePath);
        QString file_name = file_getBaseName(fileinfo_source.filePath());
        QFileInfo fileinfo_Orginal(Orginal);
        QString file_ext = fileinfo_Orginal.suffix();
        Target_fileName=file_name+"."+file_ext;
    }
    else
    {
        QFileInfo fileinfo_Orginal(Orginal);
        Target_fileName = fileinfo_Orginal.fileName();
    }
    //确定输出文件夹
    if(ui->checkBox_KeepParentFolder->isChecked())
    {
        QFileInfo fileinfo_SourceFilePath(SourceFilePath);
        QString folder_path_SourceFilePath = file_getFolderPath(fileinfo_SourceFilePath);
        QStringList folder_path_SourceFilePath_QStringList = folder_path_SourceFilePath.split("/");
        QString ParentFolderName = folder_path_SourceFilePath_QStringList.last();
        //判断上级文件夹是不是磁盘根目录,若是则将文件夹名改为[盘符 Drive]
        if(ParentFolderName.contains(":"))
        {
            ParentFolderName=ParentFolderName.replace(":"," Drive");
        }
        Target_folder = OutPutFolder_main+"/"+ParentFolderName;
        file_mkDir(Target_folder);
    }
    else
    {
        Target_folder=OutPutFolder_main;
    }
    //组装生成完整路径
    Target_fullpPath = Target_folder+"/"+Target_fileName;
    //判断输出路径是否有和目标文件重名的 以及 是否启用了直接覆盖
    if(QFile::exists(Target_fullpPath)&&(ui->checkBox_OutPath_Overwrite->isChecked()==false))
    {
        int suffix_int = 1;
        QString Target_fullpPath_tmp = "";
        while(true)
        {
            QFileInfo fileinfo_tmp(Target_fullpPath);
            QString file_name = file_getBaseName(fileinfo_tmp.filePath());
            QString file_ext = fileinfo_tmp.suffix();
            QString file_path = file_getFolderPath(fileinfo_tmp);
            Target_fullpPath_tmp = file_path+"/"+file_name+"_"+QString::number(suffix_int,10)+"."+file_ext;
            if(!QFile::exists(Target_fullpPath_tmp))break;
            suffix_int++;
        }
        Target_fullpPath = Target_fullpPath_tmp;
    }
    if(ui->checkBox_OutPath_Overwrite->isChecked()==true)
    {
        QFile::remove(Target_fullpPath);
    }
    if(QFile::rename(Orginal,Target_fullpPath)==false)
    {
        emit Send_TextBrowser_NewMessage(tr("Error! Failed to move [")+Orginal+tr("] to [")+Target_fullpPath+"]");
    }
    MoveFile_QMutex.unlock();
}
