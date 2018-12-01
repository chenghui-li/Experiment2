#ifndef WORKERWINDOW_H
#define WORKERWINDOW_H

#include <QMainWindow>

#include "ParkSystemManager.h"
#include "ParkSystemWorker.h"
#include "SQL_Test.h"
namespace Ui {
class WorkerWindow;
}

class WorkerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit WorkerWindow(QWidget *parent = nullptr);
    ~WorkerWindow();
    QString filename;
    QString plateNumber;
private:
    Ui::WorkerWindow *ui;

    QImage *image;
private slots:

    void on_recoBtn_clicked();
    void on_submitBtn_clicked();
    void on_toVIPBtn_clicked();
    void on_choosePctBtn_clicked();
};

#endif // WORKERWINDOW_H
