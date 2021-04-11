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
多线程调整文件夹内图片大小
*/
void MainWindow::ImagesResize_Folder_MultiThread(int New_width,int New_height,QString ImagesFolderPath)
{
    if(file_isDirExist(ImagesFolderPath)==false)return;
    //====
    QMutex_ResizeImage_MultiThread.lock();
    //=====
    TotalNumOfThreads_ImagesResize_Folder_MultiThread = QThread::idealThreadCount()/2;
    if(TotalNumOfThreads_ImagesResize_Folder_MultiThread>32)TotalNumOfThreads_ImagesResize_Folder_MultiThread=32;
    if(TotalNumOfThreads_ImagesResize_Folder_MultiThread<1)TotalNumOfThreads_ImagesResize_Folder_MultiThread=1;
    RunningNumOfThreads_ImagesResize_Folder_MultiThread=0;
    //=====
    QMutex_ResizeImage_MultiThread.unlock();
    //================
    QStringList Frames_QStringList = file_getFileNames_in_Folder_nofilter(ImagesFolderPath);
    if(Frames_QStringList.isEmpty())return;
    for(int i=0; i<Frames_QStringList.size(); i++)
    {
        QString OutPut_Path = ImagesFolderPath+"/"+Frames_QStringList.at(i);
        //====
        QMutex_ResizeImage_MultiThread.lock();
        RunningNumOfThreads_ImagesResize_Folder_MultiThread++;
        QMutex_ResizeImage_MultiThread.unlock();
        //====
        QtConcurrent::run(this,&MainWindow::ResizeImage_MultiThread,New_width,New_height,OutPut_Path);
        while(RunningNumOfThreads_ImagesResize_Folder_MultiThread>=TotalNumOfThreads_ImagesResize_Folder_MultiThread)
        {
            Delay_msec_sleep(300);
        }
    }
    while(RunningNumOfThreads_ImagesResize_Folder_MultiThread>0)
    {
        Delay_msec_sleep(300);
    }
    return;
}

void MainWindow::ResizeImage_MultiThread(int New_width,int New_height,QString ImagesPath)
{
    QImage qimage_adj(ImagesPath);
    QImage qimage_adj_scaled = qimage_adj.scaled(New_width,New_height,CustRes_AspectRatioMode,Qt::SmoothTransformation);
    QImageWriter qimageW_adj;
    qimageW_adj.setFormat("png");
    qimageW_adj.setFileName(ImagesPath);
    if(qimageW_adj.canWrite())
    {
        qimageW_adj.write(qimage_adj_scaled);
    }
    QMutex_ResizeImage_MultiThread.lock();
    RunningNumOfThreads_ImagesResize_Folder_MultiThread--;
    QMutex_ResizeImage_MultiThread.unlock();
    return;
}

