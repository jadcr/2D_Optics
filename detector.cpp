#include "detector.h"
#include <QtMath>
#include <QLineF>
#include <cmath>
#include "constants.h"


Detector::Detector(const QPointF& p1, const QPointF& p2, const QString& name)
    : OpticalElement(name), m_p1(p1), m_p2(p2)
{
    setReflective(false);
    QVector2D vec(p2 - p1);
    m_direction = vec.normalized();
    m_normal = QVector2D(-m_direction.y(), m_direction.x()).normalized();
}

void Detector::setPoints(const QPointF& p1, const QPointF& p2)
{
    m_p1 = p1;
    m_p2 = p2;
    QVector2D vec(p2 - p1);
    m_direction = vec.normalized();
    m_normal = QVector2D(-m_direction.y(), m_direction.x()).normalized();
}


bool Detector::intersect(const Ray& ray, double& t, QPointF& point, QVector2D& normal) const
{
    QVector2D dir = ray.direction();
    QVector2D origin = QVector2D(ray.origin());

    double denominator = QVector2D::dotProduct(dir, m_normal);
    if (qFabs(denominator) < EPSILON)
        return false;

    QVector2D w = origin - QVector2D(m_p1);
    double t_intersect = -QVector2D::dotProduct(w, m_normal) / denominator;
    if (t_intersect <= EPSILON)
        return false;

    QPointF p = ray.pointAt(t_intersect);

    QVector2D v = QVector2D(p - m_p1);
    double dot = QVector2D::dotProduct(v, m_direction);
    double length = QVector2D(m_p2 - m_p1).length();
    if (dot < -EPSILON || dot > length + EPSILON)
        return false;

    t = t_intersect;
    point = p;
    normal = m_normal;   // нормаль для детектора не важна
    return true;
}


void Detector::process(Ray& ray, const QPointF& point, const QVector2D& /*normal*/) const
{
    ray.setActive(false);
    HitData data;
    data.pos = point;
    data.initialY = ray.initialY();   // сохраняем зрачковую координату
    m_hits.append(data);
}

QString Detector::info() const
{
    return QString("%1: (%2,%3)-(%4,%5) hits=%6")
        .arg(name())
        .arg(m_p1.x()).arg(m_p1.y())
        .arg(m_p2.x()).arg(m_p2.y())
        .arg(m_hits.size());
}


// Новые методы для диаграммы рассеяния
QPointF Detector::centroid() const
{
    if (m_hits.isEmpty())
        return QPointF(0.0, 0.0);
    double sumX = 0.0, sumY = 0.0;
    for (const HitData& d : m_hits) {
        sumX += d.pos.x();
        sumY += d.pos.y();
    }
    return QPointF(sumX / m_hits.size(), sumY / m_hits.size());
}

double Detector::rmsRadius() const
{
    if (m_hits.isEmpty())
        return 0.0;
    QPointF c = centroid();
    double sumSq = 0.0;
    for (const HitData& d : m_hits) {
        double dx = d.pos.x() - c.x();
        double dy = d.pos.y() - c.y();
        sumSq += dx*dx + dy*dy;
    }
    return std::sqrt(sumSq / m_hits.size());
}
