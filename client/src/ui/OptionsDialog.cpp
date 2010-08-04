#include "OptionsDialog.h"
#include "OptionsServerPage.h"
#include <QBoxLayout>
#include <QTabWidget>
#include <QMessageBox>
#include <QCloseEvent>

OptionsDialog::OptionsDialog(QWidget *parent)
    : QDialog(parent)
{
    setModal(true);

    QBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 3, 0, 0);

    m_tabWidget = new QTabWidget;
    m_tabWidget->setDocumentMode(true);
    layout->addWidget(m_tabWidget);

    m_tabWidget->addTab(new QWidget, tr("General"));
    m_tabWidget->addTab(new OptionsServerPage(this), tr("DVR Servers"));
}

void OptionsDialog::showPage(OptionsPage page)
{
    m_tabWidget->setCurrentIndex((int)page);
}

QWidget *OptionsDialog::pageWidget(OptionsPage page) const
{
    return m_tabWidget->widget((int)page);
}

void OptionsDialog::closeEvent(QCloseEvent *event)
{
    bool saveChanges = false;

    /* Prompt the user to save changes if any are unsaved, but prompt only once */
    for (int i = 0; i < m_tabWidget->count(); ++i)
    {
        OptionsDialogPage *pageWidget = qobject_cast<OptionsDialogPage*>(m_tabWidget->widget(i));
        if (!pageWidget)
            continue;

        if (pageWidget->hasUnsavedChanges())
        {
            if (saveChanges)
            {
                pageWidget->saveChanges();
                continue;
            }

            QMessageBox msg(QMessageBox::Question, tr("Options"), tr("Do you want to save your changes?"),
                            QMessageBox::NoButton, this);

            msg.addButton(QMessageBox::Save);
            msg.addButton(QMessageBox::Discard);
            msg.addButton(QMessageBox::Cancel);

            switch (msg.exec())
            {
            case QMessageBox::Save:
                saveChanges = true;
                pageWidget->saveChanges();
                break;
            case QMessageBox::Discard:
                event->accept();
                return;
            case QMessageBox::Cancel:
                event->ignore();
                return;
            }
        }
    }

    event->accept();
}
