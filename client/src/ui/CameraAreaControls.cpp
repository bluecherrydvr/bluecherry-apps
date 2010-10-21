#include "CameraAreaControls.h"
#include "CameraAreaWidget.h"
#include "SavedLayoutsModel.h"
#include <QBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QInputDialog>
#include <QSettings>
#include <QDebug>
#include <QMenu>
#include <QMacStyle>

CameraAreaControls::CameraAreaControls(CameraAreaWidget *area, QWidget *parent)
    : QWidget(parent), cameraArea(area), m_lastLayoutIndex(-1)
{
    QBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);

    /* Saved layouts box */
    m_savedLayouts = new QComboBox;
    m_savedLayouts->setModel(new SavedLayoutsModel(m_savedLayouts));
    m_savedLayouts->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_savedLayouts->setInsertPolicy(QComboBox::NoInsert);
    m_savedLayouts->setMinimumWidth(100);
    m_savedLayouts->setContextMenuPolicy(Qt::CustomContextMenu);
    layout->addWidget(m_savedLayouts);

    connect(m_savedLayouts, SIGNAL(currentIndexChanged(int)), SLOT(savedLayoutChanged(int)));
    connect(m_savedLayouts, SIGNAL(customContextMenuRequested(QPoint)), SLOT(showLayoutMenu(QPoint)));

    layout->addSpacing(20);

    /* Rows */
    QLabel *rowsLabel = new QLabel(tr("Rows:"));
    layout->addWidget(rowsLabel);

    QPushButton *addRowBtn = new QPushButton(tr("+"));
    connect(addRowBtn, SIGNAL(clicked()), area, SLOT(addRow()));
    layout->addWidget(addRowBtn);
    QPushButton *delRowBtn = new QPushButton(tr("-"));
    connect(delRowBtn, SIGNAL(clicked()), area, SLOT(removeRow()));
    layout->addWidget(delRowBtn);
    layout->addSpacing(20);

    /* Columns */
    QLabel *colsLabel = new QLabel(tr("Columns:"));
    layout->addWidget(colsLabel);

    QPushButton *addColBtn = new QPushButton(tr("+"));
    connect(addColBtn, SIGNAL(clicked()), area, SLOT(addColumn()));
    layout->addWidget(addColBtn);
    QPushButton *delColBtn = new QPushButton(tr("-"));
    connect(delColBtn, SIGNAL(clicked()), area, SLOT(removeColumn()));
    layout->addWidget(delColBtn);

#ifdef Q_OS_MAC
    if (!qobject_cast<QMacStyle*>(style()))
#endif
    {
        addRowBtn->setFixedWidth(26);
        delRowBtn->setFixedWidth(26);
        addColBtn->setFixedWidth(26);
        delColBtn->setFixedWidth(26);
    }

    layout->addStretch();

    QSettings settings;
    QString lastLayout = settings.value(QLatin1String("ui/cameraArea/lastLayout")).toString();
    if (!lastLayout.isEmpty())
    {
        int index = m_savedLayouts->findText(lastLayout);
        if (index >= 0 && index != m_savedLayouts->currentIndex())
            m_savedLayouts->setCurrentIndex(index);
        else
            savedLayoutChanged(m_savedLayouts->currentIndex());
    }
    else
        savedLayoutChanged(m_savedLayouts->currentIndex());
}

void CameraAreaControls::savedLayoutChanged(int index)
{
    static bool recursing = false;
    if (recursing)
        return;

    if (static_cast<SavedLayoutsModel*>(m_savedLayouts->model())->isNewLayoutItem(index))
    {
        QString re = QInputDialog::getText(window(), tr("Create camera layout"), tr("Enter a name for the new camera layout:"));
        if (re.isEmpty())
        {
            /* Cancelled; change back to the previous item */
            recursing = true;
            m_savedLayouts->setCurrentIndex(m_lastLayoutIndex);
            recursing = false;
            return;
        }

        recursing = true;

        index = m_savedLayouts->count() - 1;
        m_savedLayouts->insertItem(index, re, cameraArea->saveLayout());
        m_savedLayouts->setCurrentIndex(index);

        recursing = false;
        return;
    }

    saveLayout();

    QByteArray data = m_savedLayouts->itemData(index, SavedLayoutsModel::LayoutDataRole).toByteArray();
    if (!data.isEmpty() && !cameraArea->loadLayout(data))
        qDebug() << "Failed to load camera layout" << m_savedLayouts->itemText(index);

    m_lastLayoutIndex = index;
    QSettings settings;
    settings.setValue(QLatin1String("ui/cameraArea/lastLayout"), m_savedLayouts->itemText(index));
}

void CameraAreaControls::saveLayout()
{
    if (m_lastLayoutIndex < 0)
        return;

    QByteArray data = cameraArea->saveLayout();
    m_savedLayouts->setItemData(m_lastLayoutIndex, data, SavedLayoutsModel::LayoutDataRole);
}

void CameraAreaControls::showLayoutMenu(const QPoint &rpos, int index)
{
    if (index < 0)
        index = m_savedLayouts->currentIndex();

    if (static_cast<SavedLayoutsModel*>(m_savedLayouts->model())->isNewLayoutItem(index))
        return;

    QPoint pos = rpos;
    if (qobject_cast<QWidget*>(sender()))
        pos = static_cast<QWidget*>(sender())->mapToGlobal(pos);

    QMenu menu;
    menu.setTitle(m_savedLayouts->itemText(index));

    QAction *deleteAction = menu.addAction(tr("Delete %1").arg(menu.title()));
    if (m_savedLayouts->count() == 2)
        deleteAction->setEnabled(false);

    QAction *action = menu.exec(pos);
    if (!action)
        return;

    if (action == deleteAction)
    {
        bool b = m_savedLayouts->blockSignals(true);
        m_savedLayouts->removeItem(index);
        m_savedLayouts->blockSignals(b);

        int i = m_lastLayoutIndex - ((index < m_lastLayoutIndex) ? 1 : 0);
        if (index == m_lastLayoutIndex)
            i = qMax(index - 1, 0);
        m_lastLayoutIndex = -1;
        if (i != m_savedLayouts->currentIndex())
            m_savedLayouts->setCurrentIndex(i);
        else
            savedLayoutChanged(i);
    }
}
