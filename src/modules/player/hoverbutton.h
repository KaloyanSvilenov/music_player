#pragma once
#include <QPushButton>

class HoverButton : public QPushButton {
    Q_OBJECT
public:
    explicit HoverButton(QWidget *parent = nullptr);

    void setDefaultIcon(const QString &path);
    void setHoverIcon(const QString &path);

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    QString defaultIconPath;
    QString hoverIconPath;
};
