#include "workerwindow.h"
#include "ui_workerwindow.h"
#include "QFileDialog"
#include "vipdialog.h"
#include "QPixmap"
#include "ParkSystemManager.h"
#include "ParkSystemWorker.h"
#include "SQL_Test.h"
#include "QMessageBox"
WorkerWindow::WorkerWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WorkerWindow)
{
    ui->setupUi(this);
    this->image = new QImage();
}

WorkerWindow::~WorkerWindow()
{
    delete ui;
}

void WorkerWindow::on_recoBtn_clicked()
{
    //调用识别函数，返回识别到的字符串
    //QString plateNumber = ui->numberEdit->text();
    ui->numberEdit->clear();
    ui->recoLabel->clear();
    ParkSystemWorker worker;
    worker.init();
    QString str = QString::fromStdString(worker.recognizePlate(filename.toStdString()));
    ui->recoLabel->setText(str);
    ui->numberEdit->setText(str);
}

void WorkerWindow::on_submitBtn_clicked()
{
    QString finalNumber = ui->numberEdit->text();
    //插入数据库
    ParkSystemWorker worker;
    worker.init();
    plateNumber = finalNumber;
    bool in;
    vector<string> v;
    v = worker.submit(finalNumber.toStdString(),in);
    if(v.size()>0){
    if(in == true){
        //车辆入库弹窗
        string msg = "车辆入库成功\n";
        msg += "是否会员： ";
        msg += v.at(0);
        msg += "\n";
        msg += "会员到期时间：";
        msg += v.at(1);
        QMessageBox::warning(this,tr("提示"),tr(msg.c_str()),QMessageBox::Yes);
    }else{
        //车辆出库弹窗
        string msg = "车辆出库成功\n";
        string intime = "车辆驶入时间： ";
        string outtime = "车辆驶出时间： ";
        string lasttime = "停车时间： ";
        string price = "收费金额： ";
        string isvip = "是否会员： ";
        string vipdiscount = "会员折扣： ";
        msg = msg + intime + v.at(0) + "\n" +
                outtime + v.at(1) + "\n" +
                lasttime + v.at(2) +"\n" +
                price + v.at(3) + "\n" +
                isvip + v.at(4) + "\n" +
                vipdiscount + v.at(5);
        QMessageBox::warning(this,tr("提示"),tr(msg.c_str()),QMessageBox::Yes);
    }
    }
    else{
        QMessageBox::warning(this,tr("提示"),tr("查询异常"),QMessageBox::Yes);
    }
    //ui->recoLabel->setText("插入数据库成功！");
    ui->recoLabel->clear();
    ui->numberEdit->clear();
}

void WorkerWindow::on_toVIPBtn_clicked()
{
    VIPDialog *vipdlg = new VIPDialog();
    vipdlg->plateNumber = ui->numberEdit->text();
    vipdlg->show();
}

void WorkerWindow::on_choosePctBtn_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(
                    this, "open image file",
                    ".",
                    "Image files (*.bmp *.jpg *.pbm *.pgm *.png *.ppm *.xbm *.xpm);;All files (*.*)");
    if(fileName != ""){
        if(image->load(fileName)){
            QGraphicsScene *scene = new QGraphicsScene;
            int width = ui->pictureView->width();
            int height = ui->pictureView->height();
            QPixmap pixmap = QPixmap::fromImage(*image);
            QPixmap nowmap = pixmap.scaled(width,height);
            scene->addPixmap(nowmap);
            //ui->recoLabel->setText(fileName);
            filename = fileName;
            ui->pictureView->setScene(scene);
            ui->pictureView->show();
        }
    }
}
