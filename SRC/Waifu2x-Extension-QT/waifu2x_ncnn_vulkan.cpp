#include "mainwindow.h"
#include "ui_mainwindow.h"

int MainWindow::Waifu2x_NCNN_Vulkan_Image(QMap<QString, QString> File_map)
{
    //============================= 读取设置 ================================
    int ImageStyle = ui->comboBox_ImageStyle->currentIndex();
    int TileSize = ui->spinBox_TileSize->value();
    int ScaleRatio = ui->spinBox_ScaleRatio_image->value();
    int DenoiseLevel = ui->spinBox_DenoiseLevel_image->value();
    bool DelOriginal = ui->checkBox_DelOriginal->checkState();
    bool SaveAsJPG = ui->checkBox_SaveAsJPG->checkState();
    bool CompressJPG = ui->checkBox_CompressJPG->checkState();
    bool ReProcFinFiles = ui->checkBox_ReProcFinFiles->checkState();
    //========================= 拆解map得到参数 =============================
    int rowNum = File_map["rowNum"].toInt();
    QString status = "Processing";
    emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    QString fullPath = File_map["fullPath"];
    /*
    QFile qfile_fullPath(fullPath);
    if(!qfile_fullPath.isWritable())
    {
        emit Send_TextBrowser_NewMessage("Error occured when processing ["+fullPath+"]. Error: [Insufficient permissions, doesn't has write permission. Please give this software administrator permission.]");
        status = "Failed";
        emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        ThreadNumRunning--;//线程数量统计-1s
        return 0;
    }
    */
    QFileInfo fileinfo(fullPath);
    QString file_name = fileinfo.baseName();
    QString file_ext = fileinfo.suffix();
    QString file_path = fileinfo.path();
    if(file_path.right(1)=="/")
    {
        file_path = file_path.left(file_path.length() - 1);
    }
    QString OutPut_Path = file_path + "/" + file_name + "_waifu2x_"+QString::number(ScaleRatio, 10)+"x_"+QString::number(DenoiseLevel, 10)+"n.png";
    //============================== 放大 =======================================
    QProcess *Waifu2x = new QProcess();
    QString Current_Path = qApp->applicationDirPath();
    QString Waifu2x_folder_path = Current_Path + "/waifu2x-ncnn-vulkan";
    QString program = Waifu2x_folder_path + "/waifu2x-ncnn-vulkan.exe";
    QString model_path;
    if(ImageStyle==0)
    {
        model_path = Waifu2x_folder_path+"/models-upconv_7_anime_style_art_rgb";
    }
    else
    {
        model_path = Waifu2x_folder_path+"/models-upconv_7_photo";
    }
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
    QString InputPath_tmp = fullPath;
    QString OutputPath_tmp ="";
    int DenoiseLevel_tmp = DenoiseLevel;
    for(int i=2; i<=ScaleRatio_tmp; i+=2)
    {
        OutputPath_tmp = file_path + "/" + file_name + "_waifu2x_"+QString::number(i, 10)+"x_"+QString::number(DenoiseLevel, 10)+"n.png";
        QString cmd = "\"" + program + "\"" + " -i " + "\"" + InputPath_tmp + "\"" + " -o " + "\"" + OutputPath_tmp + "\"" + " -s " + "2" + " -n " + QString::number(DenoiseLevel_tmp, 10) + " -t " + QString::number(TileSize, 10) + " -m " + "\"" + model_path + "\"" + " -j " + "1:1:1";
        Waifu2x->start(cmd);
        Waifu2x->waitForStarted();
        while(!Waifu2x->waitForFinished(500))
        {
            if(waifu2x_STOP)
            {
                Waifu2x->close();
                if(i>2)
                {
                    QFile::remove(InputPath_tmp);
                }
                status = "Interrupted";
                emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, status);
                ThreadNumRunning--;
                return 0;
            }
        }
        if(!file_isFileExist(OutputPath_tmp))
        {
            if(i>2)
            {
                QFile::remove(InputPath_tmp);
            }
            emit Send_TextBrowser_NewMessage("Error occured when processing ["+fullPath+"]. Error: [Unable to scale the picture.]");
            status = "Failed";
            emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, status);
            ThreadNumRunning--;
            return 0;
        }
        if(i>2)
        {
            QFile::remove(InputPath_tmp);
            DenoiseLevel_tmp = -1;
        }
        InputPath_tmp = OutputPath_tmp;
    }
    //============================ 调整大小 ====================================================
    if(ScaleRatio_tmp != ScaleRatio)
    {
        QImage qimage_original;
        qimage_original.load(fullPath);
        int New_height = qimage_original.height()*ScaleRatio;//用原文件的height乘以scaleRatio得到新的height
        int New_width = qimage_original.width()*ScaleRatio;//用原文件的width乘以scaleRatio得到新的width
        QImage qimage_adj(OutputPath_tmp);
        qimage_adj.load(OutputPath_tmp);
        //读取放大后的图片并调整大小
        QImage qimage_adj_scaled = qimage_adj.scaled(New_width,New_height,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        QImageWriter qimageW_adj;
        qimageW_adj.setFormat("png");
        qimageW_adj.setFileName(OutPut_Path);
        if(qimageW_adj.canWrite())
        {
            qimageW_adj.write(qimage_adj_scaled);
        }
        QFile::remove(OutputPath_tmp);
        if(!file_isFileExist(OutPut_Path))
        {
            emit Send_TextBrowser_NewMessage("Error occured when processing ["+fullPath+"]. Error: [Unable to resize the scaled picture to the target size]");
            status = "Failed";
            emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, status);
            ThreadNumRunning--;//线程数量统计-1s
            return 0;
        }
    }
    //=========================== 另存为JPG&压缩JPG ===========================================
    if(SaveAsJPG)
    {
        QString OutPut_Path_jpg;
        QImage qimage;
        qimage.load(OutPut_Path);
        QImageWriter qimageW;
        qimageW.setFormat("jpg");
        if(CompressJPG)//压缩JPG
        {
            OutPut_Path_jpg = file_path + "/" + file_name + "_waifu2x_"+QString::number(ScaleRatio, 10)+"x_"+QString::number(DenoiseLevel, 10)+"n_compressed.jpg";
            qimageW.setQuality(90);
            qimageW.setFileName(OutPut_Path_jpg);
        }
        else
        {
            OutPut_Path_jpg = file_path + "/" + file_name + "_waifu2x_"+QString::number(ScaleRatio, 10)+"x_"+QString::number(DenoiseLevel, 10)+"n.jpg";
            qimageW.setQuality(100);
            qimageW.setFileName(OutPut_Path_jpg);
        }
        if(qimageW.canWrite())
        {
            qimageW.write(qimage);
        }
        else
        {
            // 获取错误信息
            QImageWriter::ImageWriterError error = qimageW.error();
            QString strError = qimageW.errorString();
            emit TextBrowser_NewMessage(strError);
        }
        if(file_isFileExist(OutPut_Path_jpg))
        {
            QFile::remove(OutPut_Path);
        }
        else
        {
            emit Send_TextBrowser_NewMessage("Error occured when processing ["+fullPath+"]. Error: [Cannot save scaled picture as .jpg.]");
        }
    }
    //============================= 删除原文件 & 更新filelist & 更新table status ============================
    if(DelOriginal)
    {
        QFile::remove(fullPath);
        FileList_remove(File_map);
        status = "Finished, original file deleted";
        emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    }
    else
    {
        if(!ReProcFinFiles)
        {
            FileList_remove(File_map);
            FileList_image_finished.append(File_map);
        }
        status = "Finished";
        emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    }
    //============================ 更新进度条 =================================
    emit Send_progressbar_Add();
    //=========================== 更新线程数量统计==============================
    ThreadNumRunning--;//线程数量统计-1s
    return 0;
}

