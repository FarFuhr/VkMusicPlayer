#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>

class ClickableLabel : public QLabel
{
    Q_OBJECT

public:
    ClickableLabel(int id, const QString &text = QString(), QWidget *parent = 0);
    ~ClickableLabel();

    int getId() const { return m_id; }

private:
    QColor m_notActiveColor;
    QColor m_mouseEnterColor;
    QColor m_selectedColor;

    bool m_isActive;

    int m_id;

signals:
    void doubleClicked(int id);

protected:
    void mouseDoubleClickEvent(QMouseEvent *event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

public slots:
    void unselect();
    void select();
};

#endif // CLICKABLELABEL_H
