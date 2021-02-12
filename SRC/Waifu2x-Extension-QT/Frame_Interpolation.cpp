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
bool MainWindow::FrameInterpolation(QString SourcePath,QString OutputPath,int FrameNumDigits)
{
    if(ui->groupBox_FrameInterpolation->isChecked()==false)return false;
    FrameNumDigits++;
    //==========
    if(SourcePath.right(1)=="/")
    {
        SourcePath = SourcePath.left(SourcePath.length() - 1);
    }
    if(OutputPath.right(1)=="/")
    {
        OutputPath = OutputPath.left(OutputPath.length() - 1);
    }
    if(file_isDirExist(SourcePath)==false)return false;
    file_DelDir(OutputPath);
    file_mkDir(OutputPath);
    //=======
    emit Send_TextBrowser_NewMessage(tr("Starting to interpolate frames in:[")+SourcePath+"]");
    //=======
    if(ui->checkBox_AutoAdjustNumOfThreads_VFI->isChecked()==true)
    {
        if(ui->spinBox_retry->value()<5)ui->spinBox_retry->setValue(6);
    }
    //========
    int FileNum_MAX = file_getFileNames_in_Folder_nofilter(SourcePath).size()*2;
    int FileNum_New = 0;
    int FileNum_Old = 0;
    //========
    QString FrameInterpolation_ProgramPath="";
    switch (ui->comboBox_Engine_VFI->currentIndex())
    {
        case 0:
            {
                FrameInterpolation_ProgramPath = Current_Path+"/rife-ncnn-vulkan/rife-ncnn-vulkan_waifu2xEX.exe";
                break;
            }
        case 1:
            {
                FrameInterpolation_ProgramPath = Current_Path+"/cain-ncnn-vulkan/cain-ncnn-vulkan_waifu2xEX.exe";
                break;
            }
    }
    QString CMD ="\""+FrameInterpolation_ProgramPath+"\" -i \""+SourcePath.replace("%","%%")+"\" -o \""+OutputPath.replace("%","%%")+"\" -f %0"+QString("%1").arg(FrameNumDigits)+"d.png"+FrameInterpolation_ReadConfig();
    //========
    bool FrameInterpolation_QProcess_failed = false;
    QString ErrorMSG="";
    QString StanderMSG="";
    for(int retry=0; retry<ui->spinBox_retry->value(); retry++)
    {
        FrameInterpolation_QProcess_failed = false;
        ErrorMSG="";
        StanderMSG="";
        //=====
        QProcess FrameInterpolation_QProcess;
        FrameInterpolation_QProcess.start(CMD);
        while(!FrameInterpolation_QProcess.waitForStarted(200)&&!QProcess_stop) {}
        while(!FrameInterpolation_QProcess.waitForFinished(200)&&!QProcess_stop)
        {
            ErrorMSG = FrameInterpolation_QProcess.readAllStandardError().toLower();
            StanderMSG = FrameInterpolation_QProcess.readAllStandardOutput().toLower();
            if(ErrorMSG.contains("failed")||StanderMSG.contains("failed"))
            {
                FrameInterpolation_QProcess_failed = true;
                FrameInterpolation_QProcess.close();
                file_DelDir(OutputPath);
                break;
            }
            if(ui->checkBox_ShowInterPro->isChecked())
            {
                FileNum_New = file_getFileNames_in_Folder_nofilter(OutputPath).size();
                if(FileNum_New!=FileNum_Old)
                {
                    emit Send_TextBrowser_NewMessage(tr("Interpolating frames in:[")+SourcePath+tr("] Progress:[")+QString::number(FileNum_New,10)+"/"+QString::number(FileNum_MAX,10)+"]");
                    FileNum_Old=FileNum_New;
                }
            }
        }
        if(FrameInterpolation_QProcess_failed==false)
        {
            ErrorMSG = FrameInterpolation_QProcess.readAllStandardError().toLower();
            StanderMSG = FrameInterpolation_QProcess.readAllStandardOutput().toLower();
            if(ErrorMSG.contains("failed")||StanderMSG.contains("failed"))
            {
                FrameInterpolation_QProcess_failed = true;
                file_DelDir(OutputPath);
            }
        }
        //========= 检测是否成功,是否需要重试 ============
        if(FrameInterpolation_QProcess_failed==false && (file_getFileNames_in_Folder_nofilter(SourcePath).size()*2 == file_getFileNames_in_Folder_nofilter(OutputPath).size()))
        {
            emit Send_TextBrowser_NewMessage(tr("Finish interpolating frames in:[")+SourcePath+"]");
            //=======
            FrameInterpolation_QProcess.close();
            return true;
        }
        else
        {
            file_DelDir(OutputPath);
            FrameInterpolation_QProcess.close();
            if(retry==(ui->spinBox_retry->value()-1))
            {
                break;
            }
            if(retry>=2 && ui->checkBox_AutoAdjustNumOfThreads_VFI->isChecked()==true)
            {
                ui->spinBox_NumOfThreads_VFI->setValue(1);
            }
            file_mkDir(OutputPath);
            emit Send_TextBrowser_NewMessage(tr("Automatic retry, please wait."));
            Delay_sec_sleep(5);
        }
    }
    //=======
    emit Send_TextBrowser_NewMessage(tr("Failed to interpolate frames in:[")+SourcePath+"]");
    //=======
    return false;
}

