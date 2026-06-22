#include "ray.h"
#include <QtMath>

//инкапсулируем состояние луча

Ray::Ray(const QPointF& origin, const QVector2D& direction)
    : m_origin(origin)
    , m_direction(direction.normalized())
    , m_active(true)
    , m_initialY(0.0)
{
    m_path.append(origin);
}

Ray::Ray(const Ray& other)
    : m_origin(other.m_origin)
    , m_direction(other.m_direction)
    , m_active(other.m_active)
    , m_path(other.m_path)
    , m_initialY(other.m_initialY)
{
}

QPointF Ray::pointAt(double t) const
{
    return m_origin + t * m_direction.toPointF(); //начало + t * направление
}

void Ray::propagate(double t)
{
    m_origin = pointAt(t);
    addPoint(m_origin);
}

void Ray::setDirection(const QVector2D& dir)
{
    m_direction = dir.normalized();
}

void Ray::addPoint(const QPointF& pt)
{
    m_path.append(pt);
}

void Ray::clearPath()
{
    m_path.clear();
    m_path.append(m_origin);
}
