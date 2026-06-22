#ifndef DETECTOR_H
#define DETECTOR_H

#include "opticalelement.h"
#include <QList>

class Detector : public OpticalElement
{
public:
    struct HitData {
        QPointF pos;      // точка попадания на детектор
        double initialY;  // начальная координата луча на входном зрачке
    };
    Detector(const QPointF& p1, const QPointF& p2, const QString& name = "Detector");

    QPointF p1() const { return m_p1; }
    QPointF p2() const { return m_p2; }
    void setPoints(const QPointF& p1, const QPointF& p2);

    bool intersect(const Ray& ray, double& t, QPointF& point, QVector2D& normal) const override;
    void process(Ray& ray, const QPointF& point, const QVector2D& normal) const override;
    QString info() const override;

    // Новые методы для диаграммы рассеяния
    const QList<HitData>& hitData() const { return m_hits; }
    void clearHits() { m_hits.clear(); }
    double rmsRadius() const;          // среднеквадратичный радиус
    QPointF centroid() const;          // центроид
    int hitCount() const { return m_hits.size(); }

private:
    QPointF m_p1, m_p2;
    QVector2D m_direction;
    QVector2D m_normal;
    mutable QList<HitData> m_hits;   // список точек попаданий
};

#endif // DETECTOR_H