QString MainWindow::FrameInterpolation_ReadConfig()
{
    QString VFI_Config = " ";
    if(ui->comboBox_Engine_VFI->currentIndex()==0)
    {
        //TTA
        if(ui->checkBox_TTA_VFI->isChecked())
        {
            VFI_Config.append("-x ");
        }
        //UHD
        if(ui->checkBox_UHD_VFI->isChecked())
        {
            VFI_Config.append("-u ");
        }
    }
    //GPU & 多线程
    if(ui->checkBox_MultiGPU_VFI->isChecked()==false)
    {
        //单显卡
        //GPU ID
        if(ui->comboBox_GPUID_VFI->currentText().trimmed().toLower()!="auto")
        {
            VFI_Config.append("-g "+ui->comboBox_GPUID_VFI->currentText().trimmed()+" ");
        }
        //线程数量
        QString jobs_num_str = QString("%1").arg(ui->spinBox_NumOfThreads_VFI->value());
        VFI_Config.append(QString("-j "+jobs_num_str+":"+jobs_num_str+":"+jobs_num_str+" "));
    }
    else
    {
        //多显卡
        //GPU ID
        QString GPU_IDs_str = ui->lineEdit_MultiGPU_IDs_VFI->text().trimmed().trimmed().replace("，",",").remove(" ").remove("　");
        if(GPU_IDs_str.right(1)==",")
        {
            GPU_IDs_str = GPU_IDs_str.left(GPU_IDs_str.length() - 1);
        }
        QStringList GPU_IDs_StrList = GPU_IDs_str.split(",");
        GPU_IDs_StrList.removeAll("");
        GPU_IDs_StrList.removeDuplicates();
        if(GPU_IDs_StrList.isEmpty())
        {
            QString jobs_num_str = QString("%1").arg(ui->spinBox_NumOfThreads_VFI->value());
            VFI_Config.append(QString("-j "+jobs_num_str+":"+jobs_num_str+":"+jobs_num_str+" "));
        }
        else
        {
            //===
            GPU_IDs_str = "";
            for(int i=0; i<GPU_IDs_StrList.size(); i++)
            {
                if(i==0)
                {
                    GPU_IDs_str.append(GPU_IDs_StrList.at(i));
                }
                else
                {
                    GPU_IDs_str.append(","+GPU_IDs_StrList.at(i));
                }
            }
            VFI_Config.append("-g "+GPU_IDs_str+" ");
            //线程数量
            int NumOfThreads_AVG = ui->spinBox_NumOfThreads_VFI->value() / GPU_IDs_StrList.size();
            if(NumOfThreads_AVG<1)NumOfThreads_AVG=1;
            int NumOfThreads_Total = NumOfThreads_AVG * GPU_IDs_StrList.size();
            //===
            QString NumOfThreads_AVG_str = QString("%1").arg(NumOfThreads_AVG);
            QString NumOfThreads_Total_str = QString("%1").arg(NumOfThreads_Total);
            //===
            QString Jobs_Str = "";
            for(int i=0; i<GPU_IDs_StrList.size(); i++)
            {
                if(i==0)
                {
                    Jobs_Str.append(NumOfThreads_AVG_str);
                }
                else
                {
                    Jobs_Str.append(","+NumOfThreads_AVG_str);
                }
            }
            //===
            VFI_Config.append(QString("-j "+NumOfThreads_Total_str+":"+Jobs_Str+":"+NumOfThreads_Total_str+" "));
        }
    }
    //模型
    switch (ui->comboBox_Engine_VFI->currentIndex())
    {
        case 0:
            {
                VFI_Config.append("-m \""+Current_Path+"/rife-ncnn-vulkan/"+ui->comboBox_Model_VFI->currentText().trimmed()+"\" ");
                break;
            }
        case 1:
            {
                VFI_Config.append("-m \""+Current_Path+"/cain-ncnn-vulkan/cain\" ");
                break;
            }
    }
    //========================
    return VFI_Config;
}

