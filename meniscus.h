#ifndef MENISCUS_H
#define MENISCUS_H

#include "lens.h"

class Meniscus : public Lens
{
public:
    /**
     * @param center   центр линзы (середина по оси X)
     * @param thickness толщина по оси
     * @param R1       радиус передней поверхности (положительный, выпуклая влево)
     * @param R2       радиус задней поверхности (положительный, выпуклая влево)
     * @param diameter апертура (диаметр)
     * @param n_lens   показатель преломления материала
     * @param name     имя элемента
     */
    Meniscus(const QPointF& center = QPointF(0, 0),
             double thickness = 12.0,
             double R1 = 200.0,
             double R2 = 210.0,
             double diameter = 60.0,
             double n_lens = 1.5168,
             const QString& name = "Мениск");
};

#endif // MENISCUS_H
