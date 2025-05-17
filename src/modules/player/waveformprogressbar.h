#pragma once
#include <QProgressBar>
#include <QVector>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QAudioDecoder>

class WaveformProgressBar : public QProgressBar
{
    Q_OBJECT

public:
    explicit WaveformProgressBar(QWidget *parent = nullptr);
    void setAudioFile(const QString &filePath);
    ~WaveformProgressBar(); // Added destructor to clean up decoder

signals:
    void userSeeked(int position);
    void waveformReady();  // Emitted when waveform is fully decoded

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void loadWaveformData();
    void regenerateWaveform();
    void stopAndClearDecoder(); // New method to stop and clean up decoder
    QVector<float> m_waveformData;
    QVector<float> m_tempSamples; // Raw, full-resolution samples
    bool m_dragging = false;
    QString m_audioFilePath;
    bool m_isLoading = false;
    QVector<float> m_pendingWaveform;
    QAudioDecoder *m_decoder = nullptr; // Track active decoder
};