/*

  =============================================================================================
  =============================================================================================
  ============================= Waifu2x_NCNN_Vulkan_GIF =======================================
  =============================================================================================
  =============================================================================================


*/

int MainWindow::Waifu2x_NCNN_Vulkan_GIF(QMap<QString, QString> File_map)
{
    //============================= 读取设置 ================================
    int ScaleRatio = ui->spinBox_ScaleRatio_gif->value();
    int DenoiseLevel = ui->spinBox_DenoiseLevel_gif->value();
    bool DelOriginal = ui->checkBox_DelOriginal->checkState();
    bool ReProcFinFiles = ui->checkBox_ReProcFinFiles->checkState();
    bool OptGIF = ui->checkBox_OptGIF->checkState();
    int Sub_gif_ThreadNumRunning = 0;
    //========================= 拆解map得到参数 =============================
    int rowNum = File_map["rowNum"].toInt();
    QString status = "Processing";
    emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    QString fullPath = File_map["fullPath"];
    /*
    QFile qfile_fullPath(fullPath);
    if(!qfile_fullPath.isWritable())
    {
        emit Send_TextBrowser_NewMessage("Error occured when processing ["+fullPath+"]. Error: [Insufficient permissions, doesn't has write permission. Please give this software administrator permission.]");
        status = "Failed";
        emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        ThreadNumRunning--;//线程数量统计-1s
        return 0;
    }*/
    QFileInfo fileinfo(fullPath);
    QString file_name = fileinfo.baseName();
    QString file_ext = fileinfo.suffix();
    QString file_path = fileinfo.path();
    if(file_path.right(1)=="/")
    {
        file_path = file_path.left(file_path.length() - 1);
    }
    QString ResGIFPath = file_path + "/" + file_name + "_waifu2x_"+QString::number(ScaleRatio, 10)+"x_"+QString::number(DenoiseLevel, 10)+"n.gif";
    //=========================== 获取帧时间 ====================================
    int GIF_Duration = Gif_getDuration(fullPath);
    if(GIF_Duration==0)//检查是否成功获取duration
    {
        emit Send_TextBrowser_NewMessage("Error occured when processing ["+fullPath+"]. Error: [Can't get Duration value of GIF file.]");
        status = "Failed";
        emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        //file_DelDir(SplitFramesFolderPath);
        ThreadNumRunning--;//线程数量统计-1s
        return 0;//如果启用stop位,则直接return
    }
    //============================== 拆分 ==========================================
    QString SplitFramesFolderPath = file_path+"/"+file_name+"_splitFrames_waifu2x";//拆分后存储frame的文件夹
    if(file_isDirExist(SplitFramesFolderPath))
    {
        file_DelDir(SplitFramesFolderPath);
        file_mkDir(SplitFramesFolderPath);
    }
    else
    {
        file_mkDir(SplitFramesFolderPath);
    }
    Gif_splitGif(fullPath,SplitFramesFolderPath);
    //============================== 扫描获取文件名 ===============================
    QStringList Frame_fileName_list = file_getFileNames_in_Folder_nofilter(SplitFramesFolderPath);
    if(Frame_fileName_list.isEmpty())//检查是否成功拆分gif
    {
        emit Send_TextBrowser_NewMessage("Error occured when processing ["+fullPath+"]. Error: [Can't split GIF into frames.]");
        status = "Failed";
        emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        file_DelDir(SplitFramesFolderPath);
        ThreadNumRunning--;//线程数量统计-1s
        return 0;//如果启用stop位,则直接return
    }
    //===========建立存储放大后frame的文件夹===========
    QString ScaledFramesFolderPath = SplitFramesFolderPath+"/scaled";
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
    int InterPro_total = Frame_fileName_list.size();
    int InterPro_now = 0;
    for(int i = 0; i < Frame_fileName_list.size(); i++)
    {
        InterPro_now++;
        if(ui->checkBox_ShowInterPro->checkState())
        {
            emit Send_TextBrowser_NewMessage("File name:["+fullPath+"]  Scale and Denoise progress:["+QString::number(InterPro_now,10)+"/"+QString::number(InterPro_total,10)+"]");
        }
        int Sub_gif_ThreadNumMax = ui->spinBox_ThreadNum_gif_internal->value();
        if(waifu2x_STOP)
        {
            while (Sub_gif_ThreadNumRunning > 0)
            {
                Delay_msec_sleep(500);
            }
            file_DelDir(SplitFramesFolderPath);
            status = "Interrupted";
            emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
            ThreadNumRunning--;//线程数量统计-1s
            return 0;//如果启用stop位,则直接return
        }
        Sub_gif_ThreadNumRunning++;
        QString Frame_fileName = Frame_fileName_list.at(i);
        QtConcurrent::run(this,&MainWindow::Waifu2x_NCNN_Vulkan_GIF_scale,Frame_fileName,SplitFramesFolderPath,ScaledFramesFolderPath,&Sub_gif_ThreadNumRunning);
        while (Sub_gif_ThreadNumRunning >= Sub_gif_ThreadNumMax)
        {
            Delay_msec_sleep(500);
        }
    }
    while (Sub_gif_ThreadNumRunning!=0)
    {
        Delay_msec_sleep(500);
    }
    //======================= 检查是否成功放大所有帧 ===========================
    QStringList Frame_fileName_list_scaled = file_getFileNames_in_Folder_nofilter(ScaledFramesFolderPath);
    if(Frame_fileName_list.count()!=Frame_fileName_list_scaled.count())
    {
        emit Send_TextBrowser_NewMessage("Error occured when processing ["+fullPath+"]. Error: [Failed to scale frames.]");
        status = "Failed";
        emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        file_DelDir(SplitFramesFolderPath);
        ThreadNumRunning--;//线程数量统计-1s
        return 0;//如果启用stop位,则直接return
    }
    //======================================== 组装 ======================================================
    Gif_assembleGif(ResGIFPath,ScaledFramesFolderPath,GIF_Duration);
    if(!file_isFileExist(ResGIFPath))
    {
        emit Send_TextBrowser_NewMessage("Error occured when processing ["+fullPath+"]. Error: [Unable to assemble gif.]");
        status = "Failed";
        emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        file_DelDir(SplitFramesFolderPath);
        ThreadNumRunning--;//线程数量统计-1s
        return 0;//如果启用stop位,则直接return
    }
    //======================================= 优化gif ===================================================
    if(OptGIF)
    {
        QString ResGIFPath_compressed = file_path + "/" + file_name + "_waifu2x_"+QString::number(ScaleRatio, 10)+"x_"+QString::number(DenoiseLevel, 10)+"n_opt.gif";
        Gif_compressGif(ResGIFPath,ResGIFPath_compressed);
        if(file_isFileExist(ResGIFPath_compressed))
        {
            QFile::remove(ResGIFPath);
        }
        else
        {
            emit Send_TextBrowser_NewMessage("Error occured when processing ["+fullPath+"]. Error: [Can't optimize gif.]");
        }
    }
    //============================== 删除缓存文件 ====================================================
    file_DelDir(SplitFramesFolderPath);
    //============================= 删除原文件 & 更新filelist & 更新table status ============================
    if(DelOriginal)
    {
        QFile::remove(fullPath);
        FileList_remove(File_map);
        status = "Finished, original file deleted";
        emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    }
    else
    {
        if(!ReProcFinFiles)
        {
            FileList_remove(File_map);
            FileList_gif_finished.append(File_map);
        }
        status = "Finished";
        emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    }
    //============================ 更新进度条 =================================
    emit Send_progressbar_Add();
    //=========================== 更新filelist ==============================
    ThreadNumRunning--;//线程数量统计-1s
    return 0;
}

