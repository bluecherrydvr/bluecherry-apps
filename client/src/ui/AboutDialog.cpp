#include "AboutDialog.h"
#include <QGridLayout>
#include <QPlainTextEdit>
#include <QLabel>
#include <QFile>
#include <QApplication>

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Bluecherry DVR - About"));
    setFixedSize(500, 400);
    setModal(true);

    QGridLayout *layout = new QGridLayout(this);

    QLabel *logo = new QLabel;
    logo->setPixmap(QPixmap(QLatin1String(":/images/logo.png"))
                    .scaled(130, 130, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    logo->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    layout->addWidget(logo, 0, 0);

    QLabel *text = new QLabel;
    text->setText(tr("Bluecherry DVR<br>Version %1").arg(QApplication::applicationVersion()));
    text->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    QFont font = text->font();
    font.setPixelSize(15);
    text->setFont(font);

    layout->addWidget(text, 0, 1);
    layout->setColumnStretch(1, 1);

    QPlainTextEdit *license = new QPlainTextEdit(getLicenseText());
    license->setReadOnly(true);
    license->setTabChangesFocus(true);
    layout->addWidget(license, 1, 0, 1, 2);

    font = QFont();
    font.setStyleHint(QFont::SansSerif);
    font.setPixelSize(13);
    license->setFont(font);
}

QString AboutDialog::getLicenseText() const
{
    QFile file(QLatin1String(":/license.txt"));
    if (!file.open(QIODevice::ReadOnly))
        return QString();

    return QString::fromUtf8(file.readAll());
}
