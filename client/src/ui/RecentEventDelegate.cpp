#include "RecentEventDelegate.h"
#include <QPainter>
#include <QTextLayout>
#include <QApplication>
#include <QStyle>

RecentEventDelegate::RecentEventDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

QSize RecentEventDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(option.rect.width(), option.fontMetrics.height()+3);
}

void RecentEventDelegate::paint(QPainter *p, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 opt = option;
    qApp->style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, p, opt.widget);

    p->save();

    QRect r = option.rect.adjusted(2, 0, -2, 0);
    p->setPen(qvariant_cast<QColor>(index.data(Qt::ForegroundRole)));

    QTextLayout layout;
    createText(index, option, &layout);
    layout.draw(p, r.topLeft());

    const QAbstractItemModel *model = index.model();
    p->drawText(r, Qt::AlignVCenter | Qt::AlignRight, model->index(index.row(), 3, index.parent()).data().toString());
    p->restore();
}

void RecentEventDelegate::createText(const QModelIndex &index, const QStyleOptionViewItem &opt, QTextLayout *layout) const
{
    int row = index.row();
    QModelIndex parent = index.parent();
    const QAbstractItemModel *model = index.model();

    layout->setCacheEnabled(true);
    layout->setFont(opt.font);

    QTextOption textopt;
    textopt.setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    layout->setTextOption(textopt);

    QString text;
    QList<QTextLayout::FormatRange> formats;
    QTextLayout::FormatRange fmt;

    fmt.start = text.size();
    fmt.format.setFontWeight(QFont::Bold);
    text.append(model->index(row, 2, parent).data().toString());
    fmt.length = text.size() - fmt.start;
    formats.append(fmt);

    text.append(tr(" @ ", "[type] @ [camera], [server]"));

    fmt.start = text.size();
    text.append(model->index(row, 1, parent).data().toString());
    fmt.length = text.size() - fmt.start;
    formats.append(fmt);

    text.append(tr(", ", "[type] @ [camera], [server]"));
    text.append(model->index(row, 0, parent).data().toString());

    layout->setText(text);
    layout->setAdditionalFormats(formats);

    layout->beginLayout();
    QTextLine line = layout->createLine();
    line.setNumColumns(text.size());
    line.setPosition(QPointF(0, 0));

    layout->endLayout();
}
