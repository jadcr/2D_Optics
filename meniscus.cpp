#include "meniscus.h"

Meniscus::Meniscus(const QPointF& center, double thickness,
                   double R1, double R2,
                   double diameter, double n_lens,
                   const QString& name)
    : Lens(center, thickness, R1, R2, 0.0, 0.0, diameter, n_lens, name)
{
    // Все параметры передаются в конструктор Lens.
    // Конические константы k1 и k2 равны 0 (сферические поверхности).
}
