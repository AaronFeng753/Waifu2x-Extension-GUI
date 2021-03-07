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

//====== 自动关机===================================================================================
/*
60s倒计时
*/
int MainWindow::SystemShutDown_Countdown()
{
    Delay_sec_sleep(60);
    switch(ui->comboBox_FinishAction->currentIndex())
    {
        case 1:
        case 4:
            {
                emit Send_SystemShutDown();
                break;
            }
        case 2:
            {
                QProcess::execute(Current_Path+"/nircmd-x64/nircmd.exe standby");
                break;
            }
        case 3:
            {
                QProcess::execute(Current_Path+"/nircmd-x64/nircmd.exe hibernate");
                break;
            }
    }
    return 0;
}
/*
关机
保存列表,生成关机标志,关机
*/
bool MainWindow::SystemShutDown()
{
    Table_Save_Current_Table_Filelist(Current_Path+"/FilesList_W2xEX/FilesList_W2xEX_AutoSave.ini");
    //================
    QString AutoShutDown = Current_Path+"/AutoShutDown_W2xEX";
    QFile file(AutoShutDown);
    file.remove();
    if (file.open(QIODevice::ReadWrite | QIODevice::Text)) //QIODevice::ReadWrite支持读写
    {
        QTextStream stream(&file);
        stream << "Don't delete this file!!";
    }
    //================
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;
    //获取进程标志
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        return false;
    //获取关机特权的LUID
    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,    &tkp.Privileges[0].Luid);
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    //获取这个进程的关机特权
    AdjustTokenPrivileges(hToken, false, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
    if (GetLastError() != ERROR_SUCCESS) return false;
    //===
    switch(ui->comboBox_FinishAction->currentIndex())
    {
        case 1://关机
            {
                // 强制关闭计算机
                if ( !ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0))
                    return false;
                return true;
            }
        case 4://重启
            {
                // 强制重启计算机
                if ( !ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0))
                    return false;
                return true;
            }
    }
    return false;
}
/*
判断上次软件启动后是否执行了自动关机
*/
int MainWindow::SystemShutDown_isAutoShutDown()
{
    QString AutoShutDown = Current_Path+"/AutoShutDown_W2xEX";
    QString Table_FileList_ini = Current_Path+"/FilesList_W2xEX/FilesList_W2xEX_AutoSave.ini";
    if(QFile::exists(AutoShutDown)&&QFile::exists(Table_FileList_ini))
    {
        QMessageBox *MSG = new QMessageBox();
        MSG->setWindowTitle(tr("Notification"));
        MSG->setText(tr("It was detected that the program executed an automatic shutdown of the computer when it was last run. The last File List was automatically saved before the shutdown. You can manually load the File List to view the file processing status."));
        MSG->setIcon(QMessageBox::Information);
        MSG->setModal(true);
        MSG->show();
    }
    QFile::remove(AutoShutDown);//删除之前生成的自动关机标记
    return 0;
}

void MainWindow::AutoFinishAction_Message()
{
    QString ActionName = ui->comboBox_FinishAction->currentText();
    //===
    QMessageBox *AutoShutDown = new QMessageBox();
    AutoShutDown->setWindowTitle(tr("Warning!"));
    AutoShutDown->setText(tr("The computer will automatically ")+ActionName+tr(" in 60 seconds!"));
    AutoShutDown->setIcon(QMessageBox::Warning);
    AutoShutDown->setModal(false);
    AutoShutDown->show();
    //===
    emit Send_TextBrowser_NewMessage(tr("The computer will automatically ")+ActionName+tr(" in 60 seconds!"));
    emit Send_SystemTray_NewMessage(tr("The computer will automatically ")+ActionName+tr(" in 60 seconds!"));
}
