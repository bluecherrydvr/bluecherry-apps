#include "CameraAreaWidget.h"
#include "LiveFeedWidget.h"
#include <QGridLayout>

CameraAreaWidget::CameraAreaWidget(QWidget *parent)
    : QFrame(parent), m_rowCount(0), m_columnCount(0)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFrameStyle(QFrame::Sunken | QFrame::Panel);
    setAutoFillBackground(true);

    QPalette p = palette();
    p.setColor(QPalette::Window, QColor(20, 20, 20));
    p.setColor(QPalette::WindowText, Qt::white);
    setPalette(p);

    mainLayout = new QGridLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(2);

    setGridSize(3, 3);
}

void CameraAreaWidget::setGridSize(int rows, int columns)
{
    if (rows == m_rowCount && columns == m_columnCount)
        return;

    m_rowCount = rows;
    m_columnCount = columns;

    /* Create a new set of placeholder widgets.. */
    qDeleteAll(m_cameraWidgets);
    m_cameraWidgets.clear();

    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < columns; ++c)
        {
            LiveFeedWidget *widget = new LiveFeedWidget;
            mainLayout->addWidget(widget, r, c);
            m_cameraWidgets.append(widget);
        }
    }

    emit gridSizeChanged(rows, columns);
}

void CameraAreaWidget::openFullScreen()
{
    setWindowFlags(Qt::Window);
    setFrameStyle(QFrame::NoFrame);
    showFullScreen();
}

void CameraAreaWidget::closeFullScreen()
{
    setWindowFlags(0);
    setFrameStyle(QFrame::Sunken | QFrame::Panel);
    show();
}

void CameraAreaWidget::toggleFullScreen()
{
    if (isFullScreen())
        closeFullScreen();
    else
        openFullScreen();
}
