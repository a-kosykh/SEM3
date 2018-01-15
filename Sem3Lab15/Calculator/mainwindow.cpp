#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "history.h"

QStringList historyList;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    double a = ui->spinBox->value();
    double b = ui->spinBox_2->value();
    double res = 0;
    bool invalid = false;
    QString data4List;
    QString operand;

    if (ui->comboBox->currentText() == "+"){
        res = a + b;
        operand = "+";
    }
    if (ui->comboBox->currentText() == "-"){
        res = a - b;
        operand = "-";
    }
    if (ui->comboBox->currentText() == "*"){
        res = a * b;
        operand = "*";
    }
    if (ui->comboBox->currentText() == "/"){
        if (b == 0){
            invalid = true;
        }
        else {
            res = a / b;
        }
        operand = "/";
    }

    QString aString = QString::number(a);
    QString bString = QString::number(b);
    data4List = aString + operand + bString;

    QString resString;
    if (!invalid) {
        if (ui->radix10->isChecked()) {
            ui->textBrowser->setText(QString::number(res));
            resString = QString::number(res);
        }
        if (ui->radix16->isChecked()){
            ui->textBrowser->setText(QString("%1").arg((int)res,0,16));
            resString = QString("%1").arg((int)res,0,16) + " (hex)";
        }
        if (ui->radix8->isChecked()){
            ui->textBrowser->setText(QString("%1").arg((int)res,0,8));
            resString = QString("%1").arg((int)res,0,8) + " (oct)";
        }
        if (!(ui->radix8->isChecked()) && !(ui->radix10->isChecked())
                && !(ui->radix16->isChecked())){
            ui->textBrowser->setText(QString::number(res));
            resString = QString::number(res);
        }
    }
    else {
        ui->textBrowser->setText("inf");
        resString = "inf";
    }
    data4List += "=" + resString;
    historyList.push_back(data4List);
}

void MainWindow::on_pushButton_2_clicked()
{
    history* hist;
    hist = new history(historyList);
    hist->show();

    connect(hist,SIGNAL(sendData(QStringList)),this,SLOT(receiveData(QStringList)));
}

void MainWindow::on_spinBox_valueChanged(int arg1)
{
    if(ui->instantRes->isChecked()){
        MainWindow::on_pushButton_clicked();
    }
}

void MainWindow::on_spinBox_2_valueChanged(int arg1)
{
    if(ui->instantRes->isChecked()){
        MainWindow::on_pushButton_clicked();
    }
}
void MainWindow::receiveData(QStringList sl) {
    int a = sl.at(0).toInt();
    int b = sl.at(2).toInt();
    ui->spinBox->setValue(a);

    if (sl.at(1) == "+"){
        ui->comboBox->setCurrentIndex(0);
    }
    if (sl.at(1) == "-"){
        ui->comboBox->setCurrentIndex(1);
    }
    if (sl.at(1) == "*"){
        ui->comboBox->setCurrentIndex(2);
    }
    if (sl.at(1) == "/"){
        ui->comboBox->setCurrentIndex(3);
    }

    ui->spinBox_2->setValue(b);
}
