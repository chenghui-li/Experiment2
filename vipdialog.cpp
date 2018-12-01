
#include <iostream>
#include "vipdialog.h"
#include "ui_vipdialog.h"
#include "ParkSystemManager.h"
#include "ParkSystemWorker.h"
#include "SQL_Test.h"
using namespace std;
VIPDialog::VIPDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VIPDialog)
{
    ui->setupUi(this);
    connect(ui->yearBox,SIGNAL(currentIndexChanged(int)),this,SLOT(on_slotChooseYear()));
    ParkSystemWorker worker;
    if(worker.init() == true){
        QString price = QString("%1").arg(worker.getVIPPrice());
        ui->VIPPriceLabel->setText(price);
        QString discount = QString("%1").arg(worker.getCurrentDiscount());
        ui->discountLabel->setText(discount);

    }
}

VIPDialog::~VIPDialog()
{
    delete ui;
}
void VIPDialog::on_slotChooseYear(){
    int year[] = {0,1,2,3,5};
    int ans = 0;
    int index = ui->yearBox->currentIndex();
    ans = year[index];
    QString str = ui->VIPPriceLabel->text();
    int allprice = str.toInt();
    allprice = allprice*ans;
    ui->finalPriceLabel->setText(QString::number(allprice));
    //ui->discountLabel->setText(QString::number(index));
}

void VIPDialog::on_payYesBtn_clicked()
{
    ParkSystemWorker worker;
    worker.init();
    QString lasttime = ui->yearBox->currentText();
    QString finalPirce = ui->finalPriceLabel->text();
    std::cout<<lasttime.toStdString()<<endl;
    worker.registerMembership(plateNumber.toStdString(),lasttime.toStdString(),finalPirce.toStdString());
    close();
}
