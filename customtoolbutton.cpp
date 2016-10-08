#include "customtoolbutton.h"
#include <QMouseEvent>

CustomToolButton::CustomToolButton(QWidget *parent) :
    QLabel(parent)
{

}

CustomToolButton::~CustomToolButton()
{
}

void CustomToolButton::initialize(const QString &sImage, const QString &sPressImage, const QString &sHoverImage)
{
    m_Image.load(sImage);
    m_PressImage.load(sPressImage);
    if(!sHoverImage.isEmpty()) {
        m_HoverImage.load(sHoverImage);
    } else {
        // 先适配没有hover效果的情况.
        m_HoverImage.load(sImage);
    }
    this->setPixmap(m_Image);
}

void CustomToolButton::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    this->setPixmap(m_PressImage);
}

void CustomToolButton::mouseReleaseEvent(QMouseEvent *event)
{
    // 只有当按钮释放后，还在按钮区域内，才触发clicked事件.
    QRect rect = this->geometry();
    QPoint globalPt = this->mapToGlobal(QPoint(0, 0));
    QRect globalRect = QRect(globalPt, rect.size());
    if(globalRect.contains(event->globalPos())) {
        emit clicked();
    }

    // clicked()触发的槽函数，可能鼠标位置又改变了，比如弹出Menu操作后,所以重新判断.
    if(globalRect.contains(QCursor::pos())) {
        this->setPixmap(m_HoverImage);
    } else {
        this->setPixmap(m_Image);
    }
}

void CustomToolButton::enterEvent(QEvent * event)
{
    Q_UNUSED(event)
    if(this->isEnabled()) {
        this->setPixmap(m_HoverImage);
    }
}

void CustomToolButton::leaveEvent(QEvent * event)
{
    Q_UNUSED(event)
    if(this->isEnabled()) {
        this->setPixmap(m_Image);
    }
}
