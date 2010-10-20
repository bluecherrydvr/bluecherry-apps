#include "OptionsServerPage.h"
#include "DVRServersModel.h"
#include "core/BluecherryApp.h"
#include "core/DVRServer.h"
#include <QTreeView>
#include <QHeaderView>
#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextDocument>
#include <QMessageBox>
#include <QIntValidator>

OptionsServerPage::OptionsServerPage(QWidget *parent)
    : OptionsDialogPage(parent)
{
    QBoxLayout *mainLayout = new QVBoxLayout(this);
    QBoxLayout *topLayout = new QHBoxLayout;
    mainLayout->addLayout(topLayout);

    /* Servers list */
    m_serversView = new QTreeView;
    m_serversView->setModel(new DVRServersModel(m_serversView));
    m_serversView->header()->setHighlightSections(false);
    m_serversView->header()->setResizeMode(QHeaderView::ResizeToContents);
    m_serversView->header()->setResizeMode(0, QHeaderView::Stretch);
    m_serversView->header()->setStretchLastSection(false);
    m_serversView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_serversView->setMinimumSize(480, 150);
    m_serversView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_serversView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_serversView->setItemsExpandable(false);
    m_serversView->setRootIsDecorated(false);
    topLayout->addWidget(m_serversView);

    connect(m_serversView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            SLOT(currentServerChanged(QModelIndex,QModelIndex)));

    /* Editing area */
    QGridLayout *editsLayout = new QGridLayout;
    mainLayout->addLayout(editsLayout);

    QLabel *label = new QLabel(tr("Name:"));
    editsLayout->addWidget(label, 0, 0, Qt::AlignRight);

    m_nameEdit = new QLineEdit;
    editsLayout->addWidget(m_nameEdit, 0, 1);

    label = new QLabel(tr("Hostname:"));
    editsLayout->addWidget(label, 1, 0, Qt::AlignRight);

    QBoxLayout *hnLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    m_hostnameEdit = new QLineEdit;
    hnLayout->addWidget(m_hostnameEdit);

    m_portEdit = new QLineEdit;
    m_portEdit->setValidator(new QIntValidator(1, 65535, m_portEdit));
    m_portEdit->setFixedWidth(50);
    hnLayout->addWidget(m_portEdit);

    editsLayout->addLayout(hnLayout, 1, 1);

    label = new QLabel(tr("Username:"));
    editsLayout->addWidget(label, 0, 2, Qt::AlignRight);

    m_usernameEdit = new QLineEdit;
    editsLayout->addWidget(m_usernameEdit, 0, 3);

    label = new QLabel(tr("Password:"));
    editsLayout->addWidget(label, 1, 2, Qt::AlignRight);

    m_passwordEdit = new QLineEdit;
    m_passwordEdit->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    editsLayout->addWidget(m_passwordEdit, 1, 3);

    /* Errors */
    m_connectionStatus = new QLabel;
    m_connectionStatus->setAlignment(Qt::AlignCenter);
    m_connectionStatus->setContentsMargins(4, 4, 4, 4);
    m_connectionStatus->setVisible(false);
    mainLayout->addWidget(m_connectionStatus);

    /* Buttons */
    QFrame *line = new QFrame;
    line->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    mainLayout->addWidget(line);

    QBoxLayout *btnLayout = new QHBoxLayout;
    mainLayout->addLayout(btnLayout);

    QPushButton *newBtn = new QPushButton(tr("Add Server"));
    newBtn->setAutoDefault(false);
    connect(newBtn, SIGNAL(clicked()), SLOT(addNewServer()));
    btnLayout->addWidget(newBtn);

    QPushButton *delBtn = new QPushButton(tr("Delete"));
    delBtn->setAutoDefault(false);
    connect(delBtn, SIGNAL(clicked()), SLOT(deleteServer()));
    btnLayout->addWidget(delBtn);

    btnLayout->addStretch();
    QPushButton *applyBtn = new QPushButton(tr("Apply"));
    applyBtn->setAutoDefault(false);
    connect(applyBtn, SIGNAL(clicked()), SLOT(saveChanges()));
    btnLayout->addWidget(applyBtn);
}

void OptionsServerPage::setCurrentServer(DVRServer *server)
{
    QModelIndex index = static_cast<DVRServersModel*>(m_serversView->model())->indexForServer(server);
    if (index.isValid())
        m_serversView->setCurrentIndex(index);
}

