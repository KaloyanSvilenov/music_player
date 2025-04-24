#include "loopbutton.h"

LoopButton::LoopButton(QWidget *parent)
    : HoverButton(parent){
    setAttribute(Qt::WA_Hover);
}

void LoopButton::enterEvent(QEnterEvent *event){
    if(hoverEnabled) setIcon(QIcon(hoverIconPath));
}

void LoopButton::leaveEvent(QEvent *event){
    if(hoverEnabled) setIcon(QIcon(defaultIconPath));
}

void LoopButton::changeHoverEnabled(){
    hoverEnabled ? setIcon(QIcon(hoverIconPath)) : setIcon(QIcon(defaultIconPath));
    hoverEnabled = hoverEnabled ? false : true;
}
