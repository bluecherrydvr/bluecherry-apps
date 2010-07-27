#include "CameraAreaControls.h"
#include "CameraAreaWidget.h"
#include <QBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>

CameraAreaControls::CameraAreaControls(CameraAreaWidget *area, QWidget *parent)
    : QWidget(parent), cameraArea(area)
{
    QBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);

    /* Saved layouts box */
    m_savedLayouts = new QComboBox;
    m_savedLayouts->addItem(tr("Unsaved Layout"));
    m_savedLayouts->addItem(QLatin1String("My Saved Layout"));
    m_savedLayouts->addItem(QLatin1String("Another Layout"));
    layout->addWidget(m_savedLayouts);

    QPushButton *saveLayoutBtn = new QPushButton(tr("Save"));
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
}
