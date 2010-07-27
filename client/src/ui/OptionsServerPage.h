#ifndef OPTIONSSERVERPAGE_H
#define OPTIONSSERVERPAGE_H

#include <QWidget>

class QTableView;
class QLineEdit;

class OptionsServerPage : public QWidget
{
    Q_OBJECT

public:
    explicit OptionsServerPage(QWidget *parent = 0);

private slots:
    void currentServerChanged();

    void addNewServer();
    void deleteServer();
    void saveChanges();

private:
    QTableView *m_serversView;
    QLineEdit *m_nameEdit, *m_hostnameEdit, *m_usernameEdit, *m_passwordEdit;
};

#endif // OPTIONSSERVERPAGE_H
