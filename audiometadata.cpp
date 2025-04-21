#include "audiometadata.h"
#include <QMediaPlayer>
#include <QFileInfo>
#include <QEventLoop>

AudioMetadataReader::AudioMetadataReader(QObject *parent) : QObject(parent) {}

AudioMetadata AudioMetadataReader::readMetadata(const QString &filePath)
{
    AudioMetadata meta;
    QFileInfo fi(filePath);

    // Default to filename if no title
    meta.title = fi.completeBaseName();
    meta.artist = "Unknown Artist";
    meta.album = "Unknown Album";
    meta.genre = "Unknown Genre";
    meta.duration = "--:--";
    meta.durationMs = 0;

    // Using QMediaPlayer for metadata
    QMediaPlayer player;
    player.setSource(QUrl::fromLocalFile(filePath));

    // Wait for media status to be loaded
    QEventLoop loop;
    QObject::connect(&player, &QMediaPlayer::mediaStatusChanged, [&](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::LoadedMedia || status == QMediaPlayer::BufferedMedia) {
            loop.quit();
        }
    });
    loop.exec();

    // Read metadata
    auto metadata = player.metaData();

    // Get title
    meta.title = metadata.value(QMediaMetaData::Title).toString();
    if (meta.title.isEmpty()) {
        meta.title = QFileInfo(filePath).completeBaseName();
    }

    // Get other metadata
    meta.artist = metadata.value(QMediaMetaData::AlbumArtist).toString();
    if (meta.artist.isEmpty()) meta.artist = "Unknown Artist";

    meta.album = metadata.value(QMediaMetaData::AlbumTitle).toString();
    if (meta.album.isEmpty()) meta.album = "Unknown Album";

    meta.genre = metadata.value(QMediaMetaData::Genre).toString();
    if (meta.genre.isEmpty()) meta.genre = "Unknown Genre";

    // Get duration
    QVariant duration = metadata.value(QMediaMetaData::Duration);
    if (!duration.isNull()) {
        meta.durationMs = duration.toLongLong();
        qint64 seconds = meta.durationMs / 1000;
        meta.duration = QString("%1:%2")
                            .arg(seconds / 60, 2, 10, QLatin1Char('0'))
                            .arg(seconds % 60, 2, 10, QLatin1Char('0'));
    }

    return meta;
}
