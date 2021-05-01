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

#ifndef TOPSUPPORTERSLIST_H
#define TOPSUPPORTERSLIST_H

#include <QWidget>
#include <QSettings>
#include <QFile>
#include <QTextCodec>
#include <QDesktopServices>

namespace Ui
{
    class TopSupportersList;
}

class TopSupportersList : public QWidget
{
    Q_OBJECT

public:
    explicit TopSupportersList(QWidget *parent = nullptr);
    ~TopSupportersList();

private slots:
    void on_pushButton_BecomePatron_clicked();

private:
    Ui::TopSupportersList *ui;
};

#endif // TOPSUPPORTERSLIST_H
