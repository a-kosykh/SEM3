#include "mainwindow.h"
#include "ui_mainwindow.h"

#define NUM_OF_COLOURS 5

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QBrush brush(Qt::white);
    if (ui->brushColorBox->currentIndex() == (int)userColor::blackCol){
            brush.setColor(Qt::black);
        }
    if (ui->brushColorBox->currentIndex() == (int)userColor::redCol){
            brush.setColor(Qt::red);
        }
    if (ui->brushColorBox->currentIndex() == (int)userColor::blue){
            brush.setColor(Qt::blue);
        }
    if (ui->brushColorBox->currentIndex() == (int)userColor::white){
            brush.setColor(Qt::white);
        }
    if (ui->brushColorBox->currentIndex() == (int)userColor::yellow){
            brush.setColor(Qt::yellow);
        }

    QPen pen;
    pen.setWidth(ui->penSizBox->text().toInt());
    if (ui->penColorBox->currentIndex() == (int)userColor::blackCol){
            pen.setColor(Qt::black);
        }
    if (ui->penColorBox->currentIndex() == (int)userColor::redCol){
            pen.setColor(Qt::red);
        }
    if (ui->penColorBox->currentIndex() == (int)userColor::blue){
            pen.setColor(Qt::blue);
        }
    if (ui->penColorBox->currentIndex() == (int)userColor::white){
            pen.setColor(Qt::white);
        }
    if (ui->penColorBox->currentIndex() == (int)userColor::yellow){
            pen.setColor(Qt::yellow);
        }

    if (ui->objSelBox->currentIndex() == 0){
        ellipse = scene->addEllipse(10,10,100,100,pen,brush);
        ellipse->setFlag(QGraphicsItem::ItemIsSelectable);
        ellipse->setFlag(QGraphicsItem::ItemIsMovable);
    }
    if (ui->objSelBox->currentIndex() == 1){
        QPolygonF poly;
        poly << QPointF(10.4, 100) << QPointF(200, 30.2) << QPointF(23, 300);
        polygon = scene->addPolygon(poly,pen,brush);
        polygon->setFlag(QGraphicsItem::ItemIsSelectable);
        polygon->setFlag(QGraphicsItem::ItemIsMovable);
    }
    if (ui->objSelBox->currentIndex() == 2){
        rect = scene->addRect(100,200,100,100,pen,brush);
        rect->setFlag(QGraphicsItem::ItemIsSelectable);
        rect->setFlag(QGraphicsItem::ItemIsMovable);
    }
    if (ui->objSelBox->currentIndex() == 3){
        line = scene->addLine(10, 43, 54, 100, pen);
        line->setFlag(QGraphicsItem::ItemIsSelectable);
        line->setFlag(QGraphicsItem::ItemIsMovable);
    }

}

void MainWindow::on_drawText_clicked()
{
    textItem = scene->addSimpleText(ui->textToDraw->toPlainText());
    textItem->setFlag(QGraphicsItem::ItemIsSelectable);
    textItem->setFlag(QGraphicsItem::ItemIsMovable);
}

void MainWindow::on_rotateButton_clicked()
{
    foreach (QGraphicsItem *it, scene->selectedItems()) {
        it->setRotation(ui->rotateGrad->toPlainText().toInt());
    }
}
void MainWindow::wheelEvent(QWheelEvent *event){

        ui->graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        double scaleFactor = 1.15;
        if(event->delta() > 0) {
            ui->graphicsView-> scale(scaleFactor, scaleFactor);

        } else {
             ui->graphicsView->scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        }
}

void MainWindow::on_layerDown_clicked()
{
    foreach (QGraphicsItem *it, scene->selectedItems()) {
        it->setZValue(-1);
    }
}

void MainWindow::on_layerUp_clicked()
{
    foreach (QGraphicsItem *it, scene->selectedItems()) {
        it->setZValue(1);
    }
}

void MainWindow::on_saveButton_clicked()
{
    QImage img(581,461,QImage::Format_ARGB32_Premultiplied);
    QPainter p(&img);
    scene->render(&p);
    p.end();
    img.save("scene.png");
}
