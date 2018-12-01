#include "logindialog.h"
#include "ui_logindialog.h"
#include "QRadioButton"
#include "QButtonGroup"
#include "QMessageBox"
#include "ParkSystemManager.h"
#include "ParkSystemWorker.h"
#include "SQL_Test.h"
LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    qbg.addButton(ui->adRadioButton);
    qbg.addButton(ui->wkRadioButton);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}
bool LoginDialog::check(bool isAdmin,QString userid,QString password){
    bool ans = false;

    if(isAdmin == true){
        //查找管理员数据库
        ParkSystemManager admin;
        admin.init();
        if(admin.login(userid.toStdString(),password.toStdString()) == true)
        //if(!QString::compare(userid,"000") && !QString::compare(password,"000"))
            ans = true;
    }else{
        //查找业务员数据库
        ParkSystemWorker worker;
        worker.init();
        if(worker.login(userid.toStdString(),password.toStdString()) == true)
        //if(!QString::compare(userid,"111") && !QString::compare(password,"111"))
            ans = true;
    }
    return ans;
}
void LoginDialog::on_loginBtn_clicked()
{
    QRadioButton* pbtn = qobject_cast<QRadioButton*>(qbg.checkedButton());
    QString name = pbtn->objectName();
    userid = ui->usrEdit->text().trimmed();
    password = ui->pwdEdit->text();
    if(!QString::compare(name,"adRadioButton") && check(true,userid,password) == true){
        isAdmin = true;
        accept();
    }else if(!QString::compare(name,"wkRadioButton") && check(false,userid,password) == true){
        isAdmin = false;
        accept();
    }else{
        QMessageBox::warning(this,tr("warning"),tr("用户名或密码错误"),QMessageBox::Yes);
        ui->usrEdit->clear();
        ui->pwdEdit->clear();
        ui->usrEdit->setFocus();
    }
}
