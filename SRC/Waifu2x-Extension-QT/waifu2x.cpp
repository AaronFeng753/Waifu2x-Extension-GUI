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
    QtConcurrent::run(this, &MainWindow::Table_ChangeAllStatusToWaiting);
    Progressbar_MaxVal = FileList_image.count() + FileList_gif.count() + FileList_video.count();
    Progressbar_CurrentVal = 0;
    TaskNumFinished=0;
    emit Send_PrograssBar_Range_min_max(0, Progressbar_MaxVal);
    if(!FileList_image.isEmpty())
    {
        int ImageEngine = ui->comboBox_Engine_Image->currentIndex();
        QList<QMap<QString, QString>> FileList_image_tmp = FileList_image;
        for ( int i = 0; i != FileList_image_tmp.size(); ++i )
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
            QMap<QString, QString> File_map = FileList_image_tmp.at(i);
            ThreadNumMax = ui->spinBox_ThreadNum_image->value();//获取image线程数量最大值
            switch(ImageEngine)
            {
                case 0:
                    {
                        ThreadNumRunning++;//线程数量统计+1s
                        QtConcurrent::run(this, &MainWindow::Waifu2x_NCNN_Vulkan_Image, File_map);
                        while (ThreadNumRunning >= ThreadNumMax)
                        {
                            Delay_msec_sleep(500);
                        }
                        break;
                    }
                case 1:
                    {
                        ThreadNumRunning++;//线程数量统计+1s
                        QtConcurrent::run(this, &MainWindow::Waifu2x_Converter_Image, File_map);
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
    if(!FileList_gif.isEmpty())
    {
        int GIFEngine = ui->comboBox_Engine_GIF->currentIndex();
        QList<QMap<QString, QString>> FileList_gif_tmp = FileList_gif;
        for ( int i = 0; i != FileList_gif_tmp.size(); ++i )
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
            QMap<QString, QString> File_map = FileList_gif_tmp.at(i);
            ThreadNumMax = ui->spinBox_ThreadNum_gif->value();//获取gif线程数量最大值
            switch(GIFEngine)
            {
                case 0:
                    {
                        ThreadNumRunning++;//线程数量统计+1s
                        QtConcurrent::run(this, &MainWindow::Waifu2x_NCNN_Vulkan_GIF, File_map);
                        while (ThreadNumRunning >= ThreadNumMax)
                        {
                            Delay_msec_sleep(500);
                        }
                        break;
                    }
                case 1:
                    {
                        ThreadNumRunning++;//线程数量统计+1s
                        QtConcurrent::run(this, &MainWindow::Waifu2x_Converter_GIF, File_map);
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
    if(!FileList_video.isEmpty())
    {
        int VideoEngine = ui->comboBox_Engine_Video->currentIndex();
        QList<QMap<QString, QString>> FileList_video_tmp = FileList_video;
        for ( int i = 0; i != FileList_video_tmp.size(); ++i )
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
            QMap<QString, QString> File_map = FileList_video_tmp.at(i);
            ThreadNumMax = ui->spinBox_ThreadNum_video->value();//获取video线程数量最大值
            switch(VideoEngine)
            {
                case 0:
                    {
                        ThreadNumRunning++;//线程数量统计+1s
                        QtConcurrent::run(this, &MainWindow::Waifu2x_NCNN_Vulkan_Video, File_map);
                        while (ThreadNumRunning >= ThreadNumMax)
                        {
                            Delay_msec_sleep(500);
                        }
                        break;
                    }
                case 1:
                    {
                        ThreadNumRunning++;//线程数量统计+1s
                        QtConcurrent::run(this, &MainWindow::Waifu2x_Converter_Video, File_map);
                        while (ThreadNumRunning >= ThreadNumMax)
                        {
                            Delay_msec_sleep(500);
                        }
                        break;
                    }
                case 2:
                    {
                        ThreadNumRunning++;//线程数量统计+1s
                        QtConcurrent::run(this, &MainWindow::Anime4k_Video, File_map);
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
    ui->pushButton_SaveFileList->setEnabled(1);
    ui->comboBox_AspectRatio_custRes->setEnabled(1);
    ui->spinBox_JPGCompressedQuality->setEnabled(1);
    ui->groupBox_video_settings->setEnabled(1);
    if(ui->checkBox_DelOriginal->checkState())ui->checkBox_Move2RecycleBin->setEnabled(1);
    ui->pushButton_ForceRetry->setEnabled(0);
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
    QString program = Waifu2x_folder_path + "/waifu2x-ncnn-vulkan.exe";
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
    program = Waifu2x_folder_path + "/waifu2x-ncnn-vulkan.exe";
    model_path = Waifu2x_folder_path+"/models-upconv_7_anime_style_art_rgb";
    QProcess *Waifu2x_vulkan_old = new QProcess();
    cmd = "\"" + program + "\"" + " -i " + "\"" + InputPath + "\"" + " -o " + "\"" + OutputPath + "\"" + " -s 2 -n 0 -t 50 -m " + "\"" + model_path + "\"" + " -j 1:1:1";
    Waifu2x_vulkan_old->start(cmd);
    while(!Waifu2x_vulkan_old->waitForStarted(100)&&!QProcess_stop) {}
    while(!Waifu2x_vulkan_old->waitForFinished(100)&&!QProcess_stop) {}
    if(file_isFileExist(OutputPath))
    {
        emit Send_TextBrowser_NewMessage(tr("Compatible with waifu2x-ncnn-vulkan(OLD Verison): Yes"));
    }
    else
    {
        emit Send_TextBrowser_NewMessage(tr("Compatible with waifu2x-ncnn-vulkan(OLD Verison): No. [Advice: Re-install gpu driver or update it to the latest.]"));
    }
    QFile::remove(OutputPath);
    //================
    Waifu2x_folder_path = Current_Path + "/waifu2x-converter";
    program = Waifu2x_folder_path + "/waifu2x-converter-cpp.exe";
    model_path= Waifu2x_folder_path + "/models_rgb";
    QString Denoise_cmd = " --noise-level 1 ";
    cmd = "\"" + program + "\"" + " -i " + "\"" + InputPath + "\"" + " -o " + "\"" + OutputPath + "\"" + " --scale-ratio 2" + Denoise_cmd + " --model-dir " + "\"" + model_path + "\"";
    QProcess *Waifu2x_converter = new QProcess();
    Waifu2x_converter->start(cmd);
    while(!Waifu2x_converter->waitForStarted(100)&&!QProcess_stop) {}
    while(!Waifu2x_converter->waitForFinished(100)&&!QProcess_stop) {}
    if(file_isFileExist(OutputPath))
    {
        emit Send_TextBrowser_NewMessage(tr("Compatible with waifu2x-converter: Yes."));
    }
    else
    {
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
    if(Waifu2x_anime4k->waitForStarted(-1))
    {
        while(!Waifu2x_anime4k->waitForFinished(100)&&!QProcess_stop) {}
    }
    //while(!Waifu2x_anime4k->waitForStarted(100)&&!QProcess_stop) {}
    if(file_isFileExist(OutputPath))
    {
        emit Send_TextBrowser_NewMessage(tr("Compatible with Anime4k: Yes."));
    }
    else
    {
        emit Send_TextBrowser_NewMessage(tr("Compatible with Anime4k: No. [Advice: Install the latest JDK and JRE.]"));
    }
    QFile::remove(OutputPath);
    //===============
    emit Send_TextBrowser_NewMessage(tr("Compatibility test is complete!"));
    emit Send_Waifu2x_Compatibility_Test_finished();
    return 0;
}

int MainWindow::Waifu2x_Compatibility_Test_finished()
{
    ui->pushButton_Start->setEnabled(1);
    ui->pushButton_compatibilityTest->setEnabled(1);
    ui->pushButton_DetectGPU->setEnabled(1);
    ui->pushButton_DumpProcessorList_converter->setEnabled(1);
    return 0;
}

int MainWindow::Waifu2x_DetectGPU()
{
    emit Send_TextBrowser_NewMessage(tr("Detecting available GPU, please wait."));
    //===============
    QString InputPath = Current_Path + "/Compatibility_Test/Compatibility_Test.jpg";
    QString OutputPath = Current_Path + "/Compatibility_Test/res.jpg";
    QFile::remove(OutputPath);
    //==============
    QString Waifu2x_folder_path = Current_Path + "/waifu2x-ncnn-vulkan";
    QString program = Waifu2x_folder_path + "/waifu2x-ncnn-vulkan.exe";
    QString model_path = Waifu2x_folder_path+"/models-upconv_7_anime_style_art_rgb";
    //=========
    int GPU_ID=0;
    //=========
    while(true)
    {
        QFile::remove(OutputPath);
        QProcess *Waifu2x = new QProcess();
        QString gpu_str = " -g "+QString::number(GPU_ID,10)+" ";
        QString cmd = "\"" + program + "\"" + " -i " + "\"" + InputPath + "\"" + " -o " + "\"" + OutputPath + "\"" + " -s 2 -n 0 -t 50 -m " + "\"" + model_path + "\"" + " -j 1:1:1"+gpu_str;
        Waifu2x->start(cmd);
        while(!Waifu2x->waitForStarted(100)&&!QProcess_stop) {}
        while(!Waifu2x->waitForFinished(100)&&!QProcess_stop) {}
        if(file_isFileExist(OutputPath))
        {
            Available_GPUID.append(QString::number(GPU_ID,10));
            GPU_ID++;
            QFile::remove(OutputPath);
        }
        else
        {
            break;
        }
    }
    QFile::remove(OutputPath);
    //===============
    emit Send_TextBrowser_NewMessage(tr("Detection is complete!"));
    if(Available_GPUID.isEmpty())
    {
        Send_TextBrowser_NewMessage(tr("No available GPU ID detected!"));
    }
    emit Send_Waifu2x_DetectGPU_finished();
    return 0;
}

int MainWindow::Waifu2x_DetectGPU_finished()
{
    ui->pushButton_Start->setEnabled(1);
    ui->pushButton_DetectGPU->setEnabled(1);
    ui->pushButton_compatibilityTest->setEnabled(1);
    ui->pushButton_DumpProcessorList_converter->setEnabled(1);
    //====
    ui->comboBox_GPUID->clear();
    ui->comboBox_GPUID->addItem("auto");
    if(!Available_GPUID.isEmpty())
    {
        for(int i=0; i<Available_GPUID.size(); i++)
        {
            ui->comboBox_GPUID->addItem(Available_GPUID.at(i));
        }
    }
    return 0;
}


