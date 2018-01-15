#include "history.h"
#include "ui_history.h"
#include "mainwindow.h"

#include <QThread>

history::history(QStringList historyList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::history)
{
    ui->setupUi(this);
    ui->listWidget->addItems(historyList);

}

history::~history()
{
    delete ui;
}

void history::on_restoreButton_clicked()
{
    QString storyCalc = ui->listWidget->currentItem()->text();
    if (storyCalc.endsWith(" (hex)") || storyCalc.endsWith(" (oct)")){
        storyCalc.chop(6);
    }
    QString operators = "+-*/";
    unsigned int i;
    for (i = 0; i < operators.size(); ++i) {
        int oper = storyCalc.indexOf(operators.at(i));
        if (oper != -1){
            break;
        }
    }

    QString a = storyCalc.mid(0,storyCalc.indexOf(operators.at(i)));

    QString oper = operators.at(i);

    QString b = storyCalc.remove(0,a.size()+1);
    b = b.mid(0,storyCalc.indexOf("="));

    QStringList sl;
    sl.append(a);
    sl.append(oper);
    sl.append(b);

    emit sendData(sl);
}

void history::on_pushButton_clicked()
{
    QString stringToFind = ui->textEdit->toPlainText();
    for (unsigned int i = 0; i < ui->listWidget->count(); ++i){
        QListWidgetItem *item = ui->listWidget->item(i);
        if (item->text().contains(stringToFind)){
            item->setSelected(i);
        }
    }
}
