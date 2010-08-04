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

protected:
    virtual void closeEvent(QCloseEvent *event);

private:
    QTabWidget *m_tabWidget;
};

class OptionsDialogPage : public QWidget
{
    Q_OBJECT

public:
    OptionsDialogPage(QWidget *parent = 0) : QWidget(parent) { }

    virtual bool hasUnsavedChanges() const = 0;

public slots:
    virtual void saveChanges() = 0;
};

#endif // OPTIONSDIALOG_H
