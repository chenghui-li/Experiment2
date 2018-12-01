#include "adminwindow.h"
#include <QApplication>
#include "logindialog.h"
#include "workerwindow.h"
#include "QRadioButton"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LoginDialog ldg;
    WorkerWindow ww;
    AdminWindow aw;
    if(ldg.exec() == QDialog::Accepted ){
        if(ldg.isAdmin == true)
            aw.show();
        else
            ww.show();
    }
    return a.exec();
}
