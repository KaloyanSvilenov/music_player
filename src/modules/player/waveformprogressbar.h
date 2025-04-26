#pragma once
#include <QProgressBar>
#include <QVector>
#include <QMouseEvent>
#include <QPaintEvent>

class WaveformProgressBar : public QProgressBar
{
    Q_OBJECT

public:
    explicit WaveformProgressBar(QWidget *parent = nullptr);

signals:
    void userSeeked(int position);  // Emitted when user drags the progress

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<float> m_waveformData;
    bool m_dragging = false;
};
