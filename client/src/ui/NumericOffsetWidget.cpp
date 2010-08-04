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

QSize NumericOffsetWidget::textAreaSize() const
{
    QFontMetrics fm(font());
    return fm.boundingRect(QLatin1String("-999")).size() + QSize(6, 0);
}

QSize NumericOffsetWidget::sizeHint() const
{
    const int iconSize = 16;
    QSize textSize = textAreaSize();

    return QSize(textSize.width() + (iconSize*2), qMax(iconSize, textSize.height()));
}

void NumericOffsetWidget::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    QRect r = event->rect();

    QSize textSize = textAreaSize();

    int textLeft = (r.width() - textSize.width()) / 2;

    /* No icons yet; draw - and + instead */
    p.drawText(QRect(textLeft - 16, 0, 16, r.height()), Qt::AlignVCenter | Qt::AlignRight, QLatin1String("-"));
    p.drawText(QRect(textLeft + textSize.width(), 0, 16, r.height()), Qt::AlignVCenter | Qt::AlignLeft, QLatin1String("+"));

    /* Text */
    p.drawText(QRect(textLeft, 0, textSize.width(), r.height()), Qt::AlignCenter, QString::number(value()));
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
