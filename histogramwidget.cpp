#include "histogramwidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QtMath>
#include <algorithm>

HistogramWidget::HistogramWidget(QWidget *parent)
    : QWidget(parent), m_useY(true), m_bins(50)
{
    setMinimumSize(200, 300);
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}

void HistogramWidget::setPoints(const QList<QPointF>& points)
{
    m_points = points;
    computeHistogram();
    update();
}

void HistogramWidget::setAxis(bool useY)
{
    m_useY = useY;
    computeHistogram();
    update();
}

void HistogramWidget::setBins(int bins)
{
    m_bins = bins;
    computeHistogram();
    update();
}

void HistogramWidget::computeHistogram()
{
    if (m_points.isEmpty()) {
        m_histogram.clear();
        return;
    }

    QVector<double> values;
    for (const QPointF& p : m_points) {
        double v = m_useY ? p.y() : p.x();
        values.append(v);
    }
    // min/max
    m_min = *std::min_element(values.begin(), values.end());
    m_max = *std::max_element(values.begin(), values.end());
    if (qFabs(m_max - m_min) < 1e-9) {
        m_min -= 0.5;
        m_max += 0.5;
    }
    double step = (m_max - m_min) / m_bins;
    m_histogram.fill(0, m_bins);
    for (double v : values) {
        int idx = (v - m_min) / step;
        if (idx < 0) idx = 0;
        if (idx >= m_bins) idx = m_bins - 1;
        m_histogram[idx]++;
    }
}

void HistogramWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::white);
    if (m_points.isEmpty()) {
        painter.drawText(rect(), Qt::AlignCenter, "Нет данных");
        return;
    }

    double maxCount = *std::max_element(m_histogram.begin(), m_histogram.end());
    if (maxCount < 1) maxCount = 1;

    // Определяем область для гистограммы
    QRect chartRect = rect().adjusted(40, 10, -10, -30); // слева 40 для подписей Y

    // Ось Y
    int targetTicks = 5; // желаемое количество делений
    double step = maxCount / targetTicks;
    // Округляем до 1, 2, 5 и тд
    double scale = std::pow(10.0, std::floor(std::log10(step)));
    double normalized = step / scale;
    if (normalized < 1.5) step = 1 * scale;
    else if (normalized < 3.5) step = 2 * scale;
    else if (normalized < 7.5) step = 5 * scale;
    else step = 10 * scale;

    painter.setPen(QPen(Qt::lightGray, 1, Qt::DashLine));
    for (double yVal = 0; yVal <= maxCount + step/2; yVal += step) {
        int yPix = chartRect.bottom() - (yVal / maxCount) * chartRect.height();
        if (yPix < chartRect.top()) continue;
        painter.drawLine(chartRect.left(), yPix, chartRect.right(), yPix);
        painter.setPen(Qt::black);
        painter.drawText(QRect(0, yPix - 8, chartRect.left() - 5, 16),
                         Qt::AlignRight | Qt::AlignVCenter,
                         QString::number(yVal, 'f', 0));
        painter.setPen(QPen(Qt::lightGray, 1, Qt::DashLine));
    }
    painter.save();
    painter.rotate(-90);
    painter.drawText(-(chartRect.center().y()), chartRect.left() - 25,
                     "Частота (кол-во лучей)");
    painter.restore();

    //Рисуем столбцы гистограммы
    double binWidth = double(chartRect.width()) / m_bins;
    painter.setPen(Qt::blue);
    for (int i = 0; i < m_bins; ++i) {
        int height = (m_histogram[i] / maxCount) * chartRect.height();
        QRect bar(chartRect.left() + i * binWidth,
                  chartRect.bottom() - height,
                  static_cast<int>(binWidth), height);
        painter.fillRect(bar, QColor(70, 130, 180));
        painter.drawRect(bar);
    }

    //Ось X
    painter.setPen(Qt::black);
    painter.drawLine(chartRect.left(), chartRect.bottom(), chartRect.right(), chartRect.bottom());
    painter.drawLine(chartRect.left(), chartRect.top(), chartRect.left(), chartRect.bottom());

    QString axisLabel = m_useY ? "Координата Y (мм)" : "Координата X (мм)";
    painter.drawText(chartRect.center().x(), height() - 5, axisLabel);

    for (int i = 0; i <= m_bins; ++i) {
        double val = m_min + (m_max - m_min) * i / m_bins;
        int xPix = chartRect.left() + i * binWidth;
        painter.drawLine(xPix, chartRect.bottom(), xPix, chartRect.bottom() + 5);
        if (i % (m_bins/4) == 0) {
            painter.drawText(xPix - 10, chartRect.bottom() + 15, QString::number(val, 'f', 2));
        }
    }
}
