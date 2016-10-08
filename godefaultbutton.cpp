#include "godefaultbutton.h"
#include <QPainter>
#include <QEvent>
#include <QTimer>
#include <QStyleOption>
#include <QHBoxLayout>
#include <QLineEdit>

GoDefaultButton::GoDefaultButton(QLineEdit *parent, QVariant value):
    QWidget(parent), m_parentEdit(parent), m_defaultValue(value)
{
    parent->installEventFilter(this);
    parent->setText(m_defaultValue.toString());
    m_backgroundColor = QColor(255, 255, 255);
    m_buttonColor = Qt::darkCyan;
    m_bHovered = false;
    this->hide();
    this->setAttribute(Qt::WA_Hover);
    this->setCursor(QCursor(Qt::PointingHandCursor));
    this->setToolTip("Clear up to default value");

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 1, 1, 1);
    buttonLayout->addStretch();
    buttonLayout->addWidget(this);
    parent->setLayout(buttonLayout);
}

void GoDefaultButton::setBackColor(const QColor &color)
{
    m_backgroundColor = color;
}

void GoDefaultButton::setBtnColor(const QColor &color)
{
    m_buttonColor = color;
}

bool GoDefaultButton::event(QEvent *e)
{
    if (e->type() == QEvent::HoverEnter)
    {
        m_bHovered = true;
        update();
        return true;
    }
    else if(e->type() == QEvent::HoverLeave)
    {
        m_bHovered = false;
        update();
        return true;
    }
    else if(e->type() == QEvent::MouseButtonRelease)
    {
        m_parentEdit->setText(m_defaultValue.toString());
        return true;
    }
    return QWidget::event(e);
}

bool GoDefaultButton::eventFilter(QObject *obj, QEvent *e)
{
    if (e->type() == QEvent::FocusIn)
    {
        this->show();
        QTimer::singleShot(0, m_parentEdit, SLOT(selectAll()));
    }
    else if (e->type() == QEvent::FocusOut)
    {
        this->hide();
    }
    else if (e->type() == QEvent::Resize)
    {
        int nHeight = m_parentEdit->geometry().height() - 2;
        QSize size = QSize(nHeight, nHeight);
        this->setMinimumSize(size);
        this->setMaximumSize(size);
        // 设置输入框中文件输入区，不让输入的文字在被隐藏在按钮下
        m_parentEdit->setTextMargins(0, 1, size.width(), 1);
    }
    return QWidget::eventFilter(obj, e);
}

void GoDefaultButton::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    QRectF backRect = rect();
    qreal rX = backRect.x() + backRect.width()/5;
    qreal rY = backRect.y() + backRect.height()/5;
    qreal rW = backRect.width()/5*3;
    qreal rH = backRect.height()/5*3;
    QRectF circleRect(rX, rY, rW, rH);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.setBrush(m_backgroundColor);
    painter.setPen(Qt::NoPen);
    painter.drawRect(backRect);

    painter.setBrush(m_bHovered ? m_buttonColor.light() : m_buttonColor);
    painter.drawEllipse(circleRect);

    painter.setPen(QPen(Qt::white, 2));
    qreal rR = backRect.width()/10*3;
    rX = backRect.width()/2 - rR/3;
    rY = backRect.width()/2 + rR/3;
    painter.drawLine(QPointF(rX, rX), QPointF(rY, rY));
    painter.drawLine(QPointF(rY, rX), QPointF(rX, rY));

    //为了不屏蔽掉setStyleSheet功能
    QStyleOption opt;
    opt.init(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
    //QWidget::paintEvent(e);//QWidget的paintEvent事件中，什么都没做。
}

