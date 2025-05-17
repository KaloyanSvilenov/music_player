#include "waveformprogressbar.h"
#include <QPainter>
#include <QStyleOptionProgressBar>
#include <QAudioDecoder>
#include <QDebug>

WaveformProgressBar::WaveformProgressBar(QWidget *parent)
    : QProgressBar(parent)
{
    setMouseTracking(true);
    setCursor(Qt::PointingHandCursor);
    setValue(0);
}

void WaveformProgressBar::loadWaveformData()
{
    if (m_audioFilePath.isEmpty()) return;

    QAudioDecoder *decoder = new QAudioDecoder(this);
    decoder->setSource(QUrl::fromLocalFile(m_audioFilePath));

    if (decoder->error() != QAudioDecoder::NoError) {
        qDebug() << "Failed to open audio file:" << m_audioFilePath << decoder->errorString();
        delete decoder;
        return;
    }

    m_pendingWaveform.clear();
    m_isLoading = true;

    connect(decoder, &QAudioDecoder::bufferReady, this, [=]() {
        QAudioBuffer buffer = decoder->read();
        const qint16 *data = buffer.constData<qint16>();
        int sampleCount = buffer.sampleCount();

        for (int i = 0; i < sampleCount; ++i) {
            m_pendingWaveform.append(data[i] / 32768.0f);
        }
    });

    connect(decoder, &QAudioDecoder::finished, this, [=]() {
        QVector<float> finalData;
        int targetSamples = qMin(1000, width());

        // 💡 Store decoded samples for reuse on resize
        m_tempSamples = m_pendingWaveform;

        for (int i = 0; i < targetSamples; ++i) {
            int srcIndex = (i * m_tempSamples.size()) / targetSamples;
            finalData.append(qBound(-1.0f, m_tempSamples.value(srcIndex), 1.0f));
        }

        m_waveformData = finalData;
        m_pendingWaveform.clear();
        m_isLoading = false;
        update();
        emit waveformReady();
        decoder->deleteLater();
    });


    decoder->start();
}

void WaveformProgressBar::regenerateWaveform()
{
    m_waveformData.clear();
    int targetSamples = qMin(1000, width());
    if (!m_tempSamples.isEmpty()) {
        for (int i = 0; i < targetSamples; ++i) {
            int srcIndex = (i * m_tempSamples.size()) / targetSamples;
            m_waveformData.append(qBound(-1.0f, m_tempSamples[srcIndex], 1.0f));
        }
    }
    update();
}

void WaveformProgressBar::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        int newValue = minimum() + (maximum() - minimum()) * event->position().x() / width();
        setValue(newValue);
        emit userSeeked(newValue);
    }
    QProgressBar::mousePressEvent(event);
}

void WaveformProgressBar::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        int newValue = minimum() + (maximum() - minimum()) * event->position().x() / width();
        setValue(newValue);
        emit userSeeked(newValue);
    }
    QProgressBar::mouseMoveEvent(event);
}

void WaveformProgressBar::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
    }
    QProgressBar::mouseReleaseEvent(event);
}

void WaveformProgressBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int w = width();
    int h = height();
    float progress = (value() - minimum()) / static_cast<float>(maximum() - minimum());
    int progressWidth = static_cast<int>(w * progress);

    QColor bgColor("#D0CCD0");
    QColor fgColor("#BD93F9");

    if (!m_waveformData.isEmpty() && !m_isLoading) {
        // --- Draw waveform background ---
        painter.setPen(QPen(bgColor, 1));
        for (int i = 0; i < m_waveformData.size(); ++i) {
            int x = (i * w) / m_waveformData.size();
            float amplitude = m_waveformData[i];
            int lineHeight = static_cast<int>(amplitude * h / 2);
            painter.drawLine(x, h / 2, x, h / 2 + lineHeight);
            painter.drawLine(x, h / 2, x, h / 2 - lineHeight);
        }

        // --- Draw waveform progress ---
        painter.setPen(QPen(fgColor, 1));
        for (int i = 0; i < m_waveformData.size(); ++i) {
            int x = (i * w) / m_waveformData.size();
            if (x > progressWidth) break;
            float amplitude = m_waveformData[i];
            int lineHeight = static_cast<int>(amplitude * h / 2);
            painter.drawLine(x, h / 2, x, h / 2 + lineHeight);
            painter.drawLine(x, h / 2, x, h / 2 - lineHeight);
        }
    } else {
        // --- Draw clean placeholder line while loading or no data ---
        painter.setPen(QPen(Qt::gray, 1));
        int centerY = height() / 2;
        painter.drawLine(0, centerY, width(), centerY);
    }
}

void WaveformProgressBar::resizeEvent(QResizeEvent *event)
{
    QProgressBar::resizeEvent(event);
    regenerateWaveform(); // Use existing samples instead of re-decoding
}

void WaveformProgressBar::setAudioFile(const QString &filePath)
{
    m_audioFilePath = filePath;
    loadWaveformData();
}

