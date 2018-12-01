#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include <QMainWindow>
#include "QStandardItemModel"
#include "ParkSystemManager.h"
#include "ParkSystemWorker.h"
#include "SQL_Test.h"
namespace Ui {
class AdminWindow;
}

class AdminWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AdminWindow(QWidget *parent = nullptr);
    ~AdminWindow();
    QStandardItemModel  *model;
    QStandardItemModel *initmodel;
    QString nowDiscount;
    QString nowPrice;
    QString nowVIPPrice;
private slots:
    void on_submitBtn_clicked();

    void on_changeDiscountBtn_clicked();

    void on_changePriceBtn_clicked();

    void on_changeVIPPriceBtn_clicked();

private:
    Ui::AdminWindow *ui;
};

#endif // ADMINWINDOW_H
