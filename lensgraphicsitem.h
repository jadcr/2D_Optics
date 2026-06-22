#ifndef LENSGRAPHICSITEM_H
#define LENSGRAPHICSITEM_H

#include <QGraphicsItem>
#include "lens.h"

class MainWindow;

class LensGraphicsItem : public QGraphicsItem
{
public:
    LensGraphicsItem(Lens* lens, MainWindow* mw, QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
    Lens* m_lens;
    MainWindow* m_mainWindow;
};

#endif // LENSGRAPHICSITEM_H
