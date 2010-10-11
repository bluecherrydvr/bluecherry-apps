#ifndef OPTIONSSERVERPAGE_H
#define OPTIONSSERVERPAGE_H

#include "OptionsDialog.h"

class QTreeView;
class QLineEdit;
class DVRServer;
class QModelIndex;
class QLabel;

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

    void setLoginSuccessful();
    void setLoginConnecting();
    void setLoginError(const QString &message);

private:
    QTreeView *m_serversView;
    QLabel *m_connectionStatus;
    QLineEdit *m_nameEdit, *m_hostnameEdit, *m_usernameEdit, *m_passwordEdit;
};

#endif // OPTIONSSERVERPAGE_H
