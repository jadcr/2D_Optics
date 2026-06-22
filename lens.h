#ifndef LENS_H
#define LENS_H

#include "conicsurface.h"
#include "opticalelement.h"

class Lens : public OpticalElement
{
public:
    Lens(const QPointF& center, double thickness,
         double R1, double R2,
         double k1 = 0.0, double k2 = 0.0,
         double diameter = 50.0,
         double n_lens = 1.5,
         const QString& name = "Линза");

    // Геттеры
    QPointF center() const { return m_center; }
    double thickness() const { return m_thickness; }
    double radius1() const { return m_R1; }
    double radius2() const { return m_R2; }
    double k1() const { return m_k1; }
    double k2() const { return m_k2; }
    double diameter() const { return m_diameter; }
    double nLens() const { return m_n_lens; }

    // Сеттеры
    void setCenter(const QPointF& c);
    void setThickness(double t);
    void setRadius1(double r);
    void setRadius2(double r);
    void setK1(double k);
    void setK2(double k);
    void setDiameter(double d);
    void setNLens(double n);

    // Переопределённые методы OpticalElement
    bool intersect(const Ray& ray, double& t, QPointF& point, QVector2D& normal) const override;
    void process(Ray& ray, const QPointF& point, const QVector2D& normal) const override;
    QString info() const override;

private:
    QPointF m_center;
    double m_thickness;
    double m_R1, m_R2;
    double m_k1, m_k2;
    double m_diameter;
    double m_n_lens;

    // Внутренние поверхности
    mutable ConicSurface m_frontSurface;
    mutable ConicSurface m_backSurface;

    // Вспомогательные методы для получения вершин
    QPointF vertex1() const;
    QPointF vertex2() const;

    // Вспомогательный метод пересечения с одной поверхностью
    bool intersectSingle(const ConicSurface& surf, const Ray& ray, double& t, QPointF& point, QVector2D& normal) const;
};

#endif // LENS_H
