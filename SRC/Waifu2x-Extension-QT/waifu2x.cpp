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
    Delay_msec_sleep(1000);
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
            ThreadNumMax = 1;//获取gif线程数量最大值
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
            ThreadNumMax = 1;//获取video线程数量最大值
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
    ui->checkBox_ProcessVideoBySegment->setEnabled(1);
    if(ui->checkBox_ProcessVideoBySegment->checkState())
    {
        ui->spinBox_SegmentDuration->setEnabled(1);
    }
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
        emit Send_TextBrowser_NewMessage(tr("Compatible with waifu2x-ncnn-vulkan(New Version): Yes"));
        isCompatible_Waifu2x_NCNN_Vulkan_NEW=true;
    }
    else
    {
        emit Send_TextBrowser_NewMessage(tr("Compatible with waifu2x-ncnn-vulkan(New Version): No. [Advice: Re-install gpu driver or update it to the latest.]"));
        isCompatible_Waifu2x_NCNN_Vulkan_NEW=false;
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
        emit Send_TextBrowser_NewMessage(tr("Compatible with waifu2x-ncnn-vulkan(Old Version): Yes"));
        isCompatible_Waifu2x_NCNN_Vulkan_OLD=true;
    }
    else
    {
        emit Send_TextBrowser_NewMessage(tr("Compatible with waifu2x-ncnn-vulkan(Old Version): No. [Advice: Re-install gpu driver or update it to the latest.]"));
        isCompatible_Waifu2x_NCNN_Vulkan_OLD=false;
    }
    QFile::remove(OutputPath);
    //================
    Waifu2x_folder_path = Current_Path + "/waifu2x-ncnn-vulkan";
    program = Waifu2x_folder_path + "/waifu2x-ncnn-vulkan-fp16p_waifu2xEX.exe";
    model_path = Waifu2x_folder_path+"/models-upconv_7_anime_style_art_rgb";
    QProcess *Waifu2x_vulkan_fp16p = new QProcess();
    cmd = "\"" + program + "\"" + " -i " + "\"" + InputPath + "\"" + " -o " + "\"" + OutputPath + "\"" + " -s 2 -n 0 -t 50 -m " + "\"" + model_path + "\"" + " -j 1:1:1";
    Waifu2x_vulkan_fp16p->start(cmd);
    while(!Waifu2x_vulkan_fp16p->waitForStarted(100)&&!QProcess_stop) {}
    while(!Waifu2x_vulkan_fp16p->waitForFinished(100)&&!QProcess_stop) {}
    if(file_isFileExist(OutputPath))
    {
        emit Send_TextBrowser_NewMessage(tr("Compatible with waifu2x-ncnn-vulkan(New Version(fp16p)): Yes"));
        isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P=true;
    }
    else
    {
        emit Send_TextBrowser_NewMessage(tr("Compatible with waifu2x-ncnn-vulkan(New Version(fp16p)): No. [Advice: Re-install gpu driver or update it to the latest.]"));
        isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P=false;
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
        isCompatible_Waifu2x_Converter=true;
    }
    else
    {
        emit Send_AutoDetectAlphaChannel_setChecked(false);
        emit Send_TextBrowser_NewMessage(tr("Compatible with waifu2x-converter: No."));
        isCompatible_Waifu2x_Converter=false;
    }
    QFile::remove(OutputPath);
    //===============
    Current_Path = qApp->applicationDirPath();
    QString Anime4k_folder_path = Current_Path + "/Anime4K";
    program = Anime4k_folder_path + "/Anime4K_waifu2xEX.exe";
    cmd = "\"" + program + "\" -i \"" + InputPath + "\" -o \"" + OutputPath + "\" -z 2";
    QProcess *Waifu2x_anime4k = new QProcess();
    Waifu2x_anime4k->start(cmd);
    if(Waifu2x_anime4k->waitForStarted(60000))
    {
        while(!Waifu2x_anime4k->waitForFinished(100)&&!QProcess_stop) {}
    }
    if(file_isFileExist(OutputPath))
    {
        emit Send_TextBrowser_NewMessage(tr("Compatible with Anime4k: Yes."));
        isCompatible_Anime4k_CPU=true;
    }
    else
    {
        emit Send_TextBrowser_NewMessage(tr("Compatible with Anime4k: No."));
        isCompatible_Anime4k_CPU=false;
    }
    QFile::remove(OutputPath);
    //===============
    Current_Path = qApp->applicationDirPath();
    program = Anime4k_folder_path + "/Anime4K_waifu2xEX.exe";
    cmd = "\"" + program + "\" -i \"" + InputPath + "\" -o \"" + OutputPath + "\" -z 2 -q";
    QProcess *Waifu2x_anime4k_gpu = new QProcess();
    Waifu2x_anime4k_gpu->start(cmd);
    if(Waifu2x_anime4k_gpu->waitForStarted(60000))
    {
        while(!Waifu2x_anime4k_gpu->waitForFinished(100)&&!QProcess_stop) {}
    }
    if(file_isFileExist(OutputPath))
    {
        emit Send_TextBrowser_NewMessage(tr("Compatible with Anime4k(GPU Mode): Yes."));
        isCompatible_Anime4k_GPU=true;
    }
    else
    {
        emit Send_TextBrowser_NewMessage(tr("Compatible with Anime4k(GPU Mode): No."));
        isCompatible_Anime4k_GPU=false;
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
        isCompatible_SRMD_NCNN_Vulkan=true;
    }
    else
    {
        emit Send_TextBrowser_NewMessage(tr("Compatible with srmd-ncnn-vulkan: No. [Advice: Re-install gpu driver or update it to the latest.]"));
        isCompatible_SRMD_NCNN_Vulkan=false;
    }
    QFile::remove(OutputPath);
    //================
    QString PythonExt_ProgramPath = Current_Path + "/python_ext_waifu2xEX.exe";
    QString PythonExt_VideoFilePath = Current_Path + "/Compatibility_Test/CompatibilityTest_Video.mp4";
    QProcess PythonExt_QProcess;
    PythonExt_QProcess.start("\""+PythonExt_ProgramPath+"\" \""+PythonExt_VideoFilePath+"\" fps");
    while(!PythonExt_QProcess.waitForStarted(100)&&!QProcess_stop) {}
    while(!PythonExt_QProcess.waitForFinished(100)&&!QProcess_stop) {}
    QString PythonExt_fps=PythonExt_QProcess.readAllStandardOutput().trimmed();
    if(PythonExt_fps!="0.0")
    {
        emit Send_TextBrowser_NewMessage(tr("Compatible with Python extension: Yes."));
        isCompatible_PythonExtension=true;
    }
    else
    {
        emit Send_TextBrowser_NewMessage(tr("Compatible with Python extension: No."));
        isCompatible_PythonExtension=false;
    }
    //================
    QString ffmpeg_VideoPath = Current_Path + "/Compatibility_Test/CompatibilityTest_Video.mp4";
    QString ffmpeg_AudioPath = Current_Path + "/Compatibility_Test/CompatibilityTest_Video_audio.wav";
    QString ffmpeg_path = Current_Path+"/ffmpeg_waifu2xEX.exe";
    QFile::remove(ffmpeg_AudioPath);
    QProcess ffmpeg_QProcess;
    ffmpeg_QProcess.start("\""+ffmpeg_path+"\" -y -i \""+ffmpeg_VideoPath+"\" \""+ffmpeg_AudioPath+"\"");
    while(!ffmpeg_QProcess.waitForStarted(100)&&!QProcess_stop) {}
    while(!ffmpeg_QProcess.waitForFinished(100)&&!QProcess_stop) {}
    if(file_isFileExist(ffmpeg_AudioPath))
    {
        emit Send_TextBrowser_NewMessage(tr("Compatible with FFmpeg: Yes."));
        isCompatible_FFmpeg=true;
    }
    else
    {
        emit Send_TextBrowser_NewMessage(tr("Compatible with FFmpeg: No."));
        isCompatible_FFmpeg=false;
    }
    QFile::remove(ffmpeg_AudioPath);
    //================
    QString FFprobe_VideoPath = Current_Path + "/Compatibility_Test/CompatibilityTest_Video.mp4";
    int FFprobe_duration = video_get_duration(FFprobe_VideoPath);
    if(FFprobe_duration>0)
    {
        emit Send_TextBrowser_NewMessage(tr("Compatible with FFprobe: Yes."));
        isCompatible_FFprobe=true;
    }
    else
    {
        emit Send_TextBrowser_NewMessage(tr("Compatible with FFprobe: No."));
        isCompatible_FFprobe=false;
    }
    //================
    QString convert_InputPath = Current_Path + "/Compatibility_Test/Compatibility_Test.jpg";
    QString convert_OutputPath = Current_Path + "/Compatibility_Test/convert_res.bmp";
    QString convert_program = Current_Path+"/convert_waifu2xEX.exe";
    QFile::remove(convert_OutputPath);
    QProcess convert_QProcess;
    convert_QProcess.start("\""+convert_program+"\" \""+convert_InputPath+"\" \""+convert_OutputPath+"\"");
    while(!convert_QProcess.waitForStarted(100)&&!QProcess_stop) {}
    while(!convert_QProcess.waitForFinished(100)&&!QProcess_stop) {}
    if(file_isFileExist(convert_OutputPath))
    {
        emit Send_TextBrowser_NewMessage(tr("Compatible with ImageMagick: Yes."));
        isCompatible_ImageMagick=true;
    }
    else
    {
        emit Send_TextBrowser_NewMessage(tr("Compatible with ImageMagick: No."));
        isCompatible_ImageMagick=false;
    }
    QFile::remove(convert_OutputPath);
    //================
    QString Gifsicle_InputPath = Current_Path + "/Compatibility_Test/CompatibilityTest_GIF.gif";
    QString Gifsicle_OutputPath = Current_Path + "/Compatibility_Test/CompatibilityTest_GIF_RES.gif";
    QFile::remove(Gifsicle_OutputPath);
    Gif_compressGif(Gifsicle_InputPath,Gifsicle_OutputPath);
    if(file_isFileExist(Gifsicle_OutputPath))
    {
        emit Send_TextBrowser_NewMessage(tr("Compatible with Gifsicle: Yes."));
        isCompatible_Gifsicle=true;
    }
    else
    {
        emit Send_TextBrowser_NewMessage(tr("Compatible with Gifsicle: No."));
        isCompatible_Gifsicle=false;
    }
    QFile::remove(Gifsicle_OutputPath);
    //================
    QString SoX_InputPath = Current_Path + "/Compatibility_Test/CompatibilityTest_Sound.wav";
    QString SoX_OutputPath = Current_Path + "/Compatibility_Test/CompatibilityTest_Sound_Denoised.wav";
    QFile::remove(SoX_OutputPath);
    video_AudioDenoise(SoX_InputPath);
    if(file_isFileExist(SoX_OutputPath))
    {
        emit Send_TextBrowser_NewMessage(tr("Compatible with SoX: Yes."));
        isCompatible_SoX=true;
    }
    else
    {
        emit Send_TextBrowser_NewMessage(tr("Compatible with SoX: No."));
        isCompatible_SoX=false;
    }
    QFile::remove(SoX_OutputPath);
    //================
    emit Send_TextBrowser_NewMessage(tr("Compatibility test is complete!"));
    emit Send_Waifu2x_Compatibility_Test_finished();
    return 0;
}

