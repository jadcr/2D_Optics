#include "conicgraphicsitem.h"
#include "mainwindow.h"
#include <QPainter>
#include <QtMath>

ConicGraphicsItem::ConicGraphicsItem(ConicSurface* surface, MainWindow* mw, QGraphicsItem* parent)
    : QGraphicsItem(parent)
    , m_surface(surface)
    , m_mainWindow(mw)
{
    setPos(surface->vertex());
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setZValue(50);
}

QRectF ConicGraphicsItem::boundingRect() const
{
    double R = qAbs(m_surface->radius());
    double D = m_surface->diameter();
    double maxDim = qMax(R, D / 2.0) + 10.0;
    return QRectF(-maxDim, -maxDim, 2.0 * maxDim, 2.0 * maxDim);
}

void ConicGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->save();
    painter->setPen(QPen(Qt::blue, 2));
    painter->setBrush(Qt::NoBrush);

    double R = m_surface->radius();
    double k = m_surface->k();
    double halfD = m_surface->diameter() / 2.0;

    QPainterPath path;
    bool first = true;
    int steps = 200; // больше шагов для гладкости
    for (int i = 0; i <= steps; ++i) {
        double y = -halfD + (2.0 * halfD) * i / steps;
        double x;
        if (qAbs(k + 1.0) < 1e-9) {
            // парабола
            if (qAbs(R) < 1e-9) continue;
            x = (y * y) / (2.0 * R);
        } else {
            double disc = R*R - (1.0 + k) * y*y;
            if (disc < 0) continue;
            double sqrtDisc = sqrt(disc);
            x = (R - sqrtDisc) / (1.0 + k);
        }
        // Проверка на конечность
        if (!std::isfinite(x)) continue;
        if (first) {
            path.moveTo(x, y);
            first = false;
        } else {
            path.lineTo(x, y);
        }
    }
    painter->drawPath(path);

    painter->restore();
}

QVariant ConicGraphicsItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemPositionHasChanged && m_mainWindow)
    {
        // Обновляем вершину в логическом элементе
        m_surface->setVertex(pos());
        // Сообщаем главному окну, что элемент перемещён
        m_mainWindow->elementMoved(m_surface);
    }
    return QGraphicsItem::itemChange(change, value);
}
