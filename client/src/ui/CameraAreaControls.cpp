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
    layout->addWidget(m_savedLayouts);

    connect(m_savedLayouts, SIGNAL(currentIndexChanged(int)), SLOT(savedLayoutChanged(int)));

    QPushButton *saveLayoutBtn = new QPushButton(tr("Save Layout"));
    layout->addWidget(saveLayoutBtn);
    layout->addSpacing(20);

    /* Rows */
    QLabel *rowsLabel = new QLabel(tr("Rows:"));
    layout->addWidget(rowsLabel);

    QPushButton *addRowBtn = new QPushButton(tr("+"));
    connect(addRowBtn, SIGNAL(clicked()), area, SLOT(addRow()));
    addRowBtn->setFixedWidth(26);
    layout->addWidget(addRowBtn);
    QPushButton *delRowBtn = new QPushButton(tr("-"));
    connect(delRowBtn, SIGNAL(clicked()), area, SLOT(removeRow()));
    delRowBtn->setFixedWidth(26);
    layout->addWidget(delRowBtn);
    layout->addSpacing(20);

    /* Columns */
    QLabel *colsLabel = new QLabel(tr("Columns:"));
    layout->addWidget(colsLabel);

    QPushButton *addColBtn = new QPushButton(tr("+"));
    connect(addColBtn, SIGNAL(clicked()), area, SLOT(addColumn()));
    addColBtn->setFixedWidth(26);
    layout->addWidget(addColBtn);
    QPushButton *delColBtn = new QPushButton(tr("-"));
    connect(delColBtn, SIGNAL(clicked()), area, SLOT(removeColumn()));
    delColBtn->setFixedWidth(26);
    layout->addWidget(delColBtn);

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
    if (static_cast<SavedLayoutsModel*>(m_savedLayouts->model())->isNewLayoutItem(index))
    {
        static bool recursing = false;
        if (recursing)
            return;

        QString re = QInputDialog::getText(window(), tr("Create camera layout"), tr("Enter a name for the new camera layout:"));
        if (re.isEmpty())
            return;

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
