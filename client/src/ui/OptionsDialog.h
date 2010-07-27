#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>

class QTabWidget;

class OptionsDialog : public QDialog
{
    Q_OBJECT

public:
    enum OptionsPage
    {
        GeneralPage = 0,
        ServerPage
    };

    explicit OptionsDialog(QWidget *parent = 0);

    void showPage(OptionsPage page);
    QWidget *pageWidget(OptionsPage page) const;

private:
    QTabWidget *m_tabWidget;
};

#endif // OPTIONSDIALOG_H
