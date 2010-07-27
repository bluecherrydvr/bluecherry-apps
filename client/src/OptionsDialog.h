#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>

class QTabWidget;

class OptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionsDialog(QWidget *parent = 0);

private:
    QTabWidget *m_tabWidget;
};

#endif // OPTIONSDIALOG_H
