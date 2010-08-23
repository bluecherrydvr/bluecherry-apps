#ifndef EXPANDINGTEXTEDIT_H
#define EXPANDINGTEXTEDIT_H

#include <QTextEdit>

class ExpandingTextEdit : public QTextEdit
{
    Q_OBJECT

public:
    explicit ExpandingTextEdit(QWidget *parent = 0);

    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;

private slots:
    void documentChanged() { updateGeometry(); }

protected:
    virtual void resizeEvent(QResizeEvent *event);
};

#endif // EXPANDINGTEXTEDIT_H