/*
根据分辨率判断是否跳过
true = 跳过
*/
bool MainWindow::Image_Gif_AutoSkip_CustRes(int rowNum,bool isGif)
{
    if(ui->checkBox_AutoSkip_CustomRes->isChecked()==false)return false;
    QString SourceFile_fullPath = "";
    if(isGif)
    {
        SourceFile_fullPath = Table_model_gif->item(rowNum,2)->text();
    }
    else
    {
        SourceFile_fullPath = Table_model_image->item(rowNum,2)->text();
    }
    if(CustRes_isContained(SourceFile_fullPath))
    {
        QMap<QString, QString> Res_map = CustRes_getResMap(SourceFile_fullPath);//res_map["fullpath"],["height"],["width"]
        int CustRes_height=Res_map["height"].toInt();
        int CustRes_width=Res_map["width"].toInt();
        //=========================
        QMap<QString,int> res_map = Image_Gif_Read_Resolution(SourceFile_fullPath);
        int original_height = res_map["height"];
        int original_width = res_map["width"];
        if(original_height<=0||original_width<=0)//判断是否读取失败
        {
            return false;
        }
        //==========================
        if((CustRes_height*CustRes_width) <= (original_height*original_width))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}
/*
读取图片和GIF的分辨率
*/
QMap<QString,int> MainWindow::Image_Gif_Read_Resolution(QString SourceFileFullPath)
{
    QString program = Current_Path+"/identify_waifu2xEX.exe";
    QProcess QProcess_Read_Resolution;
    QProcess_Read_Resolution.start("\""+program+"\" -format \"%w:%h;success;\" \""+SourceFileFullPath+"\"");
    while(!QProcess_Read_Resolution.waitForStarted(100)&&!QProcess_stop) {}
    while(!QProcess_Read_Resolution.waitForFinished(100)&&!QProcess_stop) {}
    QString QProcess_Read_Resolution_OutputStr = QProcess_Read_Resolution.readAllStandardOutput().trimmed();
    if(QProcess_Read_Resolution_OutputStr.contains("success"))
    {
        QStringList Res_strList = QProcess_Read_Resolution_OutputStr.split(";").at(0).split(":");
        int width = Res_strList.at(0).toInt();
        int height = Res_strList.at(1).toInt();
        if(height>0 && width>0)
        {
            QMap<QString,int> res_map;
            res_map["height"] = height;
            res_map["width"] = width;
            return res_map;
        }
    }
    //==============================
    QImage qimage_GifFileFullPath;
    qimage_GifFileFullPath.load(SourceFileFullPath);
    int original_height = qimage_GifFileFullPath.height();
    int original_width = qimage_GifFileFullPath.width();
    if(original_height<=0||original_width<=0)
    {
        emit Send_TextBrowser_NewMessage(tr("ERROR! Unable to read the resolution of the GIF. [")+SourceFileFullPath+"]");
        QMap<QString,int> empty;
        empty["height"] = 0;
        empty["width"] = 0;
        return empty;
    }
    else
    {
        QMap<QString,int> res_map;
        res_map["height"] = original_height;
        res_map["width"] = original_width;
        return res_map;
    }
}
/*
修改图片的 格式 与 图像质量
返回修改完成后文件的路径
*/
QString MainWindow::SaveImageAs_FormatAndQuality(QString OriginalSourceImage_fullPath,QString ScaledImage_fullPath,bool isDenoiseLevelEnabled,int DenoiseLevel)
{
    QString FinalFile_FullName = "";
    QString FinalFile_Ext = "";
    QString FinalFile_Folder = "";
    QString FinalFile_FullPath = "";
    int ImageQualityLevel = ui->spinBox_ImageQualityLevel->value();
    //=========== 确定扩展名 ===================
    FinalFile_Ext = ui->comboBox_ImageSaveFormat->currentText();
    QImage QImage_OriginalSourceImage_fullPath(OriginalSourceImage_fullPath);
    if(QImage_OriginalSourceImage_fullPath.hasAlphaChannel()&&ui->checkBox_AutoDetectAlphaChannel->isChecked())
    {
        if(FinalFile_Ext=="jpg" || FinalFile_Ext=="tga")
        {
            FinalFile_Ext = "png";
        }
    }
    //================ 判断是否要继续 ==================
    //如果扩展名不变且画质拉满,则直接返回原图片路径
    QFileInfo ScaledImage_fullPath_fileinfo(ScaledImage_fullPath);
    if((FinalFile_Ext==ScaledImage_fullPath_fileinfo.suffix())&&(ImageQualityLevel==100))
    {
        return ScaledImage_fullPath;
    }
    //==========================
    QFileInfo OriginalSourceImage_fileinfo(OriginalSourceImage_fullPath);
    QString OriginalSourceImage_file_name = file_getBaseName(OriginalSourceImage_fullPath);
    QString OriginalSourceImage_file_ext = OriginalSourceImage_fileinfo.suffix();
    //============ 确定文件夹 ==============
    FinalFile_Folder = file_getFolderPath(OriginalSourceImage_fileinfo);
    //============== 确定文件名 ============
    QString Compressed_str = "";
    if(ImageQualityLevel<100)
    {
        Compressed_str = "_compressed";
    }
    QString OriginalExt_str = "";
    if(OriginalSourceImage_file_ext!=FinalFile_Ext)
    {
        OriginalExt_str="_"+OriginalSourceImage_file_ext;
    }
    QString DenoiseLevel_str = "";
    if(isDenoiseLevelEnabled)
    {
        DenoiseLevel_str="_"+QString::number(DenoiseLevel,10)+"n";
    }
    //===
    if(CustRes_isContained(OriginalSourceImage_fullPath))
    {
        QMap<QString, QString> Res_map = CustRes_getResMap(OriginalSourceImage_fullPath);//res_map["fullpath"],["height"],["width"]
        int CustRes_height=Res_map["height"].toInt();
        int CustRes_width=Res_map["width"].toInt();
        FinalFile_FullName = OriginalSourceImage_file_name + "_waifu2x_"+QString::number(CustRes_width, 10)+"x"+QString::number(CustRes_height, 10)+DenoiseLevel_str+Compressed_str+OriginalExt_str+"."+FinalFile_Ext;
    }
    else
    {
        FinalFile_FullName = OriginalSourceImage_file_name + "_waifu2x_"+QString("%1").arg(ui->doubleSpinBox_ScaleRatio_image->value())+"x"+DenoiseLevel_str+Compressed_str+OriginalExt_str+"."+FinalFile_Ext;
    }
    //============ 组装完整路径 ==============
    FinalFile_FullPath = FinalFile_Folder+"/"+FinalFile_FullName;
    //==========================
    QString program = Current_Path+"/convert_waifu2xEX.exe";
    QFile::remove(FinalFile_FullPath);
    QProcess SaveImageAs_QProcess;
    SaveImageAs_QProcess.start("\""+program+"\" \""+ScaledImage_fullPath+"\" -quality "+QString::number(ImageQualityLevel,10)+" \""+FinalFile_FullPath+"\"");
    while(!SaveImageAs_QProcess.waitForStarted(100)&&!QProcess_stop) {}
    while(!SaveImageAs_QProcess.waitForFinished(100)&&!QProcess_stop) {}
    //======
    QFileInfo *FinalFile_FullPath_QFileInfo = new QFileInfo(FinalFile_FullPath);
    if((QFile::exists(FinalFile_FullPath)==false) || (FinalFile_FullPath_QFileInfo->size()<1))
    {
        QFile::remove(FinalFile_FullPath);
        emit Send_TextBrowser_NewMessage(tr("Error: Can\'t convert [")+ScaledImage_fullPath+tr("] to ")+FinalFile_Ext);
        return ScaledImage_fullPath;
    }
    QFile::remove(ScaledImage_fullPath);
    return FinalFile_FullPath;
}
/*
根据保存的格式判断是否要启用 质量等级 调整
*/
void MainWindow::on_comboBox_ImageSaveFormat_currentIndexChanged(int index)
{
    if(ui->comboBox_ImageSaveFormat->currentIndex()>2)
    {
        ui->spinBox_ImageQualityLevel->setEnabled(0);
        ui->spinBox_ImageQualityLevel->setValue(100);
    }
    else
    {
        ui->spinBox_ImageQualityLevel->setEnabled(1);
    }
}
/*
判断图片是否含有透明通道
*/
bool MainWindow::Imgae_hasAlphaChannel(int rowNum)
{
    QString SourceFile_fullPath = Table_model_image->item(rowNum,2)->text();
    if(QFile::exists(SourceFile_fullPath)==false)return false;
    QImage QImage_SourceFile_fullPath(SourceFile_fullPath);
    return QImage_SourceFile_fullPath.hasAlphaChannel();
}
/*
预处理图片
*/
QString MainWindow::Imgae_PreProcess(QString ImagePath,bool ReProcess_AlphaChannel)
{
    if(QFile::exists(ImagePath)==false)
    {
        emit Send_TextBrowser_NewMessage(tr("Error: Can\'t pre-process [")+ImagePath+tr("]. File doesn't exists."));
        return ImagePath;
    }
    QFileInfo fileinfo_ImagePath(ImagePath);
    QString file_ext_ImagePath = fileinfo_ImagePath.suffix();
    QImage QImage_ImagePath(ImagePath);
    //预处理带有Alpha的图片
    if(ui->checkBox_AlwaysPreProcessAlphaPNG->isChecked()==true)
    {
        ReProcess_AlphaChannel = true;
    }
    if(ReProcess_AlphaChannel==true && QImage_ImagePath.hasAlphaChannel()==true)
    {
        //有alpha则开始转换
        QString file_name = file_getBaseName(ImagePath);
        QString file_Folder = file_getFolderPath(fileinfo_ImagePath);
        QString OutPut_Path_WebpCache = file_Folder + "/" + file_name + "_W2xEX_temp.webp";//输出的webp缓存的完整路径
        QString OutPut_Path_FinalPNG = file_Folder + "/" + file_name + "_W2xEX_PPAC.png";//输出的png图片的完整路径
        //======
        QString program = Current_Path+"/convert_waifu2xEX.exe";
        QFile::remove(OutPut_Path_FinalPNG);
        QProcess Convert2PNG;
        //先转换到质量99的webp
        Convert2PNG.start("\""+program+"\" \""+ImagePath+"\" -quality 99 \""+OutPut_Path_WebpCache+"\"");
        while(!Convert2PNG.waitForStarted(100)&&!QProcess_stop) {}
        while(!Convert2PNG.waitForFinished(100)&&!QProcess_stop) {}
        if(QFile::exists(OutPut_Path_WebpCache)==false)
        {
            emit Send_TextBrowser_NewMessage(tr("Error: Can\'t convert [")+ImagePath+tr("] to Webp. The pre-process will be skipped and try to process the original image directly."));
            return ImagePath;
        }
        //再转换回PNG
        Convert2PNG.start("\""+program+"\" \""+OutPut_Path_WebpCache+"\" -quality 100 \""+OutPut_Path_FinalPNG+"\"");
        while(!Convert2PNG.waitForStarted(100)&&!QProcess_stop) {}
        while(!Convert2PNG.waitForFinished(100)&&!QProcess_stop) {}
        QFile::remove(OutPut_Path_WebpCache);
        //======
        if(QFile::exists(OutPut_Path_FinalPNG)==false)
        {
            emit Send_TextBrowser_NewMessage(tr("Error: Can\'t convert [")+OutPut_Path_WebpCache+tr("] back to PNG. The pre-process will be skipped and try to process the original image directly."));
            return ImagePath;
        }
        //======
        return OutPut_Path_FinalPNG;
    }
    //判断是否已经是PNG
    if(ui->checkBox_PreProcessImage->isChecked()==false)return ImagePath;
    if(file_ext_ImagePath.trimmed().toLower()=="png")return ImagePath;
    //不是PNG则开始转换
    QString file_name = file_getBaseName(ImagePath);
    QString file_Folder = file_getFolderPath(fileinfo_ImagePath);
    QString OutPut_Path = file_Folder + "/" + file_name + "_W2xEX_"+file_ext_ImagePath+".png";//输出的png图片的完整路径
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
