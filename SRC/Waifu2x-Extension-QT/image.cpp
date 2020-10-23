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
/*
修改图片的 格式 与 图像质量
返回修改完成后文件的路径
*/
QString MainWindow::SaveImageAs_FormatAndQuality(QString OriginalSourceImage_fullPath,QString ScaledImage_fullPath,int ScaleRatio,bool isDenoiseLevelEnabled,int DenoiseLevel)
{
    QString FinalFile_FullName = "";
    QString FinalFile_Ext = "";
    QString FinalFile_Folder = "";
    QString FinalFile_FullPath = "";
    int ImageQualityLevel = ui->spinBox_ImageQualityLevel->value();
    //=========== 确定扩展名 ===================
    QImage img(OriginalSourceImage_fullPath);
    if(img.hasAlphaChannel()&&ui->checkBox_AutoDetectAlphaChannel->isChecked())
    {
        FinalFile_Ext = "png";
    }
    else
    {
        FinalFile_Ext = ui->comboBox_ImageSaveFormat->currentText();
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
    QString OriginalSourceImage_file_name = file_getBaseName(OriginalSourceImage_fileinfo.filePath());
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
        FinalFile_FullName = OriginalSourceImage_file_name + "_waifu2x_"+QString::number(ScaleRatio, 10)+"x"+DenoiseLevel_str+Compressed_str+OriginalExt_str+"."+FinalFile_Ext;
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
        if(ui->comboBox_ImageSaveFormat->currentText()!="png")
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
    if(file_ext.trimmed().toLower()=="png")return ImagePath;
    if(ui->checkBox_PreProcessImage->isChecked()==false)return ImagePath;
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
