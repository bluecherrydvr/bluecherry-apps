#ifndef NUMERICOFFSETWIDGET_H
#define NUMERICOFFSETWIDGET_H

#include <QWidget>

class NumericOffsetWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged STORED true USER true)

public:
    explicit NumericOffsetWidget(QWidget *parent = 0);

    int value() const { return m_value; }

    virtual QSize sizeHint() const;

public slots:
    void setValue(int value);
    void clear() { setValue(0); }
    void increase() { setValue(value()+1); }
    void decrease() { setValue(value()-1); }

signals:
    void valueChanged(int value);

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);

private:
    int m_value;

    QSize textAreaSize() const;
};

#endif // NUMERICOFFSETWIDGET_H
