#include "EventCommentsWidget.h"
#include <QTextCursor>
#include <QTextBlock>
#include <QDateTime>

EventCommentsWidget::EventCommentsWidget(QWidget *parent)
    : QTextEdit(parent)
{
    setReadOnly(true);
}

QSize EventCommentsWidget::minimumSizeHint() const
{
    return QSize();
}

void EventCommentsWidget::appendComment(const QString &author, const QDateTime &date, const QString &text)
{
    QTextCursor cursor(document());
    cursor.movePosition(QTextCursor::End);
    if (!cursor.atBlockStart())
        cursor.insertBlock();

    cursor.insertHtml(QString::fromLatin1("<table width='100%' cellspacing=0 style='margin-bottom:6px;'><tr>"
                                          "<td style='font-weight:bold'>%1</td><td align='right'>%2</td></tr>"
                                          "</table><br>%3<hr>").arg(Qt::escape(author), date.toString(),
                                                                    Qt::escape(text)));
}
