#ifndef AUDIOMETADATA_H
#define AUDIOMETADATA_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QMediaMetaData>

struct AudioMetadata {
    QString title;
    QString artist;
    QString album;
    QString genre;
    QString duration;  // Formatted as "mm:ss"
    qint64 durationMs; // Duration in milliseconds
};

class AudioMetadataReader : public QObject
{
    Q_OBJECT
public:
    explicit AudioMetadataReader(QObject *parent = nullptr);
    AudioMetadata readMetadata(const QString &filePath);
};

#endif // AUDIOMETADATA_H