void OptionsServerPage::currentServerChanged(const QModelIndex &newIndex, const QModelIndex &oldIndex)
{
    DVRServer *server = static_cast<DVRServersModel*>(m_serversView->model())->
                        serverForRow(oldIndex);
    if (server)
    {
        saveChanges(server);
        server->api->disconnect(this);
        m_connectionStatus->setVisible(false);
    }

    server = static_cast<DVRServersModel*>(m_serversView->model())->serverForRow(newIndex);
    if (!server)
    {
        m_nameEdit->clear();
        m_hostnameEdit->clear();
        m_portEdit->clear();
        m_usernameEdit->clear();
        m_passwordEdit->clear();
        return;
    }

    m_nameEdit->setText(server->displayName());
    m_hostnameEdit->setText(server->readSetting("hostname").toString());
    m_portEdit->setText(QString::number(server->serverPort()));
    m_usernameEdit->setText(server->readSetting("username").toString());
    m_passwordEdit->setText(server->readSetting("password").toString());

    connect(server->api, SIGNAL(loginSuccessful()), SLOT(setLoginSuccessful()));
    connect(server->api, SIGNAL(loginError(QString)), SLOT(setLoginError(QString)));
    connect(server->api, SIGNAL(loginRequestStarted()), SLOT(setLoginConnecting()));

    if (server->api->isOnline())
        setLoginSuccessful();
    else if (server->api->isLoginPending())
        setLoginConnecting();
    else if (!server->api->errorMessage().isEmpty())
        setLoginError(server->api->errorMessage());
}

void OptionsServerPage::addNewServer()
{
    DVRServer *server = bcApp->addNewServer(tr("New Server"));
    if (!m_serversView->currentIndex().isValid())
        saveChanges(server);
    setCurrentServer(server);

    m_nameEdit->setFocus();
    m_nameEdit->selectAll();
}

void OptionsServerPage::deleteServer()
{
    DVRServer *server = static_cast<DVRServersModel*>(m_serversView->model())->
                        serverForRow(m_serversView->currentIndex());

    if (!server)
        return;

    QMessageBox dlg(QMessageBox::Question, tr("Delete DVR Server"), tr("Are you sure you want to delete <b>%1</b>?")
                    .arg(Qt::escape(server->displayName())), QMessageBox::NoButton, this);
    QPushButton *delBtn = dlg.addButton(tr("Delete"), QMessageBox::DestructiveRole);
    dlg.addButton(QMessageBox::Cancel);
    dlg.exec();

    if (dlg.clickedButton() != delBtn)
        return;

    server->removeServer();
}

bool OptionsServerPage::hasUnsavedChanges() const
{
    return m_nameEdit->isModified() || m_hostnameEdit->isModified()
            || m_usernameEdit->isModified() || m_passwordEdit->isModified();
}

void OptionsServerPage::saveChanges()
{
    saveChanges(0);
}

void OptionsServerPage::saveChanges(DVRServer *server)
{
    if (!server)
    {
        server = static_cast<DVRServersModel*>(m_serversView->model())->
                 serverForRow(m_serversView->currentIndex());
        if (!server)
            return;
    }

    bool connectionModified = false;

    if (m_nameEdit->isModified())
    {
        server->setDisplayName(m_nameEdit->text().trimmed());
        m_nameEdit->setModified(false);
    }
    if (m_hostnameEdit->isModified())
    {
        server->writeSetting("hostname", m_hostnameEdit->text());
        m_hostnameEdit->setModified(false);
        connectionModified = true;
    }
    if (m_portEdit->isModified())
    {
        server->writeSetting("port", m_portEdit->text());
        m_portEdit->setModified(false);
        connectionModified = true;
    }
    if (m_usernameEdit->isModified())
    {
        server->writeSetting("username", m_usernameEdit->text());
        m_usernameEdit->setModified(false);
        connectionModified = true;
    }
    if (m_passwordEdit->isModified())
    {
        server->writeSetting("password", m_passwordEdit->text());
        m_passwordEdit->setModified(false);
        connectionModified = true;
    }

    if (connectionModified)
        server->login();
}

void OptionsServerPage::setLoginSuccessful()
{
    m_connectionStatus->setVisible(false);
}

void OptionsServerPage::setLoginConnecting()
{
    m_connectionStatus->setText(tr("Connecting to server..."));
    m_connectionStatus->setVisible(true);
}

void OptionsServerPage::setLoginError(const QString &message)
{
    m_connectionStatus->setText(tr("<b>Login error:</b> %1").arg(Qt::escape(message)));
    m_connectionStatus->setVisible(true);
}
