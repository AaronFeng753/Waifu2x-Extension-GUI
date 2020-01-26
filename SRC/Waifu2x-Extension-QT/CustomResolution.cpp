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

int MainWindow::CustRes_SetCustRes()
{
    if(curRow_image==-1&&curRow_video==-1&&curRow_gif==-1)
    {
        QMessageBox::information(this,"Error","No items are currently selected.");
        return 0;
    }
    RecFinedFiles();
    if(curRow_image >= 0)
    {
        QMap<QString,QString> res_map;
        QMap<QString, QString> fileMap = FileList_find_rowNum(FileList_image, curRow_image);
        if(!fileMap.isEmpty())
        {
            CustRes_remove(fileMap["SourceFile_fullPath"]);
            res_map["fullpath"] = fileMap["SourceFile_fullPath"];
            res_map["height"] = QString::number(ui->spinBox_CustRes_height->value(),10);
            res_map["width"] = QString::number(ui->spinBox_CustRes_width->value(),10);
            Custom_resolution_list.append(res_map);
            Table_image_CustRes_rowNumInt_HeightQString_WidthQString(curRow_image,res_map["height"],res_map["width"]);
        }
        if(!ui->checkBox_ReProcFinFiles->checkState())
        {
            MovToFinedList();
        }
        return 0;
    }
    if(curRow_gif >= 0)
    {
        QMap<QString,QString> res_map;
        QMap<QString, QString> fileMap = FileList_find_rowNum(FileList_gif, curRow_gif);
        if(!fileMap.isEmpty())
        {
            CustRes_remove(fileMap["SourceFile_fullPath"]);
            res_map["fullpath"] = fileMap["SourceFile_fullPath"];
            res_map["height"] = QString::number(ui->spinBox_CustRes_height->value(),10);
            res_map["width"] = QString::number(ui->spinBox_CustRes_width->value(),10);
            Custom_resolution_list.append(res_map);
            Table_gif_CustRes_rowNumInt_HeightQString_WidthQString(curRow_gif,res_map["height"],res_map["width"]);
        }
        if(!ui->checkBox_ReProcFinFiles->checkState())
        {
            MovToFinedList();
        }
        return 0;
    }
    if(curRow_video >= 0)
    {
        if(ui->spinBox_CustRes_height->value()%2!=0||ui->spinBox_CustRes_width->value()%2!=0)
        {
            QMessageBox::information(this, "Error", "When setting a custom resolution for a video, neither the height value nor the width value can be odd.");
            if(!ui->checkBox_ReProcFinFiles->checkState())
            {
                MovToFinedList();
            }
            return 0;
        }
        QMap<QString,QString> res_map;
        QMap<QString, QString> fileMap = FileList_find_rowNum(FileList_video, curRow_video);
        if(!fileMap.isEmpty())
        {
            CustRes_remove(fileMap["SourceFile_fullPath"]);
            res_map["fullpath"] = fileMap["SourceFile_fullPath"];
            res_map["height"] = QString::number(ui->spinBox_CustRes_height->value(),10);
            res_map["width"] = QString::number(ui->spinBox_CustRes_width->value(),10);
            Custom_resolution_list.append(res_map);
            Table_video_CustRes_rowNumInt_HeightQString_WidthQString(curRow_video,res_map["height"],res_map["width"]);
        }
        if(!ui->checkBox_ReProcFinFiles->checkState())
        {
            MovToFinedList();
        }
        return 0;
    }
    return 0;
}

