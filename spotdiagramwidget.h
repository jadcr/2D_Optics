#ifndef SPOTDIAGRAMWIDGET_H
#define SPOTDIAGRAMWIDGET_H

#include <QWidget>
#include <QList>
#include <QPointF>
#include "detector.h"

class SpotDiagramWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SpotDiagramWidget(QWidget *parent = nullptr);

    void setHitData(const QList<Detector::HitData>& data);
    void setAiryRadius(double radius) { m_airyRadius = radius; }
    void clear();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    QList<Detector::HitData> m_hitData;
    QList<QPointF> m_points;
    double m_airyRadius;
    double m_scale;
    QPointF m_offset;
    bool m_panning;
    QPoint m_lastPan;

    QPointF worldToScreen(const QPointF& world) const;
    QPointF screenToWorld(const QPoint& screen) const;
};

#endif // SPOTDIAGRAMWIDGET_H
