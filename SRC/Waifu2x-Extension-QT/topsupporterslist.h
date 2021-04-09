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
