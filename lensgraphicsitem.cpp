#include "lensgraphicsitem.h"
#include "mainwindow.h"
#include <QPainter>
#include <QtMath>

LensGraphicsItem::LensGraphicsItem(Lens* lens, MainWindow* mw, QGraphicsItem* parent)
    : QGraphicsItem(parent)
    , m_lens(lens)
    , m_mainWindow(mw)
{
    setPos(lens->center());
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setZValue(50);
}

QRectF LensGraphicsItem::boundingRect() const
{
    double Rmax = qMax(qAbs(m_lens->radius1()), qAbs(m_lens->radius2()));
    double D = m_lens->diameter();
    double margin = 20.0;
    double w = qMax(Rmax, D/2.0) + margin;
    return QRectF(-w, -w, 2*w, 2*w);
}

void LensGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->save();
    painter->setPen(QPen(Qt::blue, 2));
    painter->setBrush(Qt::NoBrush);

    double R1 = m_lens->radius1();
    double R2 = m_lens->radius2();
    double halfD = m_lens->diameter() / 2.0;
    double thick = m_lens->thickness();

    QPointF v1(-thick/2, 0);
    QPointF v2(thick/2, 0);

    // Передняя поверхность (R1 > 0 — выпуклая влево)
    if (qFabs(R1) > 1e-9) {
        double cx1 = v1.x() + R1;
        QPainterPath path;
        bool first = true;
        int steps = 100;
        for (int i = 0; i <= steps; ++i) {
            double y = -halfD + (2.0 * halfD) * i / steps;
            double disc = R1*R1 - y*y;
            if (disc < 0) continue;
            double sqrtDisc = qSqrt(disc);
            double x;
            if (R1 > 0)
                x = cx1 - sqrtDisc;   // левая часть окружности
            else
                x = cx1 + sqrtDisc;   // правая часть
            if (first) {
                path.moveTo(x, y);
                first = false;
            } else {
                path.lineTo(x, y);
            }
        }
        painter->drawPath(path);
    } else {
        painter->drawLine(QPointF(v1.x(), -halfD), QPointF(v1.x(), halfD));
    }

    // Задняя поверхность (R2 < 0 — выпуклая вправо)
    if (qFabs(R2) > 1e-9) {
        double cx2 = v2.x() + R2;
        QPainterPath path;
        bool first = true;
        int steps = 100;
        for (int i = 0; i <= steps; ++i) {
            double y = -halfD + (2.0 * halfD) * i / steps;
            double disc = R2*R2 - y*y;
            if (disc < 0) continue;
            double sqrtDisc = qSqrt(disc);
            double x;
            if (R2 > 0)
                x = cx2 - sqrtDisc;
            else
                x = cx2 + sqrtDisc;   // для R2<0 – правая часть (выпуклая вправо)
            if (first) {
                path.moveTo(x, y);
                first = false;
            } else {
                path.lineTo(x, y);
            }
        }
        painter->drawPath(path);
    } else {
        painter->drawLine(QPointF(v2.x(), -halfD), QPointF(v2.x(), halfD));
    }

    // Боковые линии (оправа)
    painter->drawLine(QPointF(v1.x(), -halfD), QPointF(v2.x(), -halfD));
    painter->drawLine(QPointF(v1.x(), halfD), QPointF(v2.x(), halfD));

    // Оптическая ось (пунктир)
    painter->setPen(QPen(Qt::gray, 1, Qt::DashLine));
    painter->drawLine(QPointF(-halfD, 0), QPointF(halfD, 0));

    painter->restore();
}

QVariant LensGraphicsItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemPositionHasChanged && m_mainWindow) {
        m_lens->setCenter(pos());
        m_mainWindow->elementMoved(m_lens);
    }
    return QGraphicsItem::itemChange(change, value);
}
