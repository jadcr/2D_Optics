#include "spotdiagramwidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <cmath>

SpotDiagramWidget::SpotDiagramWidget(QWidget *parent)
    : QWidget(parent)
    , m_airyRadius(0.0)
    , m_scale(50.0)
    , m_offset(0.0, 0.0)
    , m_panning(false)
{
    setMinimumSize(300, 300);
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}

void SpotDiagramWidget::setPoints(const QList<QPointF>& points)
{
    m_points = points;
    update();
}

void SpotDiagramWidget::clear()
{
    m_points.clear();
    update();
}

void SpotDiagramWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), Qt::white);

    painter.setPen(QPen(Qt::lightGray, 1, Qt::DashLine));
    QRectF visibleRect(screenToWorld(QPoint(0,0)), screenToWorld(QPoint(width(), height())));
    double step = 10.0;
    double startX = std::floor(visibleRect.left() / step) * step;
    double endX   = std::ceil(visibleRect.right() / step) * step;
    double startY = std::floor(visibleRect.bottom() / step) * step;
    double endY   = std::ceil(visibleRect.top() / step) * step;

    for (double x = startX; x <= endX; x += step) {
        QPointF p1 = worldToScreen(QPointF(x, startY));
        QPointF p2 = worldToScreen(QPointF(x, endY));
        painter.drawLine(p1, p2);
    }
    for (double y = startY; y <= endY; y += step) {
        QPointF p1 = worldToScreen(QPointF(startX, y));
        QPointF p2 = worldToScreen(QPointF(endX, y));
        painter.drawLine(p1, p2);
    }

    // Оси
    painter.setPen(QPen(Qt::black, 1.5));
    QPointF originScreen = worldToScreen(QPointF(0,0));
    painter.drawLine(originScreen.x(), 0, originScreen.x(), height());
    painter.drawLine(0, originScreen.y(), width(), originScreen.y());

    // Круг Эйри
    if (m_airyRadius > 0) {
        painter.setPen(QPen(Qt::blue, 1.5, Qt::DashLine));
        QPointF center = worldToScreen(QPointF(0,0));
        double r = m_airyRadius * m_scale;
        painter.drawEllipse(center, r, r);
    }

    // Точки попаданий
    painter.setPen(QPen(Qt::red, 1));
    painter.setBrush(QBrush(Qt::red));
    for (const QPointF& p : m_points) {
        QPointF screen = worldToScreen(p);
        painter.drawEllipse(screen, 2, 2);
    }

    // Центроид и статистика
    if (!m_points.isEmpty()) {
        double sumX = 0, sumY = 0;
        for (const QPointF& p : m_points) {
            sumX += p.x();
            sumY += p.y();
        }
        QPointF centroid(sumX / m_points.size(), sumY / m_points.size());
        double rms = 0;
        for (const QPointF& p : m_points) {
            double dx = p.x() - centroid.x();
            double dy = p.y() - centroid.y();
            rms += dx*dx + dy*dy;
        }
        rms = std::sqrt(rms / m_points.size());

        painter.setPen(QPen(Qt::green, 2));
        QPointF screenCent = worldToScreen(centroid);
        painter.drawLine(screenCent.x() - 8, screenCent.y(), screenCent.x() + 8, screenCent.y());
        painter.drawLine(screenCent.x(), screenCent.y() - 8, screenCent.x(), screenCent.y() + 8);

        painter.setPen(Qt::black);
        painter.setFont(QFont("Arial", 9));
        painter.drawText(10, 20, QString("Points: %1   RMS: %2 mm").arg(m_points.size()).arg(rms, 0, 'f', 5));
    }
}

void SpotDiagramWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        m_panning = true;
        m_lastPan = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
    } else {
        QWidget::mousePressEvent(event);
    }
}

void SpotDiagramWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_panning) {
        QPointF deltaWorld = screenToWorld(event->pos()) - screenToWorld(m_lastPan);
        m_offset -= deltaWorld;
        m_lastPan = event->pos();
        update();
        event->accept();
    } else {
        QWidget::mouseMoveEvent(event);
    }
}

void SpotDiagramWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton && m_panning) {
        m_panning = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
    } else {
        QWidget::mouseReleaseEvent(event);
    }
}

void SpotDiagramWidget::wheelEvent(QWheelEvent *event)
{
    double factor = 1.1;
    if (event->angleDelta().y() > 0)
        m_scale *= factor;
    else
        m_scale /= factor;
    if (m_scale < 1.0) m_scale = 1.0;
    if (m_scale > 1000.0) m_scale = 1000.0;
    update();
}

QPointF SpotDiagramWidget::worldToScreen(const QPointF& world) const
{
    double x = width() / 2.0 + (world.x() - m_offset.x()) * m_scale;
    double y = height() / 2.0 - (world.y() - m_offset.y()) * m_scale;
    return QPointF(x, y);
}

QPointF SpotDiagramWidget::screenToWorld(const QPoint& screen) const
{
    double x = (screen.x() - width() / 2.0) / m_scale + m_offset.x();
    double y = -(screen.y() - height() / 2.0) / m_scale + m_offset.y();
    return QPointF(x, y);
}
