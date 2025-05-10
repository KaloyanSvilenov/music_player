#include "audioplayer.h"
#include <QDebug>

AudioPlayer::AudioPlayer(QObject *parent) : QObject(parent),
    isPlaying(false)
{
    audioOutput = new QAudioOutput(this);
    audioOutput->setVolume(1.0); // Full volume (1.0 = 100%)

    player = new QMediaPlayer(this);
    player->setAudioOutput(audioOutput);

    connect(player, &QMediaPlayer::mediaStatusChanged, this, [](QMediaPlayer::MediaStatus status) {
        qDebug() << "Media status changed:" << status;
        if (status == QMediaPlayer::LoadedMedia)
            qDebug() << "Media loaded successfully.";
    });

    connect(player, &QMediaPlayer::errorOccurred, this, [](QMediaPlayer::Error error, const QString &errorString) {
        qDebug() << "Playback error:" << errorString;
    });

    connect(player, &QMediaPlayer::playbackStateChanged, this, [](QMediaPlayer::PlaybackState state) {
        qDebug() << "Playback state changed:" << state;
    });
}

void AudioPlayer::togglePlayPause()
{
    if (!isPlaying) {
        qDebug() << "Attempting to play:" << currSong;
        player->play();
        isPlaying = true;
    } else {
        qDebug() << "Pausing playback.";
        player->pause();
        isPlaying = false;
    }
}

void AudioPlayer::setCurrentSong(const QString &currSong){
    this->currSong = currSong;
}

void AudioPlayer::setVolume(float volume){
    audioOutput->setVolume(volume);
}

QMediaPlayer* AudioPlayer::getPlayer() const
{
    return player;
}

void AudioPlayer::play(){
    qDebug() << "Attempting to play:" << currSong;
    player->setSource(QUrl::fromLocalFile(currSong));
    player->play();
    isPlaying = true;
}
