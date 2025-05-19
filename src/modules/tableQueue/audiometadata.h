#ifndef AUDIOMETADATA_H
#define AUDIOMETADATA_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QMediaMetaData>

struct AudioMetadata {
    QString filePath;
    QString title;
    QString artist;
    QString album;
    QString genre;
    QString duration;  // Formatted as "mm:ss"
    qint64 durationMs; // Duration in milliseconds
    QString bitrate;
};

class AudioMetadataReader : public QObject
{
    Q_OBJECT
public:
    explicit AudioMetadataReader(QObject *parent = nullptr);
    AudioMetadata readMetadata(const QString &filePath);
    QImage extractCoverArt(const QString &filePath);

private:
    QString parseGenre(const QVariant &genreData);
};

#endif // AUDIOMETADATA_H
