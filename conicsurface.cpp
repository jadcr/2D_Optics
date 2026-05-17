#include "conicsurface.h"
#include "constants.h"
#include <QtMath>

ConicSurface::ConicSurface(const QPointF& vertex, double radius, double k, double diameter,
                           double n1, double n2, bool reflective, const QString& name)
    : OpticalElement(name)
    , m_vertex(vertex)
    , m_R(radius)
    , m_k(k)
    , m_diameter(diameter)
{
    setN1(n1);
    setN2(n2);
    setReflective(reflective);
}

bool ConicSurface::intersect(const Ray& ray, double& t, QPointF& point, QVector2D& normal) const
{
    const double eps = 1e-9;
    QVector2D dir = ray.direction();
    QPointF org = ray.origin();

    double dx = dir.x(), dy = dir.y();
    double ox = org.x(), oy = org.y();
    double vx = m_vertex.x(), vy = m_vertex.y();
    double R = m_R;
    double k = m_k;

    // случай: R == 0 – плоскость (x = x_v)
    if (fabs(R) < eps) {
        if (fabs(dx) < eps) return false; // луч параллелен плоскости
        double t_plane = (vx - ox) / dx;
        if (t_plane <= eps) return false;
        QPointF p = ray.pointAt(t_plane);
        if (fabs(p.y() - vy) > m_diameter/2.0 + eps) return false;
        point = p;
        t = t_plane;
        normal = QVector2D(1.0, 0.0); // нормаль вдоль оси X
        return true;
    }

    double A = (1.0 + k) * dx * dx + dy * dy;
    double B = 2.0 * ((1.0 + k) * (ox - vx) * dx + (oy - vy) * dy) - 2.0 * R * dx;
    double C = (1.0 + k) * (ox - vx) * (ox - vx) - 2.0 * R * (ox - vx) + (oy - vy) * (oy - vy);

    double t_intersect = RAY_INFINITY;
    bool found = false;
    QPointF bestPoint;
    QVector2D bestNormal;

    //Вырожденный случай A ≈ 0 (парабола и луч параллельно оси)
    if (fabs(A) < eps) {
        if (fabs(B) < eps) return false;
        double t_lin = -C / B;
        if (t_lin > eps) {
            QPointF p = ray.pointAt(t_lin);
            // Выбор ветви в зависимости от знака R
            bool xOk = false;
            if (R > 0)      xOk = (p.x() >= vx - eps);
            else if (R < 0) xOk = (p.x() <= vx + eps);
            else            xOk = true;
            if (xOk && fabs(p.y() - vy) <= m_diameter/2.0 + eps) {
                t_intersect = t_lin;
                bestPoint = p;
                found = true;
            }
        }
    }
    //Полный квадратный случай
    else {
        double disc = B*B - 4.0*A*C;
        if (disc < -eps) return false;
        if (disc < 0.0) disc = 0.0;
        double sqrtD = sqrt(disc);
        double t1 = (-B - sqrtD) / (2.0*A);
        double t2 = (-B + sqrtD) / (2.0*A);
        // Проверяем оба корня, берём наименьший положительный
        for (double t_cand : {t1, t2}) {
            if (t_cand > eps && t_cand < t_intersect) {
                QPointF p = ray.pointAt(t_cand);
                // Условие на нужную ветвь: правая для R>0, левая для R<0
                bool xOk = false;
                if (R > 0)      xOk = (p.x() >= vx - eps);
                else if (R < 0) xOk = (p.x() <= vx + eps);
                else            xOk = true;
                if (xOk && fabs(p.y() - vy) <= m_diameter/2.0 + eps) {
                    t_intersect = t_cand;
                    bestPoint = p;
                    found = true;
                }
            }
        }
    }

    if (!found) return false;

    t = t_intersect;
    point = bestPoint;

    // Вычисление нормали (градиент)
    double Fx = 2.0*(1.0 + k)*(point.x() - vx) - 2.0*R;
    double Fy = 2.0*(point.y() - vy);
    QVector2D norm(Fx, Fy);
    norm.normalize();
    if (norm.length() < eps) norm = QVector2D(1.0, 0.0);
    normal = norm;

    return true;
}

void ConicSurface::process(Ray& ray, const QPointF& point, const QVector2D& normal) const
{
    // Нормаль должна быть единичной
    QVector2D n = normal;
    if (qAbs(n.length() - 1.0) > 1e-6)
        n.normalize();

    QVector2D dir = ray.direction();
    double dot = QVector2D::dotProduct(dir, n);

    // Определяем, с какой стороны пришёл луч и показатели преломления
    double n_incident, n_transmit;
    double cosTheta1 = qAbs(dot);
    if (dot < -1e-9) {
        // Луч падает со стороны, куда направлена нормаль
        n_incident = n1();
        n_transmit = n2();
        cosTheta1 = -dot;
    } else if (dot > 1e-9) {
        // Луч падает с противоположной стороны
        n_incident = n2();
        n_transmit = n1();
        cosTheta1 = dot;
    } else {
        // Луч скользит вдоль поверхности – деактивируем
        ray.setActive(false);
        return;
    }

    // Отражающая поверхность
    if (isReflective()) {
        QVector2D reflected = dir - 2.0 * dot * n;
        ray.setDirection(reflected);
        ray.setActive(true);
        return;
    }

    // Преломляющая поверхность
    double eta = n_incident / n_transmit;
    double sin2Theta2 = eta * eta * (1.0 - cosTheta1 * cosTheta1);
    if (sin2Theta2 > 1.0) {
        // Полное внутреннее отражение
        QVector2D reflected = dir - 2.0 * dot * n;
        ray.setDirection(reflected);
    } else {
        double cosTheta2 = qSqrt(1.0 - sin2Theta2);
        QVector2D refracted = eta * dir + (eta * cosTheta1 - cosTheta2) * n;
        ray.setDirection(refracted);
    }
    ray.setActive(true);
}

QString ConicSurface::info() const
{
    return QString("%1: vertex=(%2,%3) R=%4 k=%5 D=%6 n1=%7 n2=%8 %9")
    .arg(name())
        .arg(m_vertex.x()).arg(m_vertex.y())
        .arg(m_R).arg(m_k).arg(m_diameter)
        .arg(n1()).arg(n2())
        .arg(isReflective() ? "reflective" : "refractive");
}
