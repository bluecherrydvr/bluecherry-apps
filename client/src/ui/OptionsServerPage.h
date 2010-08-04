#ifndef OPTIONSSERVERPAGE_H
#define OPTIONSSERVERPAGE_H

#include "OptionsDialog.h"

class QTableView;
class QLineEdit;
class DVRServer;
class QModelIndex;

class OptionsServerPage : public OptionsDialogPage
{
    Q_OBJECT

public:
    explicit OptionsServerPage(QWidget *parent = 0);

    virtual bool hasUnsavedChanges() const;
    virtual void saveChanges();
    void saveChanges(DVRServer *server);

public slots:
    void setCurrentServer(DVRServer *server);
    void addNewServer();

private slots:
    void currentServerChanged(const QModelIndex &newIndex, const QModelIndex &oldIndex);

    void deleteServer();

private:
    QTableView *m_serversView;
    QLineEdit *m_nameEdit, *m_hostnameEdit, *m_usernameEdit, *m_passwordEdit;
};

#endif // OPTIONSSERVERPAGE_H
