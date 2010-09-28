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
    rows = qMax(0, rows);
    columns = qMax(0, columns);
    if (rows == m_rowCount && columns == m_columnCount)
        return;

    Q_ASSERT(m_cameraWidgets.size() == m_rowCount);
    Q_ASSERT(m_cameraWidgets.isEmpty() || m_cameraWidgets[0].size() == m_columnCount);

    if (m_rowCount > rows)
    {
        int remove = m_rowCount - rows;

        /* If there are any empty rows, remove those first */
        for (int r = 0; r < m_rowCount; ++r)
        {
            const QList<LiveFeedWidget*> &row = m_cameraWidgets[r];
            bool empty = true;
            foreach (LiveFeedWidget *widget, row)
            {
                if (widget->camera())
                {
                    empty = false;
                    break;
                }
            }

            if (empty)
            {
                removeRow(r);
                if (!--remove)
                    break;
                --r;
            }
        }

        /* Otherwise, take rows from the bottom */
        for (int r = m_cameraWidgets.size()-1; remove && r >= 0; --r, --remove)
            removeRow(r);

        Q_ASSERT(!remove);
    }

    if (m_columnCount > columns)
    {
        int remove = m_columnCount - columns;

        for (int c = 0; c < m_columnCount; ++c)
        {
            bool empty = true;
            for (QList<QList<LiveFeedWidget*> >::Iterator it = m_cameraWidgets.begin(); it != m_cameraWidgets.end(); ++it)
            {
                if (it->at(c)->camera())
                {
                    empty = false;
                    break;
                }
            }

            if (empty)
            {
                removeColumn(c);
                if (!--remove)
                    break;
                --c;
            }
        }

        /* removeColumn() has updated m_columnCount */

        for (int c = m_columnCount-1; remove && c >= 0; --c, --remove)
            removeColumn(c);

        Q_ASSERT(!remove);
    }

    while (m_columnCount < columns)
    {
        for (int r = 0; r < m_cameraWidgets.size(); ++r)
        {
            LiveFeedWidget *widget = new LiveFeedWidget;
            m_cameraWidgets[r].append(widget);
            mainLayout->addWidget(widget, r, m_columnCount);
        }

        ++m_columnCount;
    }

    while (m_rowCount < rows)
    {
        QList<LiveFeedWidget*> row;
        for (int c = 0; c < m_columnCount; ++c)
        {
            LiveFeedWidget *widget = new LiveFeedWidget;
            row.append(widget);
            mainLayout->addWidget(widget, m_rowCount, c);
        }
        m_cameraWidgets.append(row);
        ++m_rowCount;
    }

    m_rowCount = rows;
    m_columnCount = columns;

    Q_ASSERT(m_cameraWidgets.size() == m_rowCount);
    Q_ASSERT(m_cameraWidgets.isEmpty() || m_cameraWidgets[0].size() == m_columnCount);

    emit gridSizeChanged(rows, columns);
}

void CameraAreaWidget::removeColumn(int c)
{
    for (int r = 0; r < m_cameraWidgets.size(); ++r)
    {
        delete m_cameraWidgets[r].takeAt(c);

        for (int i = c; i < m_cameraWidgets[r].size(); ++i)
            mainLayout->addWidget(m_cameraWidgets[r][i], r, i);
    }

    m_columnCount--;
}

void CameraAreaWidget::removeRow(int r)
{
    qDeleteAll(m_cameraWidgets.takeAt(r));

    for (int i = r; i < m_cameraWidgets.size(); ++i)
    {
        for (int c = 0; c < m_cameraWidgets[i].size(); ++c)
            mainLayout->addWidget(m_cameraWidgets[i][c], i, c);
    }

    m_rowCount--;
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
