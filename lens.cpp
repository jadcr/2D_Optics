#include "lens.h"
#include "constants.h"
#include <QtMath>
#include <QDebug>


Lens::Lens(const QPointF& center, double thickness,
           double R1, double R2,
           double k1, double k2,
           double diameter, double n_lens,
           const QString& name)
    : OpticalElement(name)
    , m_center(center)
    , m_thickness(thickness)
    , m_R1(R1), m_R2(R2)
    , m_k1(k1), m_k2(k2)
    , m_diameter(diameter)
    , m_n_lens(n_lens)
    , m_frontSurface(vertex1(), R1, k1, diameter, 1.0, n_lens, false, "передняя")
    , m_backSurface(vertex2(), R2, k2, diameter, n_lens, 1.0, false, "задняя")
{
    setN1(1.0);
    setN2(1.0);
}


QPointF Lens::vertex1() const
{
    return QPointF(m_center.x() - m_thickness/2, m_center.y());
}

QPointF Lens::vertex2() const
{
    return QPointF(m_center.x() + m_thickness/2, m_center.y());
}


void Lens::setCenter(const QPointF& c)
{
    m_center = c;
    m_frontSurface.setVertex(vertex1());
    m_backSurface.setVertex(vertex2());
}

void Lens::setThickness(double t)
{
    m_thickness = t;
    m_frontSurface.setVertex(vertex1());
    m_backSurface.setVertex(vertex2());
}

void Lens::setRadius1(double r) { m_R1 = r; m_frontSurface.setRadius(r); }
void Lens::setRadius2(double r) { m_R2 = r; m_backSurface.setRadius(r); }
void Lens::setK1(double k) { m_k1 = k; m_frontSurface.setK(k); }
void Lens::setK2(double k) { m_k2 = k; m_backSurface.setK(k); }
void Lens::setDiameter(double d)
{
    m_diameter = d;
    m_frontSurface.setDiameter(d);
    m_backSurface.setDiameter(d);
}
void Lens::setNLens(double n)
{
    m_n_lens = n;
    m_frontSurface.setN2(n);
    m_backSurface.setN1(n);
}


bool Lens::intersectSingle(const ConicSurface& surf, const Ray& ray, double& t,
                           QPointF& point, QVector2D& normal) const
{
    return surf.intersect(ray, t, point, normal);
}


bool Lens::intersect(const Ray& ray, double& t, QPointF& point, QVector2D& normal) const
{
    double t1, t2;
    QPointF p1, p2;
    QVector2D n1, n2;
    bool hit1 = intersectSingle(m_frontSurface, ray, t1, p1, n1);
    bool hit2 = intersectSingle(m_backSurface, ray, t2, p2, n2);

    double bestT = RAY_INFINITY;
    if (hit1 && t1 < bestT) {
        bestT = t1;
        point = p1;
        normal = n1;
    }
    if (hit2 && t2 < bestT) {
        bestT = t2;
        point = p2;
        normal = n2;
    }

    if (bestT >= RAY_INFINITY) return false;
    t = bestT;
    return true;
}

void Lens::process(Ray& ray, const QPointF& point, const QVector2D& normal) const
{
    // Определяем, какая поверхность (передняя/задняя) по расстоянию до вершины
    QPointF v1 = vertex1();
    QPointF v2 = vertex2();
    double d1 = (point - v1).manhattanLength();
    double d2 = (point - v2).manhattanLength();

    if (d1 < d2) {
        // Передняя поверхность
        m_frontSurface.process(ray, point, normal);
    } else {
        // Задняя поверхность
        m_backSurface.process(ray, point, normal);
    }
}

QString Lens::info() const
{
    return QString("%1: center=(%2,%3) th=%4 R1=%5 R2=%6 D=%7 n=%8")
    .arg(name())
        .arg(m_center.x()).arg(m_center.y())
        .arg(m_thickness)
        .arg(m_R1).arg(m_R2)
        .arg(m_diameter)
        .arg(m_n_lens);
}