int MainWindow::Waifu2x_NCNN_Vulkan_GIF_scale(QString Frame_fileName,QString SplitFramesFolderPath,QString ScaledFramesFolderPath,int *Sub_gif_ThreadNumRunning)
{
    int ImageStyle = ui->comboBox_ImageStyle->currentIndex();
    int TileSize = ui->spinBox_TileSize->value();
    int ScaleRatio = ui->spinBox_ScaleRatio_gif->value();
    int DenoiseLevel = ui->spinBox_DenoiseLevel_gif->value();
    QString Frame_fileFullPath = SplitFramesFolderPath+"/"+Frame_fileName;
    QFileInfo fileinfo_frame(Frame_fileFullPath);
    QString Frame_fileName_basename = fileinfo_frame.baseName();
    QString Frame_fileOutPutPath = ScaledFramesFolderPath+"/"+Frame_fileName_basename+ "_waifu2x_"+QString::number(ScaleRatio, 10)+"x_"+QString::number(DenoiseLevel, 10)+"n.png";
    //========================================================================
    QProcess *Waifu2x = new QProcess();
    QString Current_Path = qApp->applicationDirPath();
    QString Waifu2x_folder_path = Current_Path + "/waifu2x-ncnn-vulkan";
    QString program = Waifu2x_folder_path + "/waifu2x-ncnn-vulkan.exe";
    QString model_path;
    if(ImageStyle==0)
    {
        model_path = Waifu2x_folder_path+"/models-upconv_7_anime_style_art_rgb";
    }
    else
    {
        model_path = Waifu2x_folder_path+"/models-upconv_7_photo";
    }
    int ScaleRatio_tmp=0;
    if((ScaleRatio%2)==0)
    {
        ScaleRatio_tmp = ScaleRatio;
    }
    else
    {
        ScaleRatio_tmp = ScaleRatio+1;
    }
    QString InputPath_tmp = Frame_fileFullPath;
    QString OutputPath_tmp ="";
    int DenoiseLevel_tmp = DenoiseLevel;
    for(int i=2; i<=ScaleRatio_tmp; i+=2)
    {
        OutputPath_tmp =  ScaledFramesFolderPath+"/"+Frame_fileName_basename+ "_waifu2x_"+QString::number(i, 10)+"x_"+QString::number(DenoiseLevel, 10)+"n.png";
        QString cmd = "\"" + program + "\"" + " -i " + "\"" + InputPath_tmp + "\"" + " -o " + "\"" + OutputPath_tmp + "\"" + " -s " + "2" + " -n " + QString::number(DenoiseLevel_tmp, 10) + " -t " + QString::number(TileSize, 10) + " -m " + "\"" + model_path + "\"" + " -j " + "1:1:1";
        Waifu2x->start(cmd);
        Waifu2x->waitForStarted();
        while(!Waifu2x->waitForFinished(500))
        {
            if(waifu2x_STOP)
            {
                Waifu2x->close();
                *Sub_gif_ThreadNumRunning=*Sub_gif_ThreadNumRunning-1;
                return 0;
            }
        }
        if(i>2)
        {
            QFile::remove(InputPath_tmp);
            DenoiseLevel_tmp = -1;
        }
        InputPath_tmp = OutputPath_tmp;
    }
    //============================ 调整大小 ====================================================
    if(ScaleRatio_tmp != ScaleRatio)
    {
        QImage qimage_original;
        qimage_original.load(Frame_fileFullPath);
        int New_height = qimage_original.height()*ScaleRatio;//用原文件的height乘以scaleRatio得到新的height
        int New_width = qimage_original.width()*ScaleRatio;//用原文件的width乘以scaleRatio得到新的width
        QImage qimage_adj(OutputPath_tmp);
        qimage_adj.load(OutputPath_tmp);
        //读取放大后的图片并调整大小
        QImage qimage_adj_scaled = qimage_adj.scaled(New_width,New_height,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        QImageWriter qimageW_adj;
        qimageW_adj.setFormat("png");
        qimageW_adj.setFileName(Frame_fileOutPutPath);
        if(qimageW_adj.canWrite())
        {
            qimageW_adj.write(qimage_adj_scaled);
            QFile::remove(OutputPath_tmp);
        }
    }
    QFile::remove(Frame_fileFullPath);
    *Sub_gif_ThreadNumRunning=*Sub_gif_ThreadNumRunning-1;
    return 0;
}

/*

  =============================================================================================
  =============================================================================================
  ============================= Waifu2x_NCNN_Vulkan_Video =======================================
  =============================================================================================
  =============================================================================================


*/

int MainWindow::Waifu2x_NCNN_Vulkan_Video(QMap<QString, QString> File_map)
{
    //============================= 读取设置 ================================
    bool DelOriginal = ui->checkBox_DelOriginal->checkState();
    bool ReProcFinFiles = ui->checkBox_ReProcFinFiles->checkState();
    int Sub_video_ThreadNumRunning = 0;
    //========================= 拆解map得到参数 =============================
    int rowNum = File_map["rowNum"].toInt();
    QString status = "Processing";
    emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    QString fullPath = File_map["fullPath"];
    /*
    QFile qfile_fullPath(fullPath);
    if(!qfile_fullPath.isWritable())
    {
        emit Send_TextBrowser_NewMessage("Error occured when processing ["+fullPath+"]. Error: [Insufficient permissions, doesn't has write permission. Please give this software administrator permission.]");
        status = "Failed";
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        ThreadNumRunning--;//线程数量统计-1s
        return 0;
    }*/
    QFileInfo fileinfo(fullPath);
    QString file_name = fileinfo.baseName();
    QString file_ext = fileinfo.suffix();
    QString file_path = fileinfo.path();
    if(file_path.right(1)=="/")
    {
        file_path = file_path.left(file_path.length() - 1);
    }
    QString video_mp4_fullpath = file_path+"/"+file_name+".mp4";
    if(file_ext!="mp4")
    {
        QFile::remove(video_mp4_fullpath);
    }
    QString AudioPath = file_path+"/audio_waifu2x.wav";
    //============================== 拆分 ==========================================
    QString SplitFramesFolderPath = file_path+"/"+file_name+"_splitFrames_waifu2x";//拆分后存储frame的文件夹
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
    video_video2images(fullPath,SplitFramesFolderPath);
    if(!file_isFileExist(video_mp4_fullpath))//检查是否成功生成mp4
    {
        emit Send_TextBrowser_NewMessage("Error occured when processing ["+fullPath+"]. Error: [Cannot convert video format to mp4.]");
        status = "Failed";
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        file_DelDir(SplitFramesFolderPath);
        QFile::remove(AudioPath);
        ThreadNumRunning--;//线程数量统计-1s
        return 0;//如果启用stop位,则直接return
    }
    //============================== 扫描获取文件名 ===============================
    QStringList Frame_fileName_list = file_getFileNames_in_Folder_nofilter(SplitFramesFolderPath);
    if(Frame_fileName_list.isEmpty())//检查是否成功拆分为帧
    {
        emit Send_TextBrowser_NewMessage("Error occured when processing ["+fullPath+"]. Error: [Unable to split video into pictures.]");
        status = "Failed";
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        file_DelDir(SplitFramesFolderPath);
        QFile::remove(AudioPath);
        ThreadNumRunning--;//线程数量统计-1s
        return 0;//如果启用stop位,则直接return
    }
    //============================== 放大 =======================================
    //===========建立存储放大后frame的文件夹===========
    QString ScaledFramesFolderPath = SplitFramesFolderPath+"/scaled";
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
    int InterPro_total = Frame_fileName_list.size();
    int InterPro_now = 0;
    for(int i = 0; i < Frame_fileName_list.size(); i++)
    {
        InterPro_now++;
        if(ui->checkBox_ShowInterPro->checkState())
        {
            emit Send_TextBrowser_NewMessage("File name:["+fullPath+"]  Scale and Denoise progress:["+QString::number(InterPro_now,10)+"/"+QString::number(InterPro_total,10)+"]");
        }
        int Sub_video_ThreadNumMax = ui->spinBox_ThreadNum_video_internal->value();
        if(waifu2x_STOP)
        {
            while (Sub_video_ThreadNumRunning > 0)
            {
                Delay_msec_sleep(500);
            }
            file_DelDir(SplitFramesFolderPath);
            QFile::remove(AudioPath);
            status = "Interrupted";
            emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
            ThreadNumRunning--;//线程数量统计-1s
            return 0;//如果启用stop位,则直接return
        }
        Sub_video_ThreadNumRunning++;
        QString Frame_fileName = Frame_fileName_list.at(i);
        QtConcurrent::run(this,&MainWindow::Waifu2x_NCNN_Vulkan_Video_scale,Frame_fileName,SplitFramesFolderPath,ScaledFramesFolderPath,&Sub_video_ThreadNumRunning);
        while (Sub_video_ThreadNumRunning >= Sub_video_ThreadNumMax)
        {
            Delay_msec_sleep(500);
        }
    }
    while (Sub_video_ThreadNumRunning!=0)
    {
        Delay_msec_sleep(500);
    }
    //================ 扫描放大后的帧文件数量,判断是否放大成功 =======================
    QStringList Frame_fileName_list_scaled = file_getFileNames_in_Folder_nofilter(ScaledFramesFolderPath);
    if(Frame_fileName_list.count()!=Frame_fileName_list_scaled.count())
    {
        emit Send_TextBrowser_NewMessage("Error occured when processing ["+fullPath+"]. Error: [Unable to scale all frames.]");
        status = "Failed";
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        file_DelDir(SplitFramesFolderPath);
        QFile::remove(AudioPath);
        ThreadNumRunning--;//线程数量统计-1s
        return 0;//如果启用stop位,则直接return
    }
    //======================================== 组装 ======================================================
    QString video_mp4_scaled_fullpath = file_path+"/"+file_name+"_waifu2x.mp4";
    QFile::remove(video_mp4_scaled_fullpath);
    video_images2video(video_mp4_fullpath,ScaledFramesFolderPath);
    if(!file_isFileExist(video_mp4_scaled_fullpath))//检查是否成功成功生成视频
    {
        emit Send_TextBrowser_NewMessage("Error occured when processing ["+fullPath+"]. Error: [Unable to assemble pictures into videos.]");
        status = "Failed";
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        file_DelDir(SplitFramesFolderPath);
        QFile::remove(AudioPath);
        ThreadNumRunning--;//线程数量统计-1s
        return 0;//如果启用stop位,则直接return
    }
    //============================== 删除缓存文件 ====================================================
    if(file_isDirExist(SplitFramesFolderPath))
    {
        file_DelDir(SplitFramesFolderPath);
    }
    //============================= 删除原文件 & 更新filelist & 更新table status ============================
    if(DelOriginal)
    {
        QFile::remove(fullPath);
        QFile::remove(video_mp4_fullpath);
        FileList_remove(File_map);
        status = "Finished, original file deleted";
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    }
    else
    {
        if(!ReProcFinFiles)
        {
            FileList_remove(File_map);
            FileList_video_finished.append(File_map);
        }
        status = "Finished";
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, status);
    }
    //============================ 更新进度条 =================================
    emit Send_progressbar_Add();
    //=========================== 更新filelist ==============================
    ThreadNumRunning--;//线程数量统计-1s
    return 0;
}




