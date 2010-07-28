#include "NumericOffsetWidget.h"
#include <QPainter>
#include <QPaintEvent>
#include <QFontMetrics>

NumericOffsetWidget::NumericOffsetWidget(QWidget *parent)
    : QWidget(parent), m_value(0)
{
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
}

void NumericOffsetWidget::setValue(int value)
{
    if (m_value == value)
        return;

    m_value = value;
    emit valueChanged(value);
    update();
}

QSize NumericOffsetWidget::sizeHint() const
{
    QFontMetrics fm(font());
    QSize textSize = fm.boundingRect(QLatin1String("999")).size();

    const int iconSize = 16, padding = 3;

    return QSize(textSize.width() + (iconSize*2) + (padding*2), qMax(iconSize, textSize.height()));
}

void NumericOffsetWidget::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    QRect r = event->rect();

    /* No icons yet; draw - and + instead */
    p.drawText(QRect(0, 0, 16, r.height()), Qt::AlignVCenter | Qt::AlignLeft, QLatin1String("-"));
    p.drawText(QRect(r.right()-16, 0, 16, r.height()), Qt::AlignVCenter | Qt::AlignRight, QLatin1String("+"));

    /* Text */
    r.adjusted(19, 0, -19, 0);
    p.drawText(r, Qt::AlignCenter, QString::number(value()));
}

void NumericOffsetWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
    {
        QWidget::mousePressEvent(event);
        return;
    }

    int x = event->pos().x();
    int mid = width()/2;

    if (qAbs(x - mid) < 10)
        clear();
    else if (x < mid)
        decrease();
    else
        increase();

    event->accept();
}
