#ifndef HISTOGRAMWIDGET_H
#define HISTOGRAMWIDGET_H

#include <QWidget>
#include <QList>
#include <QPointF>

class HistogramWidget : public QWidget
{
    Q_OBJECT
public:
    explicit HistogramWidget(QWidget *parent = nullptr);
    void setPoints(const QList<QPointF>& points);
    void setAxis(bool useY); // true - ось Y, false - ось X
    void setBins(int bins);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QList<QPointF> m_points;
    bool m_useY;
    int m_bins;
    QVector<double> m_histogram;
    double m_min, m_max;
    void computeHistogram();
};

#endif