/*
================================================================================
                    REFI NCNN VULKAN 检测可用GPU
=================================================================================
*/

void MainWindow::on_pushButton_DetectGPU_VFI_clicked()
{
    //====
    ui->pushButton_DetectGPU_VFI->setText(tr("Detecting, please wait..."));
    //====
    ui->pushButton_Start->setEnabled(0);
    ui->comboBox_GPUID_VFI->setEnabled(0);
    ui->pushButton_DetectGPU_VFI->setEnabled(0);
    Available_GPUID_FrameInterpolation.clear();
    QtConcurrent::run(this, &MainWindow::FrameInterpolation_DetectGPU);
}

int MainWindow::FrameInterpolation_DetectGPU()
{
    emit Send_TextBrowser_NewMessage(tr("Detecting available GPU, please wait."));
    //===============
    QString InputPath = Current_Path + "/Compatibility_Test/Compatibility_Test.jpg";
    QString InputPath_1 = Current_Path + "/Compatibility_Test/Compatibility_Test_1.jpg";
    QString OutputPath = Current_Path + "/Compatibility_Test/res.png";
    QFile::remove(OutputPath);
    //==============
    QString FrameInterpolation_ProgramPath = "";
    QString FrameInterpolation_ModelPath = "";
    switch (ui->comboBox_Engine_VFI->currentIndex())
    {
        case 0:
            {
                FrameInterpolation_ProgramPath = Current_Path+"/rife-ncnn-vulkan/rife-ncnn-vulkan_waifu2xEX.exe";
                FrameInterpolation_ModelPath = Current_Path+"/rife-ncnn-vulkan/rife-HD";
                break;
            }
        case 1:
            {
                FrameInterpolation_ProgramPath = Current_Path+"/cain-ncnn-vulkan/cain-ncnn-vulkan_waifu2xEX.exe";
                FrameInterpolation_ModelPath = Current_Path+"/cain-ncnn-vulkan/cain";
                break;
            }
    }
    //=========
    int GPU_ID=-1;
    //=========
    while(true)
    {
        QFile::remove(OutputPath);
        QProcess *Waifu2x = new QProcess();
        QString gpu_str = " -g "+QString::number(GPU_ID,10)+" ";
        QString cmd = "\"" + FrameInterpolation_ProgramPath + "\"" + " -0 " + "\"" + InputPath + "\"" + " -1 " + "\"" + InputPath_1 + "\" -o " + "\"" + OutputPath + "\"" + " -j 1:1:1 " + gpu_str + " -m \""+FrameInterpolation_ModelPath+"\"";
        Waifu2x->start(cmd);
        while(!Waifu2x->waitForStarted(100)&&!QProcess_stop) {}
        while(!Waifu2x->waitForFinished(100)&&!QProcess_stop) {}
        if(QFile::exists(OutputPath))
        {
            Available_GPUID_FrameInterpolation.append(QString::number(GPU_ID,10));
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
    if(Available_GPUID_FrameInterpolation.isEmpty())
    {
        Send_TextBrowser_NewMessage(tr("No available GPU ID detected!"));
    }
    emit Send_FrameInterpolation_DetectGPU_finished();
    return 0;
}

int MainWindow::FrameInterpolation_DetectGPU_finished()
{
    ui->pushButton_Start->setEnabled(1);
    ui->pushButton_DetectGPU_VFI->setEnabled(1);
    if(ui->checkBox_MultiGPU_VFI->isChecked()==false)ui->comboBox_GPUID_VFI->setEnabled(1);
    //====
    ui->comboBox_GPUID_VFI->clear();
    ui->comboBox_GPUID_VFI->addItem("auto");
    if(!Available_GPUID_FrameInterpolation.isEmpty())
    {
        QString AvaIDs_Str=tr("Available GPU IDs for current Frame Interpolation engine:[");
        for(int i=0; i<Available_GPUID_FrameInterpolation.size(); i++)
        {
            ui->comboBox_GPUID_VFI->addItem(Available_GPUID_FrameInterpolation.at(i));
            if(i == (Available_GPUID_FrameInterpolation.size()-1))
            {
                AvaIDs_Str.append(Available_GPUID_FrameInterpolation.at(i));
            }
            else
            {
                AvaIDs_Str.append(Available_GPUID_FrameInterpolation.at(i)+",");
            }
        }
        AvaIDs_Str.append("]");
        emit Send_TextBrowser_NewMessage(AvaIDs_Str);
    }
    //====
    ui->pushButton_DetectGPU_VFI->setText(tr("Detect available GPU ID"));
    //====
    return 0;
}

void MainWindow::on_lineEdit_MultiGPU_IDs_VFI_editingFinished()
{
    QString TMP_str = ui->lineEdit_MultiGPU_IDs_VFI->text();
    TMP_str = TMP_str.trimmed().replace("，",",").remove(" ").remove("　");
    ui->lineEdit_MultiGPU_IDs_VFI->setText(TMP_str);
}

void MainWindow::on_checkBox_MultiGPU_VFI_stateChanged(int arg1)
{
    bool tmp_bool = ui->checkBox_MultiGPU_VFI->isChecked();
    ui->comboBox_GPUID_VFI->setEnabled(!tmp_bool);
    ui->lineEdit_MultiGPU_IDs_VFI->setEnabled(tmp_bool);
}

void MainWindow::on_groupBox_FrameInterpolation_clicked()
{
    ui->frame_FrameInterpolation->setEnabled(ui->groupBox_FrameInterpolation->isChecked());
    on_comboBox_Engine_VFI_currentIndexChanged(0);
    on_checkBox_MultiGPU_VFI_stateChanged(0);
}

void MainWindow::on_checkBox_isCompatible_RifeNcnnVulkan_clicked()
{
    ui->checkBox_isCompatible_RifeNcnnVulkan->setChecked(isCompatible_RifeNcnnVulkan);
}

void MainWindow::on_checkBox_isCompatible_CainNcnnVulkan_clicked()
{
    ui->checkBox_isCompatible_CainNcnnVulkan->setChecked(isCompatible_CainNcnnVulkan);
}

void MainWindow::on_comboBox_Engine_VFI_currentIndexChanged(int index)
{
    if(Old_FrameInterpolation_Engine_Index!=ui->comboBox_Engine_VFI->currentIndex())
    {
        Available_GPUID_FrameInterpolation.clear();
        FrameInterpolation_DetectGPU_finished();
        ui->lineEdit_MultiGPU_IDs_VFI->setText("");
    }
    if(ui->comboBox_Engine_VFI->currentIndex()==0)
    {
        ui->checkBox_TTA_VFI->setEnabled(1);
        ui->checkBox_UHD_VFI->setEnabled(1);
        ui->comboBox_Model_VFI->setEnabled(1);
    }
    if(ui->comboBox_Engine_VFI->currentIndex()==1)
    {
        ui->checkBox_TTA_VFI->setEnabled(0);
        ui->checkBox_UHD_VFI->setEnabled(0);
        ui->checkBox_TTA_VFI->setChecked(0);
        ui->checkBox_UHD_VFI->setChecked(0);
        ui->comboBox_Model_VFI->setEnabled(0);
    }
    Old_FrameInterpolation_Engine_Index = ui->comboBox_Engine_VFI->currentIndex();
}

