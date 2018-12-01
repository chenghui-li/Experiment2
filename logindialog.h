#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "QButtonGroup"

#include "ParkSystemManager.h"
#include "ParkSystemWorker.h"
#include "SQL_Test.h"
namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    bool check(bool,QString,QString);    //验证身份
    ~LoginDialog();
    bool isAdmin;
    QString userid;
    QString password;
private slots:
    void on_loginBtn_clicked();

private:
    Ui::LoginDialog *ui;

    QButtonGroup qbg;
};

#endif // LOGINDIALOG_H