int MainWindow::Waifu2x_Compatibility_Test_finished()
{
    //更改checkbox状态以显示测试结果
    ui->checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW->setChecked(isCompatible_Waifu2x_NCNN_Vulkan_NEW);
    ui->checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P->setChecked(isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P);
    ui->checkBox_isCompatible_Waifu2x_NCNN_Vulkan_OLD->setChecked(isCompatible_Waifu2x_NCNN_Vulkan_OLD);
    ui->checkBox_isCompatible_Waifu2x_Converter->setChecked(isCompatible_Waifu2x_Converter);
    ui->checkBox_isCompatible_SRMD_NCNN_Vulkan->setChecked(isCompatible_SRMD_NCNN_Vulkan);
    ui->checkBox_isCompatible_Anime4k_CPU->setChecked(isCompatible_Anime4k_CPU);
    ui->checkBox_isCompatible_Anime4k_GPU->setChecked(isCompatible_Anime4k_GPU);
    ui->checkBox_isCompatible_PythonExtension->setChecked(isCompatible_PythonExtension);
    ui->checkBox_isCompatible_FFmpeg->setChecked(isCompatible_FFmpeg);
    ui->checkBox_isCompatible_FFprobe->setChecked(isCompatible_FFprobe);
    ui->checkBox_isCompatible_ImageMagick->setChecked(isCompatible_ImageMagick);
    ui->checkBox_isCompatible_Gifsicle->setChecked(isCompatible_Gifsicle);
    ui->checkBox_isCompatible_SoX->setChecked(isCompatible_SoX);
    //解除界面管制
    ui->tab_Home->setEnabled(1);
    ui->tab_EngineSettings->setEnabled(1);
    ui->tab_VideoSettings->setEnabled(1);
    ui->tab_AdditionalSettings->setEnabled(1);
    ui->pushButton_compatibilityTest->setEnabled(1);
    ui->pushButton_compatibilityTest->setText(tr("Compatibility test"));
    ui->tabWidget->setCurrentIndex(5);
    //提醒用户检查测试结果
    QMessageBox *MSG = new QMessageBox();
    MSG->setWindowTitle(tr("Notification"));
    MSG->setText(tr("The compatibility test has been completed. Please check the test results and configure the engine settings based on the test results."));
    MSG->setIcon(QMessageBox::Information);
    MSG->setModal(true);
    MSG->show();
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

