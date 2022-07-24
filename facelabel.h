#ifndef FACELABEL_H
#define FACELABEL_H

#include <QLabel>

class FaceLabel : public QLabel
{
    Q_OBJECT
public:
    FaceLabel(QWidget *parent = nullptr);

    void setInfo(const QString &info);
protected:
    void paintEvent(QPaintEvent* event) override;
private:
    QString info;
};

#endif // FACELABEL_H
