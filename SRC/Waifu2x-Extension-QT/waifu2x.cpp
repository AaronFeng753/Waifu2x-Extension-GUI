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
开始处理文件的按钮
*/
void MainWindow::on_pushButton_Start_clicked()
{
    /*
     * 判断是否启用自定义输出路径
     * 判断输出路径是否合法
    */
    if(ui->checkBox_OutPath_isEnabled->isChecked())
    {
        QString tmp = ui->lineEdit_outputPath->text();
        tmp = tmp.trimmed();
        if(tmp=="")
        {
            //=====
            QMessageBox *MSG = new QMessageBox();
            MSG->setWindowTitle(tr("Error"));
            MSG->setText(tr("Output path is empty."));
            MSG->setIcon(QMessageBox::Warning);
            MSG->setModal(true);
            MSG->show();
            //=====
            emit Send_TextBrowser_NewMessage(tr("Output path is empty."));
            return;
        }
        tmp = tmp.replace("\\","/");
        tmp = tmp.replace("\\\\","/");
        tmp = tmp.replace("//","/");
        if(tmp.right(1)=="/")
        {
            tmp = tmp.left(tmp.length() - 1);
        }
        QFileInfo fileinfo_tmp(tmp);
        if(file_isDirExist(tmp)&&fileinfo_tmp.isDir()&&file_isDirWritable(tmp))
        {
            OutPutFolder_main = tmp;
        }
        else
        {
            //=====
            QMessageBox *MSG = new QMessageBox();
            MSG->setWindowTitle(tr("Error"));
            MSG->setText(tr("Invalid output path.\n\nOr do not have sufficient permissions to write files to the output path, you might need to grant administrator permissions to this software."));
            MSG->setIcon(QMessageBox::Warning);
            MSG->setModal(true);
            MSG->show();
            //=====
            emit Send_TextBrowser_NewMessage(tr("Invalid output path."));
            return;
        }
    }
    //=============================
    //      判断文件列表是否为空
    //=============================
    if(Table_model_image->rowCount()==0&&Table_model_gif->rowCount()==0&&Table_model_video->rowCount()==0)
    {
        QMessageBox *MSG = new QMessageBox();
        MSG->setWindowTitle(tr("Error"));
        MSG->setText(tr("Unable to start processing files: The file list is empty."));
        MSG->setIcon(QMessageBox::Warning);
        MSG->setModal(true);
        MSG->show();
        return;
    }
    //======================================
    //      判断文件列表是否全部处理完成
    //======================================
    int rowCount_image = Table_model_image->rowCount();
    int rowCount_image_cache = rowCount_image;
    int rowCount_gif = Table_model_gif->rowCount();
    int rowCount_gif_cache = rowCount_gif;
    int rowCount_video = Table_model_video->rowCount();
    int rowCount_video_cache = rowCount_video;
    //检查图片文件列表
    for ( int RowNum = 0; RowNum < rowCount_image; RowNum++ )
    {
        if(ui->checkBox_ReProcFinFiles->isChecked()==false)
        {
            if(Table_model_image->item(RowNum,1)->text().toLower().contains("finished"))
            {
                rowCount_image_cache--;
                continue;
            }
        }
        if(Table_model_image->item(RowNum,1)->text().toLower().contains("deleted"))
        {
            rowCount_image_cache--;
            continue;
        }
    }
    //检查GIF文件列表
    for ( int RowNum = 0; RowNum < rowCount_gif; RowNum++ )
    {
        if(ui->checkBox_ReProcFinFiles->isChecked()==false)
        {
            if(Table_model_gif->item(RowNum,1)->text().toLower().contains("finished"))
            {
                rowCount_gif_cache--;
                continue;
            }
        }
        if(Table_model_gif->item(RowNum,1)->text().toLower().contains("deleted"))
        {
            rowCount_gif_cache--;
            continue;
        }
    }
    //检查视频文件列表
    for ( int RowNum = 0; RowNum < rowCount_video; RowNum++ )
    {
        if(ui->checkBox_ReProcFinFiles->isChecked()==false)
        {
            if(Table_model_video->item(RowNum,1)->text().toLower().contains("finished"))
            {
                rowCount_video_cache--;
                continue;
            }
        }
        if(Table_model_video->item(RowNum,1)->text().toLower().contains("deleted"))
        {
            rowCount_video_cache--;
            continue;
        }
    }
    //判断是否剩余可处理的文件
    if(rowCount_image_cache+rowCount_gif_cache+rowCount_video_cache==0)
    {
        QMessageBox *MSG = new QMessageBox();
        MSG->setWindowTitle(tr("Error"));
        MSG->setText(tr("Unable to start processing files: All files in Files list are Finished or Deleted.\n\nTip: You can enable [Re-process finished files] if you wanna re-process finished files."));
        MSG->setIcon(QMessageBox::Warning);
        MSG->setModal(true);
        MSG->show();
        return;
    }
    //正式开始处理列表内的文件
    Table_ChangeAllStatusToWaiting();//将table内没开始处理或者被打断的条目状态变为等待中
    //============== 判断是否需要隐藏ETA和剩余时间 ================
    if(Table_model_gif->rowCount()!=0||Table_model_video->rowCount()!=0)
    {
        ui->label_TimeRemain->setVisible(0);
        ui->label_ETA->setVisible(0);
    }
    else
    {
        ui->label_TimeRemain->setVisible(1);
        ui->label_ETA->setVisible(1);
    }
    //============== 数值初始化 ==================
    waifu2x_STOP = false;
    waifu2x_STOP_confirm = false;
    ThreadNumMax = 0;
    ThreadNumRunning = 0;
    Progressbar_MaxVal = 0;
    Progressbar_CurrentVal = 0;
    TaskNumTotal=0;
    TaskNumFinished=0;
    NewTaskFinished=false;
    ETA=0;
    isForceRetryClicked=false;
    ForceRetryCount=1;
    isForceRetryEnabled=true;
    isSuccessiveFailuresDetected_VFI=false;
    //============== 界面初始化 ======================
    ui->checkBox_EnableVFI_Home->setEnabled(0);
    ui->checkBox_FrameInterpolationOnly_Video->setEnabled(0);
    ui->groupBox_FrameInterpolation->setEnabled(0);
    ui->pushButton_BrowserFile->setEnabled(0);
    ui->comboBox_ImageSaveFormat->setEnabled(0);
    ui->spinBox_ImageQualityLevel->setEnabled(0);
    emit Send_Table_EnableSorting(0);//启用文件列表的排序功能
    RemoveFile_FilesList_QAction_FileList->setEnabled(0);
    Apply_CustRes_QAction_FileList->setEnabled(0);
    Cancel_CustRes_QAction_FileList->setEnabled(0);
    this->setAcceptDrops(0);//禁止drop file
    pushButton_Stop_setEnabled_self(1);//启用stop button
    pushButton_Start_setEnabled_self(0);//禁用start button
    OutputSettingsArea_setEnabled(false);
    ui->pushButton_ClearList->setEnabled(0);
    ui->pushButton_RemoveItem->setEnabled(0);
    ui->groupBox_Engine->setEnabled(0);
    ui->groupBox_ScaleRaton_DenoiseLevel->setEnabled(0);
    ui->checkBox_OptGIF->setEnabled(0);
    ui->checkBox_DelOriginal->setEnabled(0);
    ui->checkBox_ReProcFinFiles->setEnabled(0);
    ui->pushButton_compatibilityTest->setEnabled(0);
    ui->pushButton_ReadFileList->setEnabled(0);
    progressbar_clear();
    ui->label_TimeCost->setText(tr("Time taken:NULL"));
    ui->label_ETA->setText(tr("ETA:NULL"));
    ui->label_TimeRemain->setText(tr("Time remaining:NULL"));
    ui->groupBox_video_settings->setEnabled(0);
    QAction_checkBox_MoveToRecycleBin_checkBox_DelOriginal->setEnabled(0);
    ui->pushButton_ForceRetry->setVisible(1);
    ui->checkBox_AutoDetectAlphaChannel->setEnabled(0);
    ui->groupBox_AudioDenoise->setEnabled(0);
    ui->checkBox_ProcessVideoBySegment->setEnabled(0);
    ui->spinBox_SegmentDuration->setEnabled(0);
    ui->comboBox_ImageStyle->setEnabled(0);
    ui->checkBox_PreProcessImage->setEnabled(0);
    ui->checkBox_AlwaysPreProcessAlphaPNG->setEnabled(0);
    on_lineEdit_GPUs_Anime4k_editingFinished();
    on_lineEdit_MultiGPUInfo_Waifu2xCaffe_editingFinished();
    ui->checkBox_ReplaceOriginalFile->setEnabled(0);
    ui->groupBox_CustRes->setEnabled(0);
    ui->checkBox_DisableResize_gif->setEnabled(0);
    //==========
    TimeCost=0;
    TimeCostTimer->start(1000);
    emit Send_TextBrowser_NewMessage(tr("Start processing files."));
    //==========
    PreLoad_Engines_Settings();//预读取引擎设定
    ui->comboBox_ImageStyle_Waifu2xCaffe->setEnabled(0);
    //==========
    Waifu2xMain = QtConcurrent::run(this, &MainWindow::Waifu2xMainThread);//启动waifu2x 主线程
}
/*
主调度线程
*/
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
        for ( int currentRowNumber = 0; currentRowNumber < rowCount_image; currentRowNumber++ )
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
            if(ui->checkBox_ReProcFinFiles->isChecked()==false)
            {
                if(Table_model_image->item(currentRowNumber,1)->text().toLower().contains("finished"))
                {
                    emit Send_progressbar_Add();
                    continue;
                }
            }
            if(Table_model_image->item(currentRowNumber,1)->text().toLower().contains("deleted"))
            {
                emit Send_progressbar_Add();
                continue;
            }
            //============== 判断权限 =====================
            if(file_isFilesFolderWritable_row_image(currentRowNumber)==false)
            {
                emit Send_progressbar_Add();
                continue;
            }
            //============ 自动跳过高分辨率文件 ===================
            if(Image_Gif_AutoSkip_CustRes(currentRowNumber,false))
            {
                emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(currentRowNumber, "Skipped");
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
                        QtConcurrent::run(this, &MainWindow::Waifu2x_NCNN_Vulkan_Image, currentRowNumber, false);
                        break;
                    }
                case 1:
                    {
                        mutex_ThreadNumRunning.lock();
                        ThreadNumRunning++;//线程数量统计+1
                        mutex_ThreadNumRunning.unlock();
                        QtConcurrent::run(this, &MainWindow::Waifu2x_Converter_Image, currentRowNumber, false);
                        break;
                    }
                case 2:
                    {
                        mutex_ThreadNumRunning.lock();
                        ThreadNumRunning++;//线程数量统计+1
                        mutex_ThreadNumRunning.unlock();
                        QtConcurrent::run(this, &MainWindow::SRMD_NCNN_Vulkan_Image, currentRowNumber, false);
                        break;
                    }
                case 3:
                    {
                        mutex_ThreadNumRunning.lock();
                        ThreadNumRunning++;//线程数量统计+1
                        mutex_ThreadNumRunning.unlock();
                        QtConcurrent::run(this, &MainWindow::Anime4k_Image, currentRowNumber, false);
                        break;
                    }
                case 4:
                    {
                        mutex_ThreadNumRunning.lock();
                        ThreadNumRunning++;//线程数量统计+1
                        mutex_ThreadNumRunning.unlock();
                        QtConcurrent::run(this, &MainWindow::Waifu2x_Caffe_Image, currentRowNumber, false);
                        break;
                    }
                case 5:
                    {
                        mutex_ThreadNumRunning.lock();
                        ThreadNumRunning++;//线程数量统计+1
                        mutex_ThreadNumRunning.unlock();
                        QtConcurrent::run(this, &MainWindow::Realsr_NCNN_Vulkan_Image, currentRowNumber, false);
                        break;
                    }
                case 6:
                    {
                        mutex_ThreadNumRunning.lock();
                        ThreadNumRunning++;//线程数量统计+1
                        mutex_ThreadNumRunning.unlock();
                        QtConcurrent::run(this, &MainWindow::SRMD_CUDA_Image, currentRowNumber, false);
                        break;
                    }
            }
            //================
            while (ThreadNumRunning >= ThreadNumMax)
            {
                Delay_msec_sleep(750);
            }
        }
    }
    while (ThreadNumRunning>0)
    {
        Delay_msec_sleep(750);
    }
    mutex_ThreadNumRunning.lock();
    ThreadNumRunning=0;//线程数量统计+1
    mutex_ThreadNumRunning.unlock();
    //=========================================================
    //                   GIF 线程调度
    //===========================================================
    isForceRetryEnabled=false;
    if(rowCount_gif>0)
    {
        double ScaleRatio_Gif = ui->doubleSpinBox_ScaleRatio_gif->value();
        bool isDoubleGifScaleRatio = (ScaleRatio_Gif != (int)ScaleRatio_Gif);
        //=====
        int GIFEngine = ui->comboBox_Engine_GIF->currentIndex();
        for ( int currentRowNumber = 0; currentRowNumber < rowCount_gif; currentRowNumber++ )
        {
            if(waifu2x_STOP)
            {
                waifu2x_STOP_confirm = true;
                return 0;//如果启用stop位,则直接return
            }
            //=============== 判断状态 ================
            if(ui->checkBox_ReProcFinFiles->isChecked()==false)
            {
                if(Table_model_gif->item(currentRowNumber,1)->text().toLower().contains("finished"))
                {
                    emit Send_progressbar_Add();
                    continue;
                }
            }
            if(Table_model_gif->item(currentRowNumber,1)->text().toLower().contains("deleted"))
            {
                emit Send_progressbar_Add();
                continue;
            }
            //============== 判断权限 =====================
            if(file_isFilesFolderWritable_row_gif(currentRowNumber)==false)
            {
                emit Send_progressbar_Add();
                continue;
            }
            //============ 自动跳过高分辨率文件 ===================
            if(Image_Gif_AutoSkip_CustRes(currentRowNumber,true))
            {
                emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(currentRowNumber, "Skipped");
                continue;
            }
            //=============== 判断是否需要加入自定义分辨率列表中 ============
            bool isNeedRemoveFromCustResList = false;
            if(isDoubleGifScaleRatio)
            {
                isNeedRemoveFromCustResList = Gif_DoubleScaleRatioPrep(currentRowNumber);
            }
            //=========
            mutex_ThreadNumRunning.lock();
            ThreadNumRunning=1;//线程数量统计+1
            mutex_ThreadNumRunning.unlock();
            switch(GIFEngine)
            {
                case 0:
                    {
                        Waifu2x_NCNN_Vulkan_GIF(currentRowNumber);
                        break;
                    }
                case 1:
                    {
                        Waifu2x_Converter_GIF(currentRowNumber);
                        break;
                    }
                case 2:
                    {
                        SRMD_NCNN_Vulkan_GIF(currentRowNumber);
                        break;
                    }
                case 3:
                    {
                        Anime4k_GIF(currentRowNumber);
                        break;
                    }
                case 4:
                    {
                        Waifu2x_Caffe_GIF(currentRowNumber);
                        break;
                    }
                case 5:
                    {
                        Realsr_NCNN_Vulkan_GIF(currentRowNumber);
                        break;
                    }
                case 6:
                    {
                        SRMD_CUDA_GIF(currentRowNumber);
                        break;
                    }
            }
            //========
            if(isNeedRemoveFromCustResList == true)Gif_RemoveFromCustResList(currentRowNumber);
            //========
            mutex_ThreadNumRunning.lock();
            ThreadNumRunning=0;//线程数量统计+1
            mutex_ThreadNumRunning.unlock();
        }
    }
    //=========================================================
    //                   视频 线程调度
    //===========================================================
    if(rowCount_video>0)
    {
        double ScaleRatio_video = ui->doubleSpinBox_ScaleRatio_video->value();
        bool isDoubleVideoScaleRatio = (ScaleRatio_video != (int)ScaleRatio_video);
        //=======
        int VideoEngine = 0;
        if(ui->checkBox_FrameInterpolationOnly_Video->isChecked()==true)
        {
            //仅插帧
            VideoEngine=99;
        }
        else
        {
            VideoEngine = ui->comboBox_Engine_Video->currentIndex();
        }
        //=======
        for ( int currentRowNumber = 0; currentRowNumber<rowCount_video; currentRowNumber++ )
        {
            if(waifu2x_STOP)
            {
                waifu2x_STOP_confirm = true;
                return 0;//如果启用stop位,则直接return
            }
            //=============== 判断状态 ================
            if(ui->checkBox_ReProcFinFiles->isChecked()==false)
            {
                if(Table_model_video->item(currentRowNumber,1)->text().toLower().contains("finished"))
                {
                    emit Send_progressbar_Add();
                    continue;
                }
            }
            if(Table_model_video->item(currentRowNumber,1)->text().toLower().contains("deleted"))
            {
                emit Send_progressbar_Add();
                continue;
            }
            //============== 判断权限 =====================
            if(file_isFilesFolderWritable_row_video(currentRowNumber)==false)
            {
                emit Send_progressbar_Add();
                continue;
            }
            //============ 自动跳过高分辨率文件 ===================
            if(Video_AutoSkip_CustRes(currentRowNumber))
            {
                emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(currentRowNumber, "Skipped");
                continue;
            }
            //=============== 判断是否需要加入自定义分辨率列表中 ============
            bool isNeedRemoveFromCustResList = false;
            if(isDoubleVideoScaleRatio)
            {
                isNeedRemoveFromCustResList = video_DoubleScaleRatioPrep(currentRowNumber);
            }
            //============= 获取时长, 判断是否需要分段处理 =================
            mutex_ThreadNumRunning.lock();
            ThreadNumRunning=1;//线程数量统计+1
            mutex_ThreadNumRunning.unlock();
            switch(VideoEngine)
            {
                case 0://w2x ncnn vulkan
                    {
                        if(video_isNeedProcessBySegment(currentRowNumber))
                        {
                            Waifu2x_NCNN_Vulkan_Video_BySegment(currentRowNumber);
                        }
                        else
                        {
                            Waifu2x_NCNN_Vulkan_Video(currentRowNumber);
                        }
                        break;
                    }
                case 1://w2x converter
                    {
                        if(video_isNeedProcessBySegment(currentRowNumber))
                        {
                            Waifu2x_Converter_Video_BySegment(currentRowNumber);
                        }
                        else
                        {
                            Waifu2x_Converter_Video(currentRowNumber);
                        }
                        break;
                    }
                case 2://a4k
                    {
                        if(video_isNeedProcessBySegment(currentRowNumber))
                        {
                            Anime4k_Video_BySegment(currentRowNumber);
                        }
                        else
                        {
                            Anime4k_Video(currentRowNumber);
                        }
                        break;
                    }
                case 3://srmd ncnn vulkan
                    {
                        if(video_isNeedProcessBySegment(currentRowNumber))
                        {
                            SRMD_NCNN_Vulkan_Video_BySegment(currentRowNumber);
                        }
                        else
                        {
                            SRMD_NCNN_Vulkan_Video(currentRowNumber);
                        }
                        break;
                    }
                case 4://w2x caffe
                    {
                        if(video_isNeedProcessBySegment(currentRowNumber))
                        {
                            Waifu2x_Caffe_Video_BySegment(currentRowNumber);
                        }
                        else
                        {
                            Waifu2x_Caffe_Video(currentRowNumber);
                        }
                        break;
                    }
                case 5://realsr ncnn vulkan
                    {
                        if(video_isNeedProcessBySegment(currentRowNumber))
                        {
                            Realsr_NCNN_Vulkan_Video_BySegment(currentRowNumber);
                        }
                        else
                        {
                            Realsr_NCNN_Vulkan_Video(currentRowNumber);
                        }
                        break;
                    }
                case 6://srmd cuda
                    {
                        if(video_isNeedProcessBySegment(currentRowNumber))
                        {
                            SRMD_CUDA_Video_BySegment(currentRowNumber);
                        }
                        else
                        {
                            SRMD_CUDA_Video(currentRowNumber);
                        }
                        break;
                    }
                case 99://frame interpolation
                    {
                        if(video_isNeedProcessBySegment(currentRowNumber))
                        {
                            FrameInterpolation_Video_BySegment(currentRowNumber);
                        }
                        else
                        {
                            FrameInterpolation_Video(currentRowNumber);
                        }
                        break;
                    }
            }
            //=======
            if(isNeedRemoveFromCustResList == true)video_RemoveFromCustResList(currentRowNumber);
            //========
            mutex_ThreadNumRunning.lock();
            ThreadNumRunning=0;//线程数量统计+1
            mutex_ThreadNumRunning.unlock();
        }
    }
    isForceRetryEnabled=true;
    if(waifu2x_STOP)
    {
        waifu2x_STOP_confirm = true;
        return 0;//如果启用stop位,则直接return
    }
    emit Send_Waifu2x_Finished();
    return 0;
}
/*
正常(非手动停止)时执行的结束代码
*/
void MainWindow::Waifu2x_Finished()
{
    //=================== 提示音 =================================
    QtConcurrent::run(this, &MainWindow::Play_NFSound);
    //===================== 关机 ==============================
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
/*
手动停止处理文件时执行的代码
*/
void MainWindow::Waifu2x_Finished_manual()
{
    TimeCostTimer->stop();
    //================== 界面恢复 ===============================
    ui->checkBox_EnableVFI_Home->setEnabled(1);
    if(ui->groupBox_FrameInterpolation->isChecked())ui->checkBox_FrameInterpolationOnly_Video->setEnabled(1);
    ui->groupBox_FrameInterpolation->setEnabled(1);
    ui->pushButton_BrowserFile->setEnabled(1);
    ui->comboBox_ImageSaveFormat->setEnabled(1);
    on_comboBox_ImageSaveFormat_currentIndexChanged(1);
    emit Send_Table_EnableSorting(1);//启用文件列表的排序功能
    RemoveFile_FilesList_QAction_FileList->setEnabled(1);
    Apply_CustRes_QAction_FileList->setEnabled(1);
    Cancel_CustRes_QAction_FileList->setEnabled(1);
    this->setAcceptDrops(1);
    pushButton_Stop_setEnabled_self(0);
    pushButton_Start_setEnabled_self(1);
    if(ui->checkBox_ReplaceOriginalFile->isChecked()==false)
    {
        ui->groupBox_OutPut->setEnabled(1);
        OutputSettingsArea_setEnabled(true);
        ui->checkBox_DelOriginal->setEnabled(1);
    }
    ui->pushButton_ClearList->setEnabled(1);
    ui->pushButton_RemoveItem->setEnabled(1);
    ui->groupBox_Engine->setEnabled(1);
    ui->groupBox_ScaleRaton_DenoiseLevel->setEnabled(1);
    ui->checkBox_OptGIF->setEnabled(1);
    ui->checkBox_ReProcFinFiles->setEnabled(1);
    ui->pushButton_compatibilityTest->setEnabled(1);
    ui->pushButton_ReadFileList->setEnabled(1);
    ui->groupBox_video_settings->setEnabled(1);
    if(ui->checkBox_DelOriginal->isChecked())QAction_checkBox_MoveToRecycleBin_checkBox_DelOriginal->setEnabled(1);
    ui->pushButton_ForceRetry->setVisible(0);
    ui->groupBox_AudioDenoise->setEnabled(1);
    ui->checkBox_ProcessVideoBySegment->setEnabled(1);
    if(ui->checkBox_ProcessVideoBySegment->isChecked())
    {
        ui->spinBox_SegmentDuration->setEnabled(1);
    }
    on_comboBox_model_vulkan_currentIndexChanged(0);
    isWaifu2xCaffeEnabled();//判断是否启用caffe图片风格combobox
    emit Send_CurrentFileProgress_Stop();
    ui->checkBox_PreProcessImage->setEnabled(1);
    ui->checkBox_AlwaysPreProcessAlphaPNG->setEnabled(1);
    checkBox_ReplaceOriginalFile_setEnabled_True_Self();
    ui->checkBox_AutoDetectAlphaChannel->setEnabled(1);
    ui->groupBox_CustRes->setEnabled(1);
    ui->checkBox_DisableResize_gif->setEnabled(1);
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
    ForceRetryCount=1;
    isForceRetryEnabled=true;
    isSuccessiveFailuresDetected_VFI=false;
    //================= 杀死卡死在后台的进程 =================
    QStringList TaskNameList;
    TaskNameList << "convert_waifu2xEX.exe"<<"ffmpeg_waifu2xEX.exe"<<"ffprobe_waifu2xEX.exe"<<"identify_waifu2xEX.exe"<<"gifsicle_waifu2xEX.exe"<<"waifu2x-ncnn-vulkan_waifu2xEX.exe"
                 <<"waifu2x-ncnn-vulkan-fp16p_waifu2xEX.exe"<<"Anime4K_waifu2xEX.exe"<<"waifu2x-caffe_waifu2xEX.exe"<<"srmd-ncnn-vulkan_waifu2xEX.exe"<<"realsr-ncnn-vulkan_waifu2xEX.exe"
                 <<"waifu2x-converter-cpp_waifu2xEX.exe"<<"sox_waifu2xEX.exe"<<"rife-ncnn-vulkan_waifu2xEX.exe"<<"cain-ncnn-vulkan_waifu2xEX.exe"<<"dain-ncnn-vulkan_waifu2xEX.exe"
                 <<"srmd-cuda_waifu2xEX.exe";
    KILL_TASK_QStringList(TaskNameList,true);
    //================= 生成处理报告 =================
    ShowFileProcessSummary();
    //================= 打开输出文件夹 ====================
    if(ui->checkBox_AutoOpenOutputPath->isChecked()==true && ui->checkBox_OutPath_isEnabled->isChecked()==true)OpenOutputFolder();
    //======================
    FinishedProcessing_DN();
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
        QString file_name = file_getBaseName(SourceFilePath);
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
            QString file_name = file_getBaseName(Target_fullpPath);
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
/*
块大小调节 NCNN-Vulkan & Converter
*/
/*
块大小调整按钮 +
*/
int MainWindow::AddTileSize_NCNNVulkan_Converter(int OrginalTileSize)
{
    int TileSize = OrginalTileSize;
    //判断是否为2的幂数
    if((TileSize&(TileSize-1))==0)
    {
        TileSize++;
    }
    if((TileSize&(TileSize-1))!=0)
    {
        for(int intNum=1; true; intNum++)
        {
            int pow_ =pow(2,intNum);
            if(pow_>999999999)
            {
                return OrginalTileSize;
            }
            if(pow_>=TileSize)
            {
                return pow_;
            }
        }
    }
    return OrginalTileSize+1;
}
/*
块大小调整按钮 -
*/
int MainWindow::MinusTileSize_NCNNVulkan_Converter(int OrginalTileSize)
{
    int TileSize = OrginalTileSize;
    //判断是否为2的幂数
    if((TileSize&(TileSize-1))==0)
    {
        TileSize--;
    }
    if((TileSize&(TileSize-1))!=0)
    {
        for(int intNum=1; true; intNum++)
        {
            int pow_ =pow(2,intNum);
            if(pow_>999999999)
            {
                return OrginalTileSize;
            }
            if(pow_>=TileSize)
            {
                return pow(2,intNum-1);
            }
        }
    }
    return OrginalTileSize-1;
}
/*
预读取引擎设定
*/
void MainWindow::PreLoad_Engines_Settings()
{
    //======================
    //Waifu2x-NCNN-Vulkan
    //======================
    if(ui->comboBox_Engine_Image->currentIndex()==0)
    {
        Waifu2x_NCNN_Vulkan_PreLoad_Settings_Str = Waifu2x_NCNN_Vulkan_PreLoad_Settings();
    }
    //======================
    //Waifu2x-Converter
    //======================
    if(ui->comboBox_Engine_Image->currentIndex()==1||ui->comboBox_Engine_GIF->currentIndex()==1||ui->comboBox_Engine_Video->currentIndex()==1)
    {
        Waifu2xConverter_PreLoad_Settings_Str = Waifu2xConverter_PreLoad_Settings();
    }
    //======================
    //SRMD-NCNN-Vulkan
    //======================
    if(ui->comboBox_Engine_Image->currentIndex()==2)
    {
        SrmdNcnnVulkan_PreLoad_Settings_Str = SrmdNcnnVulkan_PreLoad_Settings();
    }
    //======================
    //Anime4k
    //======================
    if(ui->comboBox_Engine_Image->currentIndex()==3||ui->comboBox_Engine_GIF->currentIndex()==3||ui->comboBox_Engine_Video->currentIndex()==2)
    {
        Anime4k_PreLoad_Settings_Str = Anime4k_PreLoad_Settings();
        //预加载HDN等级
        if(ui->checkBox_ACNet_Anime4K->isChecked()&&ui->checkBox_HDNMode_Anime4k->isChecked())
        {
            int HDNDenoiseLevel_value = 0;
            if(ui->comboBox_Engine_Image->currentIndex()==3)//图片
            {
                HDNDenoiseLevel_value = ui->spinBox_DenoiseLevel_image->value();
                if(HDNDenoiseLevel_value>=1&&HDNDenoiseLevel_value<=3)
                {
                    HDNDenoiseLevel_image = " -L "+QString::number(HDNDenoiseLevel_value,10)+" ";
                }
                else
                {
                    HDNDenoiseLevel_image = "";
                }
            }
            if(ui->comboBox_Engine_GIF->currentIndex()==3)//GIF
            {
                HDNDenoiseLevel_value = ui->spinBox_DenoiseLevel_gif->value();
                if(HDNDenoiseLevel_value>=1&&HDNDenoiseLevel_value<=3)
                {
                    HDNDenoiseLevel_gif = " -L "+QString::number(HDNDenoiseLevel_value,10)+" ";
                }
                else
                {
                    HDNDenoiseLevel_gif = "";
                }
            }
            if(ui->comboBox_Engine_Video->currentIndex()==2)//视频
            {
                HDNDenoiseLevel_value = ui->spinBox_DenoiseLevel_video->value();
                if(HDNDenoiseLevel_value>=1&&HDNDenoiseLevel_value<=3)
                {
                    HDNDenoiseLevel_video = " -L "+QString::number(HDNDenoiseLevel_value,10)+" ";
                }
                else
                {
                    HDNDenoiseLevel_video = "";
                }
            }
        }
    }
    //======================
    //Waifu2x-Caffe
    //======================
    if(isWaifu2xCaffeEnabled())
    {
        Waifu2xCaffe_PreLoad_Settings_Str = Waifu2xCaffe_PreLoad_Settings();
    }
    //======================
    //Realsr-NCNN-Vulkan
    //======================
    if(ui->comboBox_Engine_Image->currentIndex()==5)
    {
        Realsr_NCNN_Vulkan_PreLoad_Settings_Str = Realsr_NCNN_Vulkan_PreLoad_Settings();
    }
}
/*
等待引擎IO结束
1.读取初始文件列表
2.获取初始文件大小列表
3.等待数秒,再次读取文件大小判断是否发生IO,若没有IO则返回文件列表.
*/
QStringList MainWindow::WaitForEngineIO(QStringList OutPutFilesFullPathList)
{
    QStringList ExistFileList;
    QString fullpath_tmp;
    for(int i = 0; i < OutPutFilesFullPathList.size(); i++)
    {
        fullpath_tmp= OutPutFilesFullPathList.at(i);
        if(QFile::exists(fullpath_tmp))
        {
            ExistFileList.append(fullpath_tmp);
        }
    }
    QList<qint64> FilesSizeList;
    for(int i = 0; i < ExistFileList.size(); i++)
    {
        QFileInfo finfo(ExistFileList.at(i));
        FilesSizeList.append(finfo.size());
    }
    do
    {
        Delay_sec_sleep(3);
        QList<qint64> FilesSizeList_tmp;
        for(int i = 0; i < ExistFileList.size(); i++)
        {
            QFileInfo finfo(ExistFileList.at(i));
            FilesSizeList_tmp.append(finfo.size());
        }
        if(FilesSizeList_tmp == FilesSizeList)
        {
            break;
        }
        else
        {
            FilesSizeList = FilesSizeList_tmp;
        }
    }
    while (true);
    return ExistFileList;
}
QStringList MainWindow::WaitForEngineIO_NcnnVulkan(QString OutputFolderFullPath)
{
    QStringList ExistFileList = file_getFileNames_in_Folder_nofilter(OutputFolderFullPath);
    QString fullpath_tmp;
    QList<qint64> FilesSizeList;
    for(int i = 0; i < ExistFileList.size(); i++)
    {
        QFileInfo finfo(OutputFolderFullPath+"/"+ExistFileList.at(i));
        FilesSizeList.append(finfo.size());
    }
    do
    {
        Delay_sec_sleep(3);
        QList<qint64> FilesSizeList_tmp;
        for(int i = 0; i < ExistFileList.size(); i++)
        {
            QFileInfo finfo(OutputFolderFullPath+"/"+ExistFileList.at(i));
            FilesSizeList_tmp.append(finfo.size());
        }
        if(FilesSizeList_tmp == FilesSizeList)
        {
            break;
        }
        else
        {
            FilesSizeList = FilesSizeList_tmp;
        }
    }
    while (true);
    return ExistFileList;
}
/*
恢复拆分帧文件夹
*/
void MainWindow::Restore_SplitFramesFolderPath(QString SplitFramesFolderPath, QStringList GPU_SplitFramesFolderPath_List)
{
    if(file_isDirExist(SplitFramesFolderPath)==false)return;
    for(int x = 0; x < GPU_SplitFramesFolderPath_List.size(); x++)
    {
        QString GPUfolder = GPU_SplitFramesFolderPath_List.at(x);
        if(file_isDirExist(GPUfolder)==false)continue;
        QStringList file_waitformove = file_getFileNames_in_Folder_nofilter(GPUfolder);
        for(int i = 0; i < file_waitformove.size(); i++)
        {
            QString FileName = file_waitformove.at(i);
            QFile::remove(SplitFramesFolderPath+"/"+FileName);
            QFile::rename(GPUfolder+"/"+FileName,SplitFramesFolderPath+"/"+FileName);
        }
        file_DelDir(GPUfolder);
    }
}
/*
复制一个文件夹的内容到另一个文件夹 Ncnn Vulkan
*/
void MainWindow::file_MoveFiles_Folder_NcnnVulkanFolderProcess(QString Old_folder, QString New_folder, bool Delete_)
{
    if(file_isDirExist(Old_folder)==false || file_isDirExist(New_folder)==false)return;
    QStringList file_waitformove = file_getFileNames_in_Folder_nofilter(Old_folder);
    QString NewName="";
    QString FileName="";
    for(int i = 0; i < file_waitformove.size(); i++)
    {
        FileName = file_waitformove.at(i);
        if(FileName.count(".")>1)
        {
            NewName = file_getBaseName(Old_folder+"/"+FileName);
            do
            {
                if(NewName.count(".")<2)break;
                NewName = file_getBaseName(Old_folder+"/"+NewName);
            }
            while(true);
            QFile::rename(Old_folder+"/"+FileName,Old_folder+"/"+NewName);
            FileName = NewName;
        }
        QFile::remove(New_folder+"/"+FileName);
        QFile::rename(Old_folder+"/"+FileName,New_folder+"/"+FileName);
    }
    if(Delete_)file_DelDir(Old_folder);
}
/*
//杀死进程,可选管理员
bool MainWindow::KILL_TASK_(QString TaskName,bool RequestAdmin)
{
    TaskName=TaskName.trimmed();
    if(TaskName=="")return false;
    //===============
    QProcess Get_tasklist;
    Get_tasklist.start("tasklist /fo csv");
    while(!Get_tasklist.waitForStarted(500)) {}
    while(!Get_tasklist.waitForFinished(500)) {}
    if(Get_tasklist.readAllStandardOutput().contains(TaskName.toUtf8())==false)
    {
        return true;
    }
    //===============
    QProcess Close;
    Close.start("taskkill /f /t /fi \"imagename eq "+TaskName+"\"");
    while(!Close.waitForStarted(500)) {}
    while(!Close.waitForFinished(500)) {}
    Get_tasklist.start("tasklist /fo csv");
    while(!Get_tasklist.waitForStarted(500)) {}
    while(!Get_tasklist.waitForFinished(500)) {}
    if(Get_tasklist.readAllStandardOutput().contains(TaskName.toUtf8()) && RequestAdmin==true)
    {
        ExecuteCMD_batFile("taskkill /f /t /fi \"imagename eq "+TaskName+"\"",true);
        return true;
    }
    return true;
}*/
/*
杀死进程列表,可选管理员
*/
bool MainWindow::KILL_TASK_QStringList(QStringList TaskNameList,bool RequestAdmin)
{
    TaskNameList.removeAll("");
    if(TaskNameList.isEmpty())return false;
    //===============
    QProcess Get_tasklist;
    Get_tasklist.start("tasklist /fo csv");
    while(!Get_tasklist.waitForStarted(500)) {}
    while(!Get_tasklist.waitForFinished(500)) {}
    QString RunningTaskList = Get_tasklist.readAllStandardOutput();
    //===
    QString TaskName_tmp;
    QStringList TaskNameList_tmp = TaskNameList;
    //===
    for(int i=0; i<TaskNameList.size(); i++)
    {
        TaskName_tmp = TaskNameList.at(i);
        if(RunningTaskList.contains(TaskName_tmp)==false)
        {
            TaskNameList_tmp.removeAll(TaskName_tmp);
        }
    }
    if(TaskNameList_tmp.isEmpty())return true;
    //===============
    QString CMD_commands = "";
    for(int i=0; i<TaskNameList_tmp.size(); i++)
    {
        CMD_commands.append("taskkill /f /t /fi \"imagename eq "+TaskNameList_tmp.at(i)+"\"\n");
    }
    if(RequestAdmin==true)
    {
        emit Send_TextBrowser_NewMessage(tr("Please grant administrator permissions to kill sub processes stuck in the background."));
    }
    ExecuteCMD_batFile(CMD_commands,RequestAdmin);
    //===============
    return true;
}
/*
生成处理总结报告
*/
void MainWindow::ShowFileProcessSummary()
{
    if(ui->checkBox_SummaryPopup->isChecked()==false)
    {
        int rowCount_image = Table_model_image->rowCount();
        int rowCount_gif = Table_model_gif->rowCount();
        int rowCount_video = Table_model_video->rowCount();
        //===
        int NumOfFailedFiles = 0;
        QString FailedFilesList = "\n";
        //检查图片文件列表
        for ( int RowNum = 0; RowNum < rowCount_image; RowNum++ )
        {
            QString CurrentRowStatus = Table_model_image->item(RowNum,1)->text().toLower();
            if(CurrentRowStatus.contains("fail"))
            {
                NumOfFailedFiles++;
                FailedFilesList.append(tr("Failed: [")+Table_model_image->item(RowNum,2)->text()+"]\n");
                continue;
            }
        }
        //检查GIF文件列表
        for ( int RowNum = 0; RowNum < rowCount_gif; RowNum++ )
        {
            QString CurrentRowStatus = Table_model_gif->item(RowNum,1)->text().toLower();
            if(CurrentRowStatus.contains("fail"))
            {
                NumOfFailedFiles++;
                FailedFilesList.append(tr("Failed: [")+Table_model_gif->item(RowNum,2)->text()+"]\n");
                continue;
            }
        }
        //检查视频文件列表
        for ( int RowNum = 0; RowNum < rowCount_video; RowNum++ )
        {
            QString CurrentRowStatus = Table_model_video->item(RowNum,1)->text().toLower();
            if(CurrentRowStatus.contains("fail"))
            {
                NumOfFailedFiles++;
                FailedFilesList.append(tr("Failed: [")+Table_model_video->item(RowNum,2)->text()+"]\n");
                continue;
            }
        }
        //输出失败文件列表
        if(NumOfFailedFiles>0)
        {
            emit Send_TextBrowser_NewMessage(FailedFilesList);
        }
        return;
    }
    int rowCount_image = Table_model_image->rowCount();
    int rowCount_gif = Table_model_gif->rowCount();
    int rowCount_video = Table_model_video->rowCount();
    //===
    int NumOfFinishedFiles = 0;
    int NumOfSkippedFiles = 0;
    int NumOfFailedFiles = 0;
    QString FailedFilesList = "\n";
    //===
    //检查图片文件列表
    for ( int RowNum = 0; RowNum < rowCount_image; RowNum++ )
    {
        QString CurrentRowStatus = Table_model_image->item(RowNum,1)->text().toLower();
        if(CurrentRowStatus.contains("finished"))
        {
            NumOfFinishedFiles++;
            continue;
        }
        if(CurrentRowStatus.contains("fail"))
        {
            NumOfFailedFiles++;
            FailedFilesList.append(tr("Failed: [")+Table_model_image->item(RowNum,2)->text()+"]\n");
            continue;
        }
        if(ui->checkBox_AutoSkip_CustomRes->isChecked() && CurrentRowStatus.contains("skipped"))
        {
            NumOfSkippedFiles++;
            continue;
        }
    }
    //检查GIF文件列表
    for ( int RowNum = 0; RowNum < rowCount_gif; RowNum++ )
    {
        QString CurrentRowStatus = Table_model_gif->item(RowNum,1)->text().toLower();
        if(CurrentRowStatus.contains("finished"))
        {
            NumOfFinishedFiles++;
            continue;
        }
        if(CurrentRowStatus.contains("fail"))
        {
            NumOfFailedFiles++;
            FailedFilesList.append(tr("Failed: [")+Table_model_gif->item(RowNum,2)->text()+"]\n");
            continue;
        }
        if(ui->checkBox_AutoSkip_CustomRes->isChecked() && CurrentRowStatus.contains("skipped"))
        {
            NumOfSkippedFiles++;
            continue;
        }
    }
    //检查视频文件列表
    for ( int RowNum = 0; RowNum < rowCount_video; RowNum++ )
    {
        QString CurrentRowStatus = Table_model_video->item(RowNum,1)->text().toLower();
        if(CurrentRowStatus.contains("finished"))
        {
            NumOfFinishedFiles++;
            continue;
        }
        if(CurrentRowStatus.contains("fail"))
        {
            NumOfFailedFiles++;
            FailedFilesList.append(tr("Failed: [")+Table_model_video->item(RowNum,2)->text()+"]\n");
            continue;
        }
        if(ui->checkBox_AutoSkip_CustomRes->isChecked() && CurrentRowStatus.contains("skipped"))
        {
            NumOfSkippedFiles++;
            continue;
        }
    }
    //输出报告
    QString Summary_qstr = tr("Total: ")+QString("%1").arg((rowCount_image+rowCount_gif+rowCount_video))+"\n\n"+tr("Finished: ")+QString("%1").arg(NumOfFinishedFiles)+"\n\n"+tr("Failed: ")+QString("%1").arg(NumOfFailedFiles);
    if(ui->checkBox_AutoSkip_CustomRes->isChecked())
    {
        Summary_qstr.append("\n\n"+tr("Skipped: ")+QString("%1").arg(NumOfSkippedFiles));
    }
    if(NumOfFailedFiles>0)
    {
        emit Send_TextBrowser_NewMessage(FailedFilesList);
        Summary_qstr.append("\n\n"+tr("Info of failed files will be listed in the text browser at the bottom of the GUI."));
    }
    //===
    QMessageBox *MSG_Summary = new QMessageBox();
    MSG_Summary->setWindowTitle(tr("Summary"));
    MSG_Summary->setText(Summary_qstr);
    MSG_Summary->setIcon(QMessageBox::Information);
    MSG_Summary->setModal(true);
    MSG_Summary->show();
    //===
    return;
}
