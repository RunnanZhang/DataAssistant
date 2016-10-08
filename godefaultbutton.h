#ifndef GODEFAULTBUTTON_H
#define GODEFAULTBUTTON_H

#include <QWidget>
#include <QColor>
#include <QVariant>
class QLineEdit;

class GoDefaultButton : public QWidget
{
    Q_OBJECT

public:
    explicit GoDefaultButton(QLineEdit *parent, QVariant value = 0);

public:
    void setBackColor(const QColor &);
private:
    QColor m_backgroundColor;

public:
    void setBtnColor(const QColor &);
private:
    QColor m_buttonColor;

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual bool event(QEvent * e);
    virtual bool eventFilter(QObject *obj, QEvent *e);

private:
    QLineEdit* m_parentEdit;
    bool m_bHovered;
    QVariant m_defaultValue;
};

#endif // GODEFAULTBUTTON_H
