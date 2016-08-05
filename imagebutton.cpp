#include "imagebutton.h"

#include <QVBoxLayout>
#include <QLabel>

ImageButton::ImageButton(const QString &imagePath, QWidget *parent) : QWidget(parent)
{
    m_imagePixmap.load(imagePath);
    m_label = new QLabel;
    m_label->setPixmap(m_imagePixmap);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_label, 0, Qt::AlignCenter);

    this->setLayout(layout);
}

void ImageButton::mousePressEvent(QMouseEvent *event)
{
    emit pressed();
}

void ImageButton::setPixmap(const QPixmap &pixmap)
{
    m_imagePixmap = pixmap;
    m_label->setPixmap(m_imagePixmap);
}

void ImageButton::loadPixmap(const QString &filepath)
{
    m_imagePixmap.load(filepath);
    m_label->setPixmap(m_imagePixmap);
}
