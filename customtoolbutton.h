#ifndef CUSTOMTOOLBUTTON_H
#define CUSTOMTOOLBUTTON_H

#include <QLabel>

class  CustomToolButton : public QLabel
{
    Q_OBJECT

public:
    explicit CustomToolButton(QWidget *parent = 0);
    ~CustomToolButton();

    void initialize(const QString &sImage, const QString &sPressImage, const QString &sHoverImage = QString());

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void enterEvent(QEvent * event) override;
    void leaveEvent(QEvent * event) override;

private:
    QPixmap m_Image;
    QPixmap m_PressImage;
    QPixmap m_HoverImage;
};

#endif // CUSTOMTOOLBUTTON_H