int MainWindow::CustRes_CancelCustRes()
{
    if(curRow_image==-1&&curRow_video==-1&&curRow_gif==-1)
    {
        QMessageBox::information(this,"Error","No items are currently selected.");
        return 0;
    }
    RecFinedFiles();
    if(curRow_image >= 0)
    {
        QMap<QString, QString> fileMap = FileList_find_rowNum(FileList_image, curRow_image);
        if(!fileMap.isEmpty())
        {
            CustRes_remove(fileMap["SourceFile_fullPath"]);
            Table_image_CustRes_Cancel_rowNumInt(curRow_image);
        }
        if(!ui->checkBox_ReProcFinFiles->checkState())
        {
            MovToFinedList();
        }
        return 0;
    }
    if(curRow_gif >= 0)
    {
        QMap<QString, QString> fileMap = FileList_find_rowNum(FileList_gif, curRow_gif);
        if(!fileMap.isEmpty())
        {
            CustRes_remove(fileMap["SourceFile_fullPath"]);
            Table_gif_CustRes_Cancel_rowNumInt(curRow_gif);
        }
        if(!ui->checkBox_ReProcFinFiles->checkState())
        {
            MovToFinedList();
        }
        return 0;
    }
    if(curRow_video >= 0)
    {
        QMap<QString, QString> fileMap = FileList_find_rowNum(FileList_video, curRow_video);
        if(!fileMap.isEmpty())
        {
            CustRes_remove(fileMap["SourceFile_fullPath"]);
            Table_video_CustRes_Cancel_rowNumInt(curRow_video);
        }
        if(!ui->checkBox_ReProcFinFiles->checkState())
        {
            MovToFinedList();
        }
        return 0;
    }
    if(!ui->checkBox_ReProcFinFiles->checkState())
    {
        MovToFinedList();
    }
    return 0;
}

void MainWindow::CustRes_remove(QString fullpath)
{
    //QList<QMap<QString, QString>> Custom_resolution_list;//res_map["fullpath"],["height"],["width"]
    for(int i=0; i<Custom_resolution_list.size(); i++)
    {
        QMap<QString, QString> map_res = Custom_resolution_list.at(i);
        if(map_res["fullpath"]==fullpath)
        {
            Custom_resolution_list.removeAll(map_res);
        }
    }
}

bool MainWindow::CustRes_isContained(QString fullpath)
{
    //QList<QMap<QString, QString>> Custom_resolution_list;//res_map["fullpath"],["height"],["width"]
    for(int i=0; i<Custom_resolution_list.size(); i++)
    {
        QMap<QString, QString> map_res = Custom_resolution_list.at(i);
        if(map_res["fullpath"]==fullpath)
        {
            return true;
        }
    }
    return false;
}

QMap<QString, QString> MainWindow::CustRes_getResMap(QString fullpath)
{
    //QList<QMap<QString, QString>> Custom_resolution_list;//res_map["fullpath"],["height"],["width"]
    for(int i=0; i<Custom_resolution_list.size(); i++)
    {
        QMap<QString, QString> map_res = Custom_resolution_list.at(i);
        if(map_res["fullpath"]==fullpath)
        {
            return map_res;
        }
    }
    QMap<QString, QString> empty;
    empty.clear();
    return empty;
}

int MainWindow::CustRes_CalNewScaleRatio(QString fullpath,int Height_new,int width_new)
{
    QImage qimage_original;
    qimage_original.load(fullpath);
    if(qimage_original.height()==0||qimage_original.width()==0)
    {
        return 0;
    }
    //=====================分别计算长和宽的放大倍数=======================
    int ScaleRatio_height;
    int ScaleRatio_width;
    double ScaleRatio_height_double = Height_new/qimage_original.height();
    if(ScaleRatio_height_double-(int)ScaleRatio_height_double>0)
    {
        ScaleRatio_height = (int)(ScaleRatio_height_double)+1;
    }
    else
    {
        ScaleRatio_height = (int)(ScaleRatio_height_double);
    }
    double ScaleRatio_width_double = width_new/qimage_original.width();
    if(ScaleRatio_width_double-(int)ScaleRatio_width_double>0)
    {
        ScaleRatio_width = (int)(ScaleRatio_width_double)+1;
    }
    else
    {
        ScaleRatio_width = (int)(ScaleRatio_width_double);
    }
    //========================比较决定取哪个放大倍数值返回=====================
    if(ScaleRatio_height<=1&&ScaleRatio_width<=1)
    {
        return 1;
    }
    if(ScaleRatio_height>=ScaleRatio_width)
    {
        return ScaleRatio_height;
    }
    else
    {
        return ScaleRatio_width;
    }
}
