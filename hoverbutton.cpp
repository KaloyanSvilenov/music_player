#include "hoverbutton.h"

HoverButton::HoverButton(QWidget *parent)
: QPushButton(parent) {
    setAttribute(Qt::WA_Hover);
}

void HoverButton::enterEvent(QEnterEvent *event){
    setIcon(QIcon(hoverIconPath));
    QPushButton::enterEvent(event);
}

void HoverButton::leaveEvent(QEvent *event){
    setIcon(QIcon(defaultIconPath));
    QPushButton::leaveEvent(event);
}

void HoverButton::setDefaultIcon(const QString &path){
    defaultIconPath = path;
    setIcon(QIcon(path));
}

void HoverButton::setHoverIcon(const QString &path){
    hoverIconPath = path;
}
