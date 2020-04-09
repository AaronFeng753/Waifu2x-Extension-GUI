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
    Delay_msec_sleep(2000);
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
                    Delay_msec_sleep(500);
                }
                waifu2x_STOP_confirm = true;
                return 0;//如果启用stop位,则直接return
            }
            //=============== 判断状态 ================
            if(!ui->checkBox_ReProcFinFiles->checkState())
            {
                if(Table_model_image->item(i,1)->text().contains("Finished"))
                {
                    emit Send_progressbar_Add();
                    continue;
                }
            }
            if(Table_model_image->item(i,1)->text().contains("deleted"))
            {
                emit Send_progressbar_Add();
                continue;
            }
            //=========
            ThreadNumMax = ui->spinBox_ThreadNum_image->value();//获取image线程数量最大值
            //===================================================================================
            //=============== 判断图片是否含有透明通道, 如果有, 则强制调用converter处理图片 ============
            //===================================================================================
            if(Imgae_hasAlphaChannel(i))
            {
                emit Send_TextBrowser_NewMessage(tr("Detected that the current picture [")+Table_model_image->item(i,2)->text()+tr("] contains Alpha channel, so it automatically uses the waifu2x-converter engine to process the current picture and force it to save as PNG."));
                mutex_ThreadNumRunning.lock();
                ThreadNumRunning++;//线程数量统计+1
                mutex_ThreadNumRunning.unlock();
                QtConcurrent::run(this, &MainWindow::Waifu2x_Converter_Image, i);
                while (ThreadNumRunning >= ThreadNumMax)
                {
                    Delay_msec_sleep(500);
                }
                continue;
            }
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
                            Delay_msec_sleep(500);
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
                            Delay_msec_sleep(500);
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
                            Delay_msec_sleep(500);
                        }
                        break;
                    }
            }
        }
    }
    while (ThreadNumRunning>0)
    {
        Delay_msec_sleep(500);
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
                    Delay_msec_sleep(500);
                }
                waifu2x_STOP_confirm = true;
                return 0;//如果启用stop位,则直接return
            }
            //=============== 判断状态 ================
            if(!ui->checkBox_ReProcFinFiles->checkState())
            {
                if(Table_model_gif->item(i,1)->text().contains("Finished"))
                {
                    emit Send_progressbar_Add();
                    continue;
                }
            }
            if(Table_model_gif->item(i,1)->text().contains("deleted"))
            {
                emit Send_progressbar_Add();
                continue;
            }
            //=========
            ThreadNumMax = ui->spinBox_ThreadNum_gif->value();//获取gif线程数量最大值
            switch(GIFEngine)
            {
                case 0:
                    {
                        mutex_ThreadNumRunning.lock();
                        ThreadNumRunning++;//线程数量统计+1
                        mutex_ThreadNumRunning.unlock();
                        QtConcurrent::run(this, &MainWindow::Waifu2x_NCNN_Vulkan_GIF, i);
                        while (ThreadNumRunning >= ThreadNumMax)
                        {
                            Delay_msec_sleep(500);
                        }
                        break;
                    }
                case 1:
                    {
                        mutex_ThreadNumRunning.lock();
                        ThreadNumRunning++;//线程数量统计+1
                        mutex_ThreadNumRunning.unlock();
                        QtConcurrent::run(this, &MainWindow::Waifu2x_Converter_GIF, i);
                        while (ThreadNumRunning >= ThreadNumMax)
                        {
                            Delay_msec_sleep(500);
                        }
                        break;
                    }
                case 2:
                    {
                        mutex_ThreadNumRunning.lock();
                        ThreadNumRunning++;//线程数量统计+1
                        mutex_ThreadNumRunning.unlock();
                        QtConcurrent::run(this, &MainWindow::SRMD_NCNN_Vulkan_GIF, i);
                        while (ThreadNumRunning >= ThreadNumMax)
                        {
                            Delay_msec_sleep(500);
                        }
                        break;
                    }
            }
        }
    }
    while (ThreadNumRunning>0)
    {
        Delay_msec_sleep(500);
    }
    if(rowCount_video>0)
    {
        int VideoEngine = ui->comboBox_Engine_Video->currentIndex();
        for ( int i = 0; i<rowCount_video; i++ )
        {
            if(waifu2x_STOP)
            {
                while (ThreadNumRunning > 0)
                {
                    Delay_msec_sleep(500);
                }
                waifu2x_STOP_confirm = true;
                return 0;//如果启用stop位,则直接return
            }
            //=============== 判断状态 ================
            if(!ui->checkBox_ReProcFinFiles->checkState())
            {
                if(Table_model_video->item(i,1)->text().contains("Finished"))
                {
                    emit Send_progressbar_Add();
                    continue;
                }
            }
            if(Table_model_video->item(i,1)->text().contains("deleted"))
            {
                emit Send_progressbar_Add();
                continue;
            }
            //=========
            ThreadNumMax = ui->spinBox_ThreadNum_video->value();//获取video线程数量最大值
            switch(VideoEngine)
            {
                case 0:
                    {
                        mutex_ThreadNumRunning.lock();
                        ThreadNumRunning++;//线程数量统计+1
                        mutex_ThreadNumRunning.unlock();
                        if(ui->checkBox_ProcessVideoBySegment->checkState())
                        {
                            QtConcurrent::run(this, &MainWindow::Waifu2x_NCNN_Vulkan_Video_BySegment, i);
                        }
                        else
                        {
                            QtConcurrent::run(this, &MainWindow::Waifu2x_NCNN_Vulkan_Video, i);
                        }
                        while (ThreadNumRunning >= ThreadNumMax)
                        {
                            Delay_msec_sleep(500);
                        }
                        break;
                    }
                case 1:
                    {
                        mutex_ThreadNumRunning.lock();
                        ThreadNumRunning++;//线程数量统计+1
                        mutex_ThreadNumRunning.unlock();
                        if(ui->checkBox_ProcessVideoBySegment->checkState())
                        {
                            QtConcurrent::run(this, &MainWindow::Waifu2x_Converter_Video_BySegment, i);
                        }
                        else
                        {
                            QtConcurrent::run(this, &MainWindow::Waifu2x_Converter_Video, i);
                        }
                        while (ThreadNumRunning >= ThreadNumMax)
                        {
                            Delay_msec_sleep(500);
                        }
                        break;
                    }
                case 2:
                    {
                        mutex_ThreadNumRunning.lock();
                        ThreadNumRunning++;//线程数量统计+1
                        mutex_ThreadNumRunning.unlock();
                        if(ui->checkBox_ProcessVideoBySegment->checkState())
                        {
                            QtConcurrent::run(this, &MainWindow::Anime4k_Video_BySegment, i);
                        }
                        else
                        {
                            QtConcurrent::run(this, &MainWindow::Anime4k_Video, i);
                        }
                        while (ThreadNumRunning >= ThreadNumMax)
                        {
                            Delay_msec_sleep(500);
                        }
                        break;
                    }
                case 3:
                    {
                        mutex_ThreadNumRunning.lock();
                        ThreadNumRunning++;//线程数量统计+1
                        mutex_ThreadNumRunning.unlock();
                        if(ui->checkBox_ProcessVideoBySegment->checkState())
                        {
                            QtConcurrent::run(this, &MainWindow::SRMD_NCNN_Vulkan_Video_BySegment, i);
                        }
                        else
                        {
                            QtConcurrent::run(this, &MainWindow::SRMD_NCNN_Vulkan_Video, i);
                        }
                        while (ThreadNumRunning >= ThreadNumMax)
                        {
                            Delay_msec_sleep(500);
                        }
                        break;
                    }
            }
        }
    }
    while (ThreadNumRunning>0)
    {
        Delay_msec_sleep(500);
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
    if(ui->checkBox_NfSound->checkState())
    {
        QtConcurrent::run(this, &MainWindow::Play_NFSound);
    }
    //===================== 关机 ==============================
    if(ui->checkBox_AutoTurnOFF->checkState())
    {
        QMessageBox *AutoShutDown = new QMessageBox();
        AutoShutDown->setWindowTitle(tr("Warning!"));
        AutoShutDown->setText(tr("The computer will automatically shut down in 60 seconds!"));
        AutoShutDown->setIcon(QMessageBox::Warning);
        AutoShutDown->setModal(false);
        AutoShutDown->show();
        emit Send_TextBrowser_NewMessage(tr("The computer will automatically shut down in 60 seconds!"));
        //关机前自动保存设置
        if(ui->checkBox_AutoSaveSettings->checkState())
        {
            emit Send_Settings_Save();
        }
        QtConcurrent::run(this, &MainWindow::SystemShutDown_Countdown);
    }
    //==================== 进度条 =================================
    progressbar_SetToMax(Progressbar_MaxVal);
    //=============================================================
    emit Send_TextBrowser_NewMessage(tr("Process finished."));
    //============================================================
    Waifu2x_Finished_manual();
}

