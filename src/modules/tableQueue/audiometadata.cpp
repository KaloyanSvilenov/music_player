#include "audiometadata.h"
#include <QMediaPlayer>
#include <QFileInfo>
#include <QEventLoop>
#include <QRegularExpression>
#include <QImage>

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
    meta.bitrate = 0;

    // Using QMediaPlayer for metadata
    QMediaPlayer player;
    player.setSource(QUrl::fromLocalFile(filePath));

    // Wait for media status to be loaded
    QEventLoop loop;
    QObject::connect(&player, &QMediaPlayer::mediaStatusChanged, this, [&](QMediaPlayer::MediaStatus status) {
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

    // Get artist
    meta.artist = metadata.value(QMediaMetaData::AlbumArtist).toString();
    if (meta.artist.isEmpty()) meta.artist = "Unknown Artist";

    // Get album
    meta.album = metadata.value(QMediaMetaData::AlbumTitle).toString();
    if (meta.album.isEmpty()) meta.album = "Unknown Album";

    // Get genre
    meta.genre = parseGenre(metadata.value(QMediaMetaData::Genre));
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

    // Get bitrate
    meta.bitrate = metadata.value(QMediaMetaData::AudioBitRate).toString();
    if (meta.bitrate.isEmpty()) meta.bitrate = "Unknown BitRate";

    return meta;
}

QString AudioMetadataReader::parseGenre(const QVariant &genreData)
{
    if (!genreData.isValid())
        return "Unknown Genre";

    // Case 1: Already a single string (e.g., "Pop, Rock/Alt.Metal")
    if (genreData.typeId() == QMetaType::QString) {
        return genreData.toString().trimmed();
    }
    // Case 2: List of genres (e.g., ["Pop", "Rock", "Alt.Metal"])
    else if (genreData.typeId() == QMetaType::QStringList) {
        return genreData.toStringList().join(", ");
    }

    return "Unknown Genre";
}

QImage AudioMetadataReader::extractCoverArt(const QString& filePath)
{
    QMediaPlayer player;
    player.setSource(QUrl::fromLocalFile(filePath));

    // Wait for media to load
    QEventLoop loop;

    // Fixed connect call with context object (this)
    connect(&player, &QMediaPlayer::mediaStatusChanged,
            this,  // Added context object
            [&](QMediaPlayer::MediaStatus status) {
                if (status == QMediaPlayer::LoadedMedia ||
                    status == QMediaPlayer::BufferedMedia) {
                    loop.quit();
                }
            });

    loop.exec();

    // Try multiple cover art properties
    QVariant coverData = player.metaData().value(QMediaMetaData::CoverArtImage);
    if (!coverData.isValid()) {
        coverData = player.metaData().value(QMediaMetaData::ThumbnailImage);
    }

    return coverData.isValid() ? coverData.value<QImage>() : QImage();
}
