#ifndef VIPDIALOG_H
#define VIPDIALOG_H

#include <QDialog>

#include "ParkSystemManager.h"
#include "ParkSystemWorker.h"
#include "SQL_Test.h"
namespace Ui {
class VIPDialog;
}

class VIPDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VIPDialog(QWidget *parent = nullptr);
    ~VIPDialog();
    //ParkSystemWorker worker;

    QString plateNumber;
private:
    Ui::VIPDialog *ui;
private slots:
    void on_slotChooseYear();
    void on_payYesBtn_clicked();
};

#endif // VIPDIALOG_H
