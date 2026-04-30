#ifndef CONICGRAPHICSITEM_H
#define CONICGRAPHICSITEM_H

#include <QGraphicsItem>
#include "conicsurface.h"

class MainWindow;

class ConicGraphicsItem : public QGraphicsItem
{
public:
    ConicGraphicsItem(ConicSurface* surface, MainWindow* mw, QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
    ConicSurface* m_surface;
    MainWindow* m_mainWindow;
};

#endif // CONICGRAPHICSITEM_H