void MainWindow::Waifu2x_Finished_manual()
{
    TimeCostTimer->stop();
    //================== 界面恢复 ===============================
    this->setAcceptDrops(1);
    ui->pushButton_Stop->setEnabled(0);
    ui->pushButton_Start->setEnabled(1);
    ui->groupBox_Input->setEnabled(1);
    ui->groupBox_OutPut->setEnabled(1);
    ui->pushButton_ClearList->setEnabled(1);
    ui->pushButton_RemoveItem->setEnabled(1);
    ui->groupBox_Engine->setEnabled(1);
    ui->groupBox_ScaleRaton_DenoiseLevel->setEnabled(1);
    ui->checkBox_OptGIF->setEnabled(1);
    ui->checkBox_SaveAsJPG->setEnabled(1);
    if(ui->checkBox_SaveAsJPG->checkState())
    {
        ui->checkBox_CompressJPG->setEnabled(1);
    }
    ui->checkBox_DelOriginal->setEnabled(1);
    ui->checkBox_ReProcFinFiles->setEnabled(1);
    ui->pushButton_compatibilityTest->setEnabled(1);
    ui->pushButton_CustRes_cancel->setEnabled(1);
    ui->pushButton_CustRes_apply->setEnabled(1);
    ui->pushButton_ReadFileList->setEnabled(1);
    ui->comboBox_AspectRatio_custRes->setEnabled(1);
    ui->spinBox_JPGCompressedQuality->setEnabled(1);
    ui->groupBox_video_settings->setEnabled(1);
    if(ui->checkBox_DelOriginal->checkState())ui->checkBox_Move2RecycleBin->setEnabled(1);
    ui->pushButton_ForceRetry->setEnabled(0);
    ui->checkBox_AutoDetectAlphaChannel->setEnabled(1);
    ui->groupBox_AudioDenoise->setEnabled(1);
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

int MainWindow::Waifu2x_Compatibility_Test()
{
    emit Send_TextBrowser_NewMessage(tr("Compatibility test is ongoing, please wait."));
    //===============
    QString InputPath = Current_Path + "/Compatibility_Test/Compatibility_Test.jpg";
    QString OutputPath = Current_Path + "/Compatibility_Test/res.jpg";
    QFile::remove(OutputPath);
    //==============
    QString Waifu2x_folder_path = Current_Path + "/waifu2x-ncnn-vulkan";
    QString program = Waifu2x_folder_path + "/waifu2x-ncnn-vulkan_waifu2xEX.exe";
    QString model_path = Waifu2x_folder_path+"/models-upconv_7_anime_style_art_rgb";
    QProcess *Waifu2x_vulkan = new QProcess();
    QString cmd = "\"" + program + "\"" + " -i " + "\"" + InputPath + "\"" + " -o " + "\"" + OutputPath + "\"" + " -s 2 -n 0 -t 50 -m " + "\"" + model_path + "\"" + " -j 1:1:1";
    Waifu2x_vulkan->start(cmd);
    while(!Waifu2x_vulkan->waitForStarted(100)&&!QProcess_stop) {}
    while(!Waifu2x_vulkan->waitForFinished(100)&&!QProcess_stop) {}
    if(file_isFileExist(OutputPath))
    {
        emit Send_TextBrowser_NewMessage(tr("Compatible with waifu2x-ncnn-vulkan: Yes"));
    }
    else
    {
        emit Send_TextBrowser_NewMessage(tr("Compatible with waifu2x-ncnn-vulkan: No. [Advice: Re-install gpu driver or update it to the latest.]"));
    }
    QFile::remove(OutputPath);
    //================
    Waifu2x_folder_path = Current_Path + "/waifu2x-ncnn-vulkan-old";
    program = Waifu2x_folder_path + "/waifu2x-ncnn-vulkan_waifu2xEX.exe";
    model_path = Waifu2x_folder_path+"/models-upconv_7_anime_style_art_rgb";
    QProcess *Waifu2x_vulkan_old = new QProcess();
    cmd = "\"" + program + "\"" + " -i " + "\"" + InputPath + "\"" + " -o " + "\"" + OutputPath + "\"" + " -s 2 -n 0 -t 50 -m " + "\"" + model_path + "\"" + " -j 1:1:1";
    Waifu2x_vulkan_old->start(cmd);
    while(!Waifu2x_vulkan_old->waitForStarted(100)&&!QProcess_stop) {}
    while(!Waifu2x_vulkan_old->waitForFinished(100)&&!QProcess_stop) {}
    if(file_isFileExist(OutputPath))
    {
        emit Send_TextBrowser_NewMessage(tr("Compatible with waifu2x-ncnn-vulkan(OLD Version): Yes"));
    }
    else
    {
        emit Send_TextBrowser_NewMessage(tr("Compatible with waifu2x-ncnn-vulkan(OLD Version): No. [Advice: Re-install gpu driver or update it to the latest.]"));
    }
    QFile::remove(OutputPath);
    //================
    Waifu2x_folder_path = Current_Path + "/waifu2x-converter";
    program = Waifu2x_folder_path + "/waifu2x-converter-cpp_waifu2xEX.exe";
    model_path= Waifu2x_folder_path + "/models_rgb";
    QString Denoise_cmd = " --noise-level 1 ";
    cmd = "\"" + program + "\"" + " -i " + "\"" + InputPath + "\"" + " -o " + "\"" + OutputPath + "\"" + " --scale-ratio 2" + Denoise_cmd + " --model-dir " + "\"" + model_path + "\"";
    QProcess *Waifu2x_converter = new QProcess();
    Waifu2x_converter->start(cmd);
    while(!Waifu2x_converter->waitForStarted(100)&&!QProcess_stop) {}
    while(!Waifu2x_converter->waitForFinished(100)&&!QProcess_stop) {}
    if(file_isFileExist(OutputPath))
    {
        emit Send_AutoDetectAlphaChannel_setChecked(true);
        emit Send_TextBrowser_NewMessage(tr("Compatible with waifu2x-converter: Yes."));
    }
    else
    {
        emit Send_AutoDetectAlphaChannel_setChecked(false);
        emit Send_TextBrowser_NewMessage(tr("Compatible with waifu2x-converter: No. [Advice: Buy a new computer.]"));
    }
    QFile::remove(OutputPath);
    //===============
    Current_Path = qApp->applicationDirPath();
    QString Anime4k_folder_path = Current_Path + "/Anime4K";
    program = Anime4k_folder_path + "/Anime4K.jar";
    cmd = "java -jar \"" + program + "\" \"" + InputPath + "\" \"" + OutputPath + "\" 2";
    QProcess *Waifu2x_anime4k = new QProcess();
    Waifu2x_anime4k->start(cmd);
    if(Waifu2x_anime4k->waitForStarted(60000))
    {
        while(!Waifu2x_anime4k->waitForFinished(100)&&!QProcess_stop) {}
    }
    if(file_isFileExist(OutputPath))
    {
        emit Send_TextBrowser_NewMessage(tr("Compatible with Anime4k: Yes."));
    }
    else
    {
        emit Send_TextBrowser_NewMessage(tr("Compatible with Anime4k: No. [Advice: Install the latest JDK and JRE.]"));
    }
    QFile::remove(OutputPath);
    //================
    Waifu2x_folder_path = Current_Path + "/srmd-ncnn-vulkan";
    program = Waifu2x_folder_path + "/srmd-ncnn-vulkan_waifu2xEX.exe";
    model_path = Waifu2x_folder_path+"/models-srmd";
    QProcess *SRMD_NCNN_VULKAN = new QProcess();
    cmd = "\"" + program + "\"" + " -i " + "\"" + InputPath + "\"" + " -o " + "\"" + OutputPath + "\"" + " -s 2 -n 0 -t 50 -m " + "\"" + model_path + "\"" + " -j 1:1:1";
    SRMD_NCNN_VULKAN->start(cmd);
    while(!SRMD_NCNN_VULKAN->waitForStarted(100)&&!QProcess_stop) {}
    while(!SRMD_NCNN_VULKAN->waitForFinished(100)&&!QProcess_stop) {}
    if(file_isFileExist(OutputPath))
    {
        emit Send_TextBrowser_NewMessage(tr("Compatible with srmd-ncnn-vulkan: Yes"));
    }
    else
    {
        emit Send_TextBrowser_NewMessage(tr("Compatible with srmd-ncnn-vulkan: No. [Advice: Re-install gpu driver or update it to the latest.]"));
    }
    QFile::remove(OutputPath);
    //================
    emit Send_TextBrowser_NewMessage(tr("Compatibility test is complete!"));
    emit Send_TextBrowser_NewMessage(tr("Tip: If one of these engines : [waifu2x-ncnn-vulkan, waifu2x-converter, srmd-ncnn-vulkan] is compatible with your computer, then you can use all functions in this software."));
    emit Send_Waifu2x_Compatibility_Test_finished();
    return 0;
}

int MainWindow::Waifu2x_Compatibility_Test_finished()
{
    ui->pushButton_Start->setEnabled(1);
    ui->pushButton_compatibilityTest->setEnabled(1);
    ui->pushButton_DetectGPU->setEnabled(1);
    ui->pushButton_DetectGPUID_srmd->setEnabled(1);
    ui->pushButton_DumpProcessorList_converter->setEnabled(1);
    return 0;
}

/*
判断图片是否含有透明通道
*/
bool MainWindow::Imgae_hasAlphaChannel(int rowNum)
{
    /*
    ======== 如果没开启检测, 直接返回false =============
    */
    if(ui->checkBox_AutoDetectAlphaChannel->checkState()==false)return false;
    //======
    QString SourceFile_fullPath = Table_model_image->item(rowNum,2)->text();
    QImage img(SourceFile_fullPath);
    return img.hasAlphaChannel();
}

