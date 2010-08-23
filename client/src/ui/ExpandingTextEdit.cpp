#include "ExpandingTextEdit.h"

ExpandingTextEdit::ExpandingTextEdit(QWidget *parent)
    : QTextEdit(parent)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(document(), SIGNAL(contentsChanged()), SLOT(documentChanged()));
}

QSize ExpandingTextEdit::sizeHint() const
{
    QSize sz = document()->size().toSize();
    QRect cr = contentsRect();
    sz += QSize(cr.x()*2, cr.y()*2);
    return sz;
}

QSize ExpandingTextEdit::minimumSizeHint() const
{
    return QSize();
}

void ExpandingTextEdit::resizeEvent(QResizeEvent *event)
{
    QTextEdit::resizeEvent(event);
    document()->setTextWidth(viewport()->width());
    updateGeometry();
}
