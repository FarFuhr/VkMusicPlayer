#ifndef IMAGEBUTTON_H
#define IMAGEBUTTON_H

#include <QPixmap>
#include <QWidget>

class QLabel;
class ImageButton : public QWidget
{
    Q_OBJECT
public:
    explicit ImageButton(const QString &imagePath, QWidget *parent = 0);

    void setPixmap(const QPixmap &pixmap);
    void loadPixmap(const QString &filepath);

private:
    QPixmap m_imagePixmap;
    QLabel* m_label;

protected:
    void mousePressEvent(QMouseEvent *event);

signals:
    void pressed();

public slots:
};

#endif // IMAGEBUTTON_H
