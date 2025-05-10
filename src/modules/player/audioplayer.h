#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QVector>
#include <QString>

class AudioPlayer : public QObject
{
    Q_OBJECT
public:
    explicit AudioPlayer(QObject *parent = nullptr);
    void togglePlayPause();
    void setCurrentSong(const QString &currSong);
    void setVolume(float volume);
    QMediaPlayer* getPlayer() const;
    void play();

private:
    QMediaPlayer *player;
    QAudioOutput *audioOutput;
    bool isPlaying;
    QString currSong;
};

#endif // AUDIOPLAYER_H
