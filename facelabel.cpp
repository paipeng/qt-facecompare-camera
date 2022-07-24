#include "facelabel.h"
#include <QPainter>
#include <QGuiApplication>

FaceLabel::FaceLabel(QWidget *parent):QLabel(parent)
{

}

void FaceLabel::setInfo(const QString &info) {
    this->info = info;
}

void FaceLabel::paintEvent(QPaintEvent* event) {
    QLabel::paintEvent(event);
#if 1
    QPainter painter(this);
    painter.setPen(QPen(Qt::green, 2));
    //QBrush brush(Qt::green, Qt::SolidPattern);
    //painter.setBrush(brush);
    QFont font = QGuiApplication::font();
    font.setPointSize(10);
    painter.setFont(font);
    painter.drawText(20,this->height() - 10, info);
#endif
}
