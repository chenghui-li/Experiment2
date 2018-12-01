#include "adminwindow.h"
#include "ui_adminwindow.h"
#include "QStandardItemModel"
#include "ParkSystemManager.h"
#include "ParkSystemWorker.h"
#include "SQL_Test.h"
void initModel(QStandardItemModel *model){
    model->setColumnCount(7);

    model->setHeaderData(0,Qt::Horizontal,QString::fromLocal8Bit("车牌号"));

    model->setHeaderData(1,Qt::Horizontal,QString::fromLocal8Bit("驶入时刻"));

    model->setHeaderData(2,Qt::Horizontal,QString::fromLocal8Bit("驶出时刻"));

    model->setHeaderData(3,Qt::Horizontal,QString::fromLocal8Bit("停车时间"));

    model->setHeaderData(4,Qt::Horizontal,QString::fromLocal8Bit("驶入办理人员"));

    model->setHeaderData(5,Qt::Horizontal,QString::fromLocal8Bit("驶出办理人员"));

    model->setHeaderData(6,Qt::Horizontal,QString::fromLocal8Bit("收费"));
}

AdminWindow::AdminWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AdminWindow)
{
    ui->setupUi(this);
    model = new QStandardItemModel();

    initModel(model);

    ui->tableView->setModel(model);

    ParkSystemManager admin;
    admin.init();
    nowDiscount = QString("%1").arg(admin.getCurrentDiscount());
    nowVIPPrice = QString("%1").arg(admin.getCurrentPrice());
    nowPrice = QString("%1").arg(admin.getParkingPrice());
    ui->nowVIPPriceLabel->setText(nowVIPPrice);
    ui->nowDiscountLabel->setText(nowDiscount);
    ui->nowPriceLabel->setText(nowPrice);
}
/*
string 转 qstring ：QString::fromStdString(str)
qstring -> string: qstr.toStdString();

qstring -> float: qstr.toFloat();
float -> qstring: QString("%1).arg(floatnum)

*/
AdminWindow::~AdminWindow()
{
    delete ui;
}


void AdminWindow::on_submitBtn_clicked()
{
    ParkSystemManager admin;
    admin.init();
    QString begTime = ui->begDateEdit->text();
    QString endTime = ui->endDateEdit->text();

    //QVector<QVector<QString>> list;
    vector<vector<string>> list;
    QStringList begDateList = begTime.split("/");
    QStringList endDateList = endTime.split("/");
    list = admin.checkSummaryInfo(begDateList.at(0).toStdString(),
                                  endDateList.at(0).toStdString(),
                                  begDateList.at(1).toStdString(),
                                  endDateList.at(1).toStdString(),
                                  begDateList.at(2).toStdString(),
                                  endDateList.at(2).toStdString());

    QString carNum = QString::fromStdString(list.at(0).at(1));
    QString allIncome = QString::fromStdString(list.at(0).at(0));
    QString vipSumPrice = QString::fromStdString(list.at(0).at(2));
    ui->carNumLabel->setText(carNum);
    ui->allIncomeLabel->setText(allIncome);
    ui->vipSumPriceLabel->setText(vipSumPrice);
    QStandardItemModel *nowmodel = new QStandardItemModel();
    initModel(nowmodel);
    for(int i = 1;i<list.size();i++){
        for(int j = 0;j<7;j++){
            nowmodel->setItem(i-1,j,new QStandardItem(QString::fromStdString(list.at(i).at(j))));
        }
    }
    ui->tableView->setModel(nowmodel);
}

void AdminWindow::on_changeDiscountBtn_clicked()
{
    QString newDiscount = ui->newDiscountEdit->text();
    nowDiscount = newDiscount;
    ParkSystemManager admin;
    admin.init();
    admin.modifyVIPDiscount(newDiscount.toFloat());
    ui->nowDiscountLabel->setText(nowDiscount);
    ui->newDiscountEdit->clear();
}

void AdminWindow::on_changePriceBtn_clicked()
{
    QString newPrice = ui->newPriceEdit->text();
    nowPrice = newPrice;
    ParkSystemManager admin;
    admin.init();
    admin.modifyParkingPrice(newPrice.toFloat());
    ui->nowPriceLabel->setText(nowPrice);
    ui->newPriceEdit->clear();
}

void AdminWindow::on_changeVIPPriceBtn_clicked()
{
    QString newVIPPrice = ui->newVIPPriceEdit->text();
    nowVIPPrice = newVIPPrice;
    ParkSystemManager admin;
    admin.init();
    admin.modifyVIPPrice(newVIPPrice.toFloat());
    ui->nowVIPPriceLabel->setText(nowVIPPrice);
    ui->newVIPPriceEdit->clear();
}
