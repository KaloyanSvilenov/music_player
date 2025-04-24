#pragma once
#include "hoverbutton.h"

class LoopButton : public HoverButton
{
public:
    explicit LoopButton(QWidget *parent = nullptr);

    void changeHoverEnabled();
protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
private:
    bool hoverEnabled = true;
};
