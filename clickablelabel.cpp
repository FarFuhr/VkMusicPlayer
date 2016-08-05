#include "clickablelabel.h"

#include <QDebug>

ClickableLabel::ClickableLabel(int id, const QString &text, QWidget *parent)
    : QLabel(parent), m_id(id)
{
    this->setText(text);
    this->setAutoFillBackground(true);

    m_notActiveColor = this->palette().background().color();
    m_mouseEnterColor = QColor(135, 206, 235);
    m_selectedColor = QColor(0, 191, 255);

    m_isActive = false;

    this->adjustSize();
    this->setFixedHeight(this->size().height()*3);
}

ClickableLabel::~ClickableLabel()
{

}

void ClickableLabel::mouseDoubleClickEvent(QMouseEvent *event)
{
    emit doubleClicked(m_id);

    m_isActive = true;
}

void ClickableLabel::unselect()
{
    QPalette newPalette = this->palette();
    newPalette.setColor(QPalette::Window, m_notActiveColor);
    this->setPalette(newPalette);

    m_isActive = false;
}

void ClickableLabel::enterEvent(QEvent *event)
{
    if(!m_isActive)
    {
        QPalette newPalette = this->palette();
        newPalette.setColor(QPalette::Window, m_mouseEnterColor);
        this->setPalette(newPalette);
    }
}

void ClickableLabel::leaveEvent(QEvent *event)
{
    if(!m_isActive)
    {
        QPalette newPalette = this->palette();
        newPalette.setColor(QPalette::Window, m_notActiveColor);
        this->setPalette(newPalette);
    }
}

void ClickableLabel::select()
{
    QPalette newPalette = this->palette();
    newPalette.setColor(QPalette::Window, m_selectedColor);
    this->setPalette(newPalette);

    m_isActive = true;
}
