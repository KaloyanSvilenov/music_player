#include "playbutton.h"

PlayButton::PlayButton(QWidget *parent)
: QPushButton(parent){
    setAttribute(Qt::WA_Hover);
}

void PlayButton::enterEvent(QEnterEvent *event){
    if(isPlaying) {
        setIcon(QIcon(hoverPauseIconPath));
    }else{
        setIcon(QIcon(hoverPlayIconPath));
    }
}

void PlayButton::leaveEvent(QEvent *event){
    if(isPlaying) {
        setIcon(QIcon(defaultPauseIconPath));
    }else{
        setIcon(QIcon(defaultPlayIconPath));
    }
}

void PlayButton::setDefaultPlayIcon(const QString &path){
    defaultPlayIconPath = path;
    setIcon(QIcon(defaultPlayIconPath));
}

void PlayButton::setHoverPlayIcon(const QString&path){
    hoverPlayIconPath = path;
}

void PlayButton::setDefaultPauseIcon(const QString &path){
    defaultPauseIconPath = path;
}

void PlayButton::setHoverPauseIcon(const QString&path){
    hoverPauseIconPath = path;
}

void PlayButton::changePlaying(){
    isPlaying = isPlaying ? false : true;
    setIcon(QIcon(isPlaying ? hoverPauseIconPath : hoverPlayIconPath));
}