int MainWindow::Waifu2x_NCNN_Vulkan_Video_scale(QString Frame_fileName,QString SplitFramesFolderPath,QString ScaledFramesFolderPath,int *Sub_video_ThreadNumRunning)
{
    int ImageStyle = ui->comboBox_ImageStyle->currentIndex();
    int TileSize = ui->spinBox_TileSize->value();
    int ScaleRatio = ui->spinBox_ScaleRatio_video->value();
    int DenoiseLevel = ui->spinBox_DenoiseLevel_video->value();
    //========================================================================
    QString Frame_fileFullPath = SplitFramesFolderPath+"/"+Frame_fileName;
    QFileInfo fileinfo_frame(Frame_fileFullPath);
    QString Frame_fileName_basename = fileinfo_frame.baseName();
    QString Frame_fileOutPutPath = ScaledFramesFolderPath+"/"+Frame_fileName_basename+ "_waifu2x_"+QString::number(ScaleRatio, 10)+"x_"+QString::number(DenoiseLevel, 10)+"n.png";
    //========================================================================
    QProcess *Waifu2x = new QProcess();
    QString Current_Path = qApp->applicationDirPath();
    QString Waifu2x_folder_path = Current_Path + "/waifu2x-ncnn-vulkan";
    QString program = Waifu2x_folder_path + "/waifu2x-ncnn-vulkan.exe";
    QString model_path;
    if(ImageStyle==0)
    {
        model_path = Waifu2x_folder_path+"/models-upconv_7_anime_style_art_rgb";
    }
    else
    {
        model_path = Waifu2x_folder_path+"/models-upconv_7_photo";
    }
    int ScaleRatio_tmp=0;
    if((ScaleRatio%2)==0)
    {
        ScaleRatio_tmp = ScaleRatio;
    }
    else
    {
        ScaleRatio_tmp = ScaleRatio+1;
    }
    QString InputPath_tmp = Frame_fileFullPath;
    QString OutputPath_tmp ="";
    int DenoiseLevel_tmp = DenoiseLevel;
    for(int i=2; i<=ScaleRatio_tmp; i+=2)
    {
        OutputPath_tmp =  ScaledFramesFolderPath+"/"+Frame_fileName_basename+ "_waifu2x_"+QString::number(i, 10)+"x_"+QString::number(DenoiseLevel, 10)+"n.png";
        QString cmd = "\"" + program + "\"" + " -i " + "\"" + InputPath_tmp + "\"" + " -o " + "\"" + OutputPath_tmp + "\"" + " -s " + "2" + " -n " + QString::number(DenoiseLevel_tmp, 10) + " -t " + QString::number(TileSize, 10) + " -m " + "\"" + model_path + "\"" + " -j " + "1:1:1";
        Waifu2x->start(cmd);
        Waifu2x->waitForStarted();
        while(!Waifu2x->waitForFinished(500))
        {
            if(waifu2x_STOP)
            {
                Waifu2x->close();
                *Sub_video_ThreadNumRunning=*Sub_video_ThreadNumRunning-1;
                return 0;
            }
        }
        if(i>2)
        {
            QFile::remove(InputPath_tmp);
            DenoiseLevel_tmp = -1;
        }
        InputPath_tmp = OutputPath_tmp;
    }
    //============================ 调整大小 ====================================================
    if(ScaleRatio_tmp != ScaleRatio)
    {
        QImage qimage_original;
        qimage_original.load(Frame_fileFullPath);
        int New_height = qimage_original.height()*ScaleRatio;
        int New_width = qimage_original.width()*ScaleRatio;
        QImage qimage_adj(OutputPath_tmp);
        qimage_adj.load(OutputPath_tmp);
        QImage qimage_adj_scaled = qimage_adj.scaled(New_width,New_height,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        QImageWriter qimageW_adj;
        qimageW_adj.setFormat("png");
        qimageW_adj.setFileName(Frame_fileOutPutPath);
        if(qimageW_adj.canWrite())
        {
            qimageW_adj.write(qimage_adj_scaled);
            QFile::remove(OutputPath_tmp);
        }
    }
    QFile::remove(Frame_fileFullPath);
    QFile::rename(Frame_fileOutPutPath,ScaledFramesFolderPath+"/"+Frame_fileName);
    *Sub_video_ThreadNumRunning=*Sub_video_ThreadNumRunning-1;
    return 0;
}


