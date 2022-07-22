#include "licensedialog.h"

#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QFormLayout>

LicenseDialog::LicenseDialog(QWidget *parent): QDialog(parent)
{
    init();
}

LicenseDialog::LicenseDialog(QWidget *parent, const QString &appId, const QString& sdkKey): QDialog(parent) {
    this->appId = appId;
    this->sdkKey = sdkKey;
    init();
}

void LicenseDialog::init() {
    QFormLayout *lytMain = new QFormLayout(this);

    QLabel *titleLabel = new QLabel(tr("please_input_arcsoft_license"), this);
    lytMain->addRow(titleLabel);
    QLabel *tLabel = new QLabel(tr("arcsoft_app_id"), this);
    QLineEdit *tLine = new QLineEdit(this);
    tLine->setText(appId);
    lytMain->addRow(tLabel, tLine);
    fields << tLine;
    QLabel *tLabel2 = new QLabel(tr("arcsoft_sdk_key"), this);
    QLineEdit *tLine2 = new QLineEdit(this);
    tLine2->setText(sdkKey);
    lytMain->addRow(tLabel2, tLine2);
    fields << tLine2;


    QDialogButtonBox *buttonBox = new QDialogButtonBox
            ( QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
              Qt::Horizontal, this );
    lytMain->addWidget(buttonBox);

    bool conn = connect(buttonBox, &QDialogButtonBox::accepted,
                   this, &LicenseDialog::accept);
    Q_ASSERT(conn);
    conn = connect(buttonBox, &QDialogButtonBox::rejected,
                   this, &LicenseDialog::reject);
    Q_ASSERT(conn);

    setLayout(lytMain);
}
QStringList LicenseDialog::getStrings(QWidget *parent, const QString &appId, const QString& sdkKey, bool *ok)
{
    LicenseDialog *dialog = new LicenseDialog(parent, appId, sdkKey);

    QStringList list;

    const int ret = dialog->exec();
    if (ok)
        *ok = !!ret;

    if (ret) {
        foreach (auto field, dialog->fields) {
            list << field->text();
        }
    }

    dialog->deleteLater();

    return list;
}
