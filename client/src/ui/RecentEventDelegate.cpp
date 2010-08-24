#include "RecentEventDelegate.h"
#include <QPainter>
#include <QTextLayout>

RecentEventDelegate::RecentEventDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

QSize RecentEventDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QTextLayout layout;
    createText(index, option, &layout);
    return layout.boundingRect().size().toSize();
}

void RecentEventDelegate::paint(QPainter *p, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QTextLayout layout;
    createText(index, option, &layout);
    layout.draw(p, option.rect.topLeft());
}

void RecentEventDelegate::createText(const QModelIndex &index, const QStyleOptionViewItem &opt, QTextLayout *layout) const
{
    int row = index.row();
    QModelIndex parent = index.parent();
    const QAbstractItemModel *model = index.model();

    layout->setCacheEnabled(true);
    layout->setFont(opt.font);

    QString text;
    QList<QTextLayout::FormatRange> formats;
    QTextLayout::FormatRange fmt;

    fmt.start = text.size();
    fmt.format.setFontWeight(QFont::Bold);
    text.append(model->index(row, 2, parent).data().toString());
    fmt.length = text.size() - fmt.start;
    formats.append(fmt);

    text.append(tr(" on ", "[type] on [camera] ([server]) [relative date]"));

    fmt.start = text.size();
    text.append(model->index(row, 1, parent).data().toString());
    fmt.length = text.size() - fmt.start;
    formats.append(fmt);

    text.append(tr(" (", "[type] on [camera] ([server]) [relative date]"));
    text.append(model->index(row, 0, parent).data().toString());
    text.append(tr("), ", "[type] on [camera] ([server]) [relative date]"));
    text.append(model->index(row, 3, parent).data().toString());

    layout->setText(text);
    layout->setAdditionalFormats(formats);

    layout->beginLayout();
    QTextLine line = layout->createLine();
    Q_ASSERT(line.isValid());

    line.setNumColumns(text.size());
    line.setPosition(QPointF(0, 0));

    Q_ASSERT(!layout->createLine().isValid());
    layout->endLayout();
}
