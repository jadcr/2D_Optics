#ifndef CONICSURFACE_H
#define CONICSURFACE_H

#include "opticalelement.h"

class ConicSurface : public OpticalElement
{
public:
    ConicSurface(const QPointF& vertex, double radius, double k, double diameter,
                 double n1 = 1.0, double n2 = 1.5, bool reflective = false,
                 const QString& name = "Conic");

    QPointF vertex() const { return m_vertex; }
    double radius() const { return m_R; }
    double k() const { return m_k; }
    double diameter() const { return m_diameter; }

    void setVertex(const QPointF& v) { m_vertex = v; }
    void setRadius(double r) { m_R = r; }
    void setK(double k) { m_k = k; }
    void setDiameter(double d) { m_diameter = d; }

    bool intersect(const Ray& ray, double& t, QPointF& point, QVector2D& normal) const override;
    void process(Ray& ray, const QPointF& point, const QVector2D& normal) const override;
    QString info() const override;

private:
    QPointF m_vertex;      // вершина поверхности (на оптической оси)
    double m_R;            // радиус кривизны при вершине (может быть отрицательным)
    double m_k;            // коническая константа
    double m_diameter;     // апертура (диаметр)
};

#endif // CONICSURFACE_H
