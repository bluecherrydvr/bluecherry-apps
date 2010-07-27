#include "OptionsServerPage.h"
#include "DVRServersModel.h"
#include "core/DVRServer.h"
#include <QTableView>
#include <QHeaderView>
#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

OptionsServerPage::OptionsServerPage(QWidget *parent)
    : QWidget(parent)
{
    QBoxLayout *mainLayout = new QVBoxLayout(this);
    QBoxLayout *topLayout = new QHBoxLayout;
    mainLayout->addLayout(topLayout);

    /* Servers list */
    m_serversView = new QTableView;
    m_serversView->setModel(new DVRServersModel(m_serversView));
    m_serversView->verticalHeader()->hide();
    m_serversView->horizontalHeader()->setHighlightSections(false);
    m_serversView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    m_serversView->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    m_serversView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_serversView->setMinimumSize(480, 150);
    m_serversView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_serversView->setSelectionMode(QAbstractItemView::SingleSelection);
    topLayout->addWidget(m_serversView);

    connect(m_serversView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            SLOT(currentServerChanged()));

    /* Buttons (right of servers list) */
    QBoxLayout *btnLayout = new QHBoxLayout;
    mainLayout->addLayout(btnLayout);

    QPushButton *newBtn = new QPushButton(tr("Add Server"));
    connect(newBtn, SIGNAL(clicked()), SLOT(addNewServer()));
    btnLayout->addWidget(newBtn);

    QPushButton *delBtn = new QPushButton(tr("Delete"));
    connect(delBtn, SIGNAL(clicked()), SLOT(deleteServer()));
    btnLayout->addWidget(delBtn);

    btnLayout->addStretch();
    QPushButton *applyBtn = new QPushButton(tr("Apply"));
    connect(applyBtn, SIGNAL(clicked()), SLOT(saveChanges()));
    btnLayout->addWidget(applyBtn);

    QFrame *line = new QFrame;
    line->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    mainLayout->addWidget(line);

    /* Editing area */
    QGridLayout *editsLayout = new QGridLayout;
    mainLayout->addLayout(editsLayout);

    QLabel *label = new QLabel(tr("Name:"));
    editsLayout->addWidget(label, 0, 0);

    m_nameEdit = new QLineEdit;
    editsLayout->addWidget(m_nameEdit, 0, 1);

    label = new QLabel(tr("Hostname:"));
    editsLayout->addWidget(label, 0, 2);

    m_hostnameEdit = new QLineEdit;
    editsLayout->addWidget(m_hostnameEdit, 0, 3);

    label = new QLabel(tr("Username:"));
    editsLayout->addWidget(label, 1, 0);

    m_usernameEdit = new QLineEdit;
    editsLayout->addWidget(m_usernameEdit, 1, 1);

    label = new QLabel(tr("Password:"));
    editsLayout->addWidget(label, 1, 2);

    m_passwordEdit = new QLineEdit;
    m_passwordEdit->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    editsLayout->addWidget(m_passwordEdit, 1, 3);
}

void OptionsServerPage::currentServerChanged()
{
    DVRServer *server = static_cast<DVRServersModel*>(m_serversView->model())->
                        serverForRow(m_serversView->currentIndex());
    if (!server)
    {
        m_nameEdit->clear();
        m_hostnameEdit->clear();
        m_usernameEdit->clear();
        m_passwordEdit->clear();
        return;
    }

    m_nameEdit->setText(server->displayName());
    m_hostnameEdit->setText(server->readSetting("hostname").toString());
    m_usernameEdit->setText(server->readSetting("username").toString());
    m_passwordEdit->setText(server->readSetting("password").toString());
}

void OptionsServerPage::addNewServer()
{

}

void OptionsServerPage::deleteServer()
{

}

void OptionsServerPage::saveChanges()
{
    DVRServer *server = static_cast<DVRServersModel*>(m_serversView->model())->
                        serverForRow(m_serversView->currentIndex());
    if (!server)
        return;

    server->setDisplayName(m_nameEdit->text().trimmed());
    server->writeSetting("hostname", m_hostnameEdit->text());
    server->writeSetting("username", m_usernameEdit->text());
    server->writeSetting("password", m_passwordEdit->text());
}
