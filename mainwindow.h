#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "histogramwidget.h"
#include "movableitem.h"
#include "spotdiagramwidget.h"
#include <QMainWindow>
#include <QGraphicsScene>
#include <QList>
#include <QGraphicsItem>
#include <QListWidgetItem>
#include <qvectornd.h>

class Scene;
class RayTracer;
class OpticalElement;
class CoordinateGrid;

struct SourceParams {
    int type = 0;                 // 0 – параллельный пучок, 1 – точечный источник
    QPointF origin = QPointF(-80, 40);
    QVector2D direction = QVector2D(1, 0);
    double spacing = 5.0;
    int rayCount = 20;
    double angleSpread = 30.0;    // градусы
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    const SourceParams& sourceParams() const { return m_sourceParams; }

    void drawRays();
    void regenerateRays();
    void sourceMoved(const QPointF& newPos);
    void elementMoved(OpticalElement* element);

private slots:
    void on_actionAddPlane_triggered();
    void on_actionAddSpherical_triggered();
    void on_actionAddDetector_triggered();
    void on_actionTraceRays_triggered();
    void on_actionClearRays_triggered();
    void on_actionClearAll_triggered();
    void on_listWidget_itemClicked(QListWidgetItem* item);
    void on_applyButton_clicked();
    void on_actionRemoveElement_triggered();
    void on_applySource_clicked();
    void on_actionAddConic_triggered();

    void on_actionSpotDiagram_triggered();
    void updateSpotDiagram();


private:
    QGraphicsScene *m_graphicsScene;
    Scene *m_scene;
    RayTracer *m_rayTracer;
    QList<QGraphicsItem*> m_rayGraphicsItems;
    CoordinateGrid *m_coordinateGrid;
    OpticalElement* m_currentElement = nullptr;

    QDialog *m_spotDialog = nullptr;

    QMap<OpticalElement*, QGraphicsItem*> m_elementToItem;
    void registerGraphicsItem(OpticalElement* elem, QGraphicsItem* item);
    void setMovableForSelectedOnly();

    void updateElementsList();
    void addElementToScene(OpticalElement* elem);
    void clearRayGraphics();
    void setupGrid();

    SourceParams m_sourceParams;
    bool m_hasLastRayParams = false;
    void generateAndTraceRays();

    MovableSourceItem* m_sourceItem = nullptr;
    void createSourceItem();
    void updateSourceItemGraphics();

    SpotDiagramWidget *m_spotWidget = nullptr;
    HistogramWidget *m_histogramWidget = nullptr;
};

#endif // MAINWINDOW_H
