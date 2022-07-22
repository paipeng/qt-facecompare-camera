#ifndef LICENSEDIALOG_H
#define LICENSEDIALOG_H

#include <QDialog>

class QLineEdit;
class QLabel;

class LicenseDialog : public QDialog
{
    Q_OBJECT
public:
    explicit LicenseDialog(QWidget *parent = nullptr);
    LicenseDialog(QWidget *parent, const QString &appId, const QString& sdkKey);
    static QStringList getStrings(QWidget *parent, const QString &appId, const QString& sdkKey, bool *ok = nullptr);
private:
    void init();
private:
    QList<QLineEdit*> fields;
    QString appId;
    QString sdkKey;
};

#endif // LICENSEDIALOG_H
