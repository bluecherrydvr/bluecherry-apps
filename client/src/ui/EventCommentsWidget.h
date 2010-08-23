#ifndef EVENTCOMMENTSWIDGET_H
#define EVENTCOMMENTSWIDGET_H

#include <QTextEdit>

class QDateTime;

class EventCommentsWidget : public QTextEdit
{
    Q_OBJECT

public:
    explicit EventCommentsWidget(QWidget *parent = 0);

    virtual QSize minimumSizeHint() const;

    void appendComment(const QString &author, const QDateTime &date, const QString &text);
};

#endif // EVENTCOMMENTSWIDGET_H
