#pragma once
#include <QPushButton>

class PlayButton : public QPushButton {
    Q_OBJECT
public:
    explicit PlayButton(QWidget *parent = nullptr);

    void setDefaultPlayIcon(const QString &path);
    void setHoverPlayIcon(const QString &path);
    void setDefaultPauseIcon(const QString &path);
    void setHoverPauseIcon(const QString &path);
    void changePlaying();
    void setToPlaying();

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QString defaultPlayIconPath;
    QString hoverPlayIconPath;
    QString defaultPauseIconPath;
    QString hoverPauseIconPath;
    bool isPlaying = false;
};
