#include "WaveformProgressBar.h"
#include <QPainter>
#include <QStyleOptionProgressBar>

WaveformProgressBar::WaveformProgressBar(QWidget *parent)
    : QProgressBar(parent)
{
    setMouseTracking(true);
    setCursor(Qt::PointingHandCursor);
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
    QStyleOptionProgressBar option;
    initStyleOption(&option);
    style()->drawControl(QStyle::CE_ProgressBar, &option, &painter, this);
}
