#include "mainwindow.h"
#include "movableitem.h"
#include "scene.h"
#include "raytracer.h"
#include "straightinterface.h"
#include "sphericalinterface.h"
#include "detector.h"
#include "graphicsview.h"
#include "coordgrid.h"
#include "conicsurface.h"
#include "conicgraphicsitem.h"
#include "spotdiagramwidget.h"

#include "lens.h"
#include "lensgraphicsitem.h"

#include "histogramwidget.h"
#include <QDialog>
#include <QSplitter>
#include <QLabel>

#include <QGraphicsLineItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolBar>
#include <QDockWidget>
#include <QListWidget>
#include <QTabWidget>
#include <QFormLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QGraphicsView>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QGraphicsEllipseItem>
#include <QVariant>
#include <QPen>
#include <QPainter>
#include <qevent.h>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QFormLayout>

#include <QDialog>
#include <QVBoxLayout>
#include <QMenu>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_graphicsScene(new QGraphicsScene(this))
    , m_scene(new Scene())
    , m_rayTracer(new RayTracer(this))
    , m_coordinateGrid(nullptr)
{

    m_hasLastRayParams = false;
    // Центральный виджет
    GraphicsView *view = new GraphicsView(this);

    view->setScene(m_graphicsScene);
    setCentralWidget(view);



    // Действия
    QAction *actionAddPlane = new QAction("Добавить плоскость", this);
    QAction *actionAddSpherical = new QAction("Добавить круг", this);
    QAction *actionAddDetector = new QAction("Добавить детектор", this);
    QAction *actionTraceRays = new QAction("Трассировать лучи", this);
    QAction *actionClearRays = new QAction("Очистить лучи", this);
    QAction *actionClearAll = new QAction("Очистить всё", this);
    QAction *actionRemoveElement = new QAction("Удалить элемент", this);
    QAction *actionAddConic = new QAction("Добавить коническую поверхность", this);

    QAction *actionSpotDiagram = new QAction("Диаграмма рассеяния", this);
    QAction *actionAddLens = new QAction("Добавить линзу", this);

    // Меню


    //QMenu *elementsMenu = menuBar()->addMenu("Элементы");
    //elementsMenu->addAction(actionAddPlane);
    //elementsMenu->addAction(actionAddSpherical);
    //elementsMenu->addAction(actionAddDetector);
    //elementsMenu->addAction(actionRemoveElement);
    //elementsMenu->addAction(actionAddConic);

    QMenu *traceMenu = menuBar()->addMenu("Трассировка");
    traceMenu->addAction(actionTraceRays);
    traceMenu->addAction(actionClearRays);
    traceMenu->addAction(actionClearAll);

    // Панель инструментов
    QToolBar *toolBar = addToolBar("Инструменты");
    toolBar->addAction(actionAddPlane);
    toolBar->addAction(actionAddSpherical);
    toolBar->addAction(actionAddConic);
    toolBar->addAction(actionAddDetector);
    toolBar->addSeparator();
    toolBar->addAction(actionTraceRays);
    toolBar->addAction(actionClearRays);
    toolBar->addAction(actionRemoveElement);
    toolBar->addAction(actionAddLens);

    QMenu *analysisMenu = menuBar()->addMenu("Анализ");
    analysisMenu->addAction(actionSpotDiagram);

    // Док-панель списка элементов
    QDockWidget *listDock = new QDockWidget("Список элементов", this);
    listDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    QListWidget *listWidget = new QListWidget(listDock);
    listDock->setWidget(listWidget);
    addDockWidget(Qt::LeftDockWidgetArea, listDock);

    // Док-панель свойств
    QDockWidget *propDock = new QDockWidget("Свойства элемента", this);
    propDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    QWidget *propWidget = new QWidget(propDock);
    QVBoxLayout *propLayout = new QVBoxLayout(propWidget);

    QTabWidget *tabWidget = new QTabWidget;
    propLayout->addWidget(tabWidget);

    // Вкладка "Общие"
    QWidget *generalTab = new QWidget;
    QFormLayout *generalForm = new QFormLayout(generalTab);
    QLineEdit *nameEdit = new QLineEdit;
    generalForm->addRow("Имя:", nameEdit);
    QCheckBox *reflectiveCheck = new QCheckBox("Отражающий");
    generalForm->addRow("Тип:", reflectiveCheck);
    tabWidget->addTab(generalTab, "Общие");

    // Вкладка "Геометрия"
    QWidget *geomTab = new QWidget;
    QFormLayout *geomForm = new QFormLayout(geomTab);
    QDoubleSpinBox *x1Spin = new QDoubleSpinBox;
    x1Spin->setRange(-1000, 1000);
    QDoubleSpinBox *y1Spin = new QDoubleSpinBox;
    y1Spin->setRange(-1000, 1000);
    QDoubleSpinBox *x2Spin = new QDoubleSpinBox;
    x2Spin->setRange(-1000, 1000);
    QDoubleSpinBox *y2Spin = new QDoubleSpinBox;
    y2Spin->setRange(-1000, 1000);

    QDoubleSpinBox *diameterSpin = new QDoubleSpinBox;
    diameterSpin->setRange(1, 1000);
    diameterSpin->setValue(50.0);

    QLabel *focalLabel = new QLabel("Фокусное расстояние: --");
    focalLabel->setObjectName("focalLabel");

    geomForm->addRow("X1:", x1Spin);
    geomForm->addRow("Y1:", y1Spin);
    geomForm->addRow("X2:", x2Spin);
    geomForm->addRow("Y2:", y2Spin);
    geomForm->addRow("Диаметр:", diameterSpin);
    geomForm->addRow("Фокус:", focalLabel);
    tabWidget->addTab(geomTab, "Геометрия");

    // Вкладка "Материалы"
    QWidget *materialTab = new QWidget;
    QFormLayout *materialForm = new QFormLayout(materialTab);
    QDoubleSpinBox *n1Spin = new QDoubleSpinBox;
    n1Spin->setRange(1.0, 10.0);
    n1Spin->setValue(1.0);
    QDoubleSpinBox *n2Spin = new QDoubleSpinBox;
    n2Spin->setRange(1.0, 10.0);
    n2Spin->setValue(1.5);
    materialForm->addRow("n1 (среда 1):", n1Spin);
    materialForm->addRow("n2 (среда 2):", n2Spin);
    tabWidget->addTab(materialTab, "Материалы");

    // Вкладка "Источник"
    QWidget *sourceTab = new QWidget;
    QFormLayout *sourceForm = new QFormLayout(sourceTab);

    QComboBox *sourceTypeCombo = new QComboBox;
    sourceTypeCombo->addItem("Параллельный пучок");
    sourceTypeCombo->addItem("Точечный источник");
    sourceTypeCombo->setObjectName("sourceTypeCombo");
    sourceForm->addRow("Тип источника:", sourceTypeCombo);

    QDoubleSpinBox *sourceOriginX = new QDoubleSpinBox;
    sourceOriginX->setRange(-1000, 1000);
    sourceOriginX->setValue(-80.0);
    sourceOriginX->setObjectName("sourceOriginX");
    sourceForm->addRow("Начало X:", sourceOriginX);

    QDoubleSpinBox *sourceOriginY = new QDoubleSpinBox;
    sourceOriginY->setRange(-1000, 1000);
    sourceOriginY->setValue(40.0);
    sourceOriginY->setObjectName("sourceOriginY");
    sourceForm->addRow("Начало Y:", sourceOriginY);

    QDoubleSpinBox *sourceDirX = new QDoubleSpinBox;
    sourceDirX->setRange(-1, 1);
    sourceDirX->setSingleStep(0.1);
    sourceDirX->setValue(1.0);
    sourceDirX->setObjectName("sourceDirX");
    sourceForm->addRow("Направление X:", sourceDirX);

    QDoubleSpinBox *sourceDirY = new QDoubleSpinBox;
    sourceDirY->setRange(-1, 1);
    sourceDirY->setSingleStep(0.1);
    sourceDirY->setValue(0.0);
    sourceDirY->setObjectName("sourceDirY");
    sourceForm->addRow("Направление Y:", sourceDirY);

    QSpinBox *sourceRayCount = new QSpinBox;
    sourceRayCount->setRange(1, 500);
    sourceRayCount->setValue(20);
    sourceRayCount->setObjectName("sourceRayCount");
    sourceForm->addRow("Количество лучей:", sourceRayCount);

    QDoubleSpinBox *sourceSpacing = new QDoubleSpinBox;
    sourceSpacing->setRange(0.1, 100);
    sourceSpacing->setValue(5.0);
    sourceSpacing->setObjectName("sourceSpacing");
    sourceForm->addRow("Шаг (для пучка):", sourceSpacing);

    QDoubleSpinBox *sourceAngleSpread = new QDoubleSpinBox;
    sourceAngleSpread->setRange(0, 360);
    sourceAngleSpread->setValue(30.0);
    sourceAngleSpread->setObjectName("sourceAngleSpread");
    sourceForm->addRow("Угловой разброс (град):", sourceAngleSpread);

    QPushButton *applySourceButton = new QPushButton("Применить источник");
    applySourceButton->setObjectName("applySourceButton");
    sourceForm->addRow(applySourceButton);

    tabWidget->addTab(sourceTab, "Источник");

    QPushButton *applyButton = new QPushButton("Применить");
    propLayout->addWidget(applyButton);

    propDock->setWidget(propWidget);
    addDockWidget(Qt::RightDockWidgetArea, propDock);

    // Установка objectName для доступа
    listWidget->setObjectName("listWidget");
    nameEdit->setObjectName("nameEdit");
    reflectiveCheck->setObjectName("reflectiveCheck");
    x1Spin->setObjectName("x1Spin");
    y1Spin->setObjectName("y1Spin");
    x2Spin->setObjectName("x2Spin");
    y2Spin->setObjectName("y2Spin");
    n1Spin->setObjectName("n1Spin");
    n2Spin->setObjectName("n2Spin");
    applyButton->setObjectName("applyButton");
    diameterSpin->setObjectName("diameterSpin");

    // Подключение сигналов
    connect(actionAddPlane, &QAction::triggered, this, &MainWindow::on_actionAddPlane_triggered);
    connect(actionAddSpherical, &QAction::triggered, this, &MainWindow::on_actionAddSpherical_triggered);
    connect(actionAddDetector, &QAction::triggered, this, &MainWindow::on_actionAddDetector_triggered);
    connect(actionTraceRays, &QAction::triggered, this, &MainWindow::on_actionTraceRays_triggered);
    connect(actionClearRays, &QAction::triggered, this, &MainWindow::on_actionClearRays_triggered);
    connect(actionClearAll, &QAction::triggered, this, &MainWindow::on_actionClearAll_triggered);
    connect(listWidget, &QListWidget::itemClicked, this, &MainWindow::on_listWidget_itemClicked);
    connect(applyButton, &QPushButton::clicked, this, &MainWindow::on_applyButton_clicked);
    connect(actionRemoveElement, &QAction::triggered, this, &MainWindow::on_actionRemoveElement_triggered);
    connect(actionAddConic, &QAction::triggered, this, &MainWindow::on_actionAddConic_triggered);
    connect(actionSpotDiagram, &QAction::triggered, this, &MainWindow::on_actionSpotDiagram_triggered);
    connect(actionAddLens, &QAction::triggered, this, &MainWindow::on_actionAddLens_triggered);
    QPushButton *applySourceBtn = findChild<QPushButton*>("applySourceButton");
    if (applySourceBtn) {
        connect(applySourceBtn, &QPushButton::clicked, this, &MainWindow::on_applySource_clicked);
    }

    m_rayTracer->setScene(m_scene);

    setupGrid();
}

MainWindow::~MainWindow()
{
    delete m_scene;
    delete m_coordinateGrid;
}

void MainWindow::setupGrid()
{
    if (m_coordinateGrid) {
        m_graphicsScene->removeItem(m_coordinateGrid);
        delete m_coordinateGrid;
        m_coordinateGrid = nullptr;
    }
    m_coordinateGrid = new CoordinateGrid();
    m_graphicsScene->addItem(m_coordinateGrid);
}

void MainWindow::on_actionAddPlane_triggered()
{
    StraightInterface *plane = new StraightInterface(QPointF(-50, 20), QPointF(50, 20),
                                                     1.0, 1.5, false, "Плоскость");
    m_scene->addElement(plane);
    addElementToScene(plane);
    regenerateRays();
    updateElementsList();
}

void MainWindow::on_actionAddSpherical_triggered()
{
    SphericalInterface *sphere = new SphericalInterface(QPointF(0, 0), 30,
                                                        1.0, 1.5, false, "Круг");
    m_scene->addElement(sphere);
    addElementToScene(sphere);
    regenerateRays();
    updateElementsList();
}

void MainWindow::on_actionAddDetector_triggered()
{
    Detector *det = new Detector(QPointF(0, -30), QPointF(0, 0), "Детектор");
    m_scene->addElement(det);
    addElementToScene(det);
    regenerateRays();
    updateElementsList();
}

//void MainWindow::addElementToScene(OpticalElement* elem)(старая реализация)
//{
//    if (StraightInterface* line = dynamic_cast<StraightInterface*>(elem))
//    {
//       item->setPen(QPen(Qt::blue, 2));
//       item->setData(0, QVariant::fromValue((void*)elem));
//        m_graphicsScene->addItem(item);
//    }
//    else if (SphericalInterface* sphere = dynamic_cast<SphericalInterface*>(elem))
//    {
//        QGraphicsEllipseItem *item = new QGraphicsEllipseItem(
//            sphere->center().x() - sphere->radius(),
//            sphere->center().y() - sphere->radius(),
//            sphere->radius() * 2, sphere->radius() * 2);
//        item->setPen(QPen(Qt::blue, 2));
//        item->setData(0, QVariant::fromValue((void*)elem));
//        m_graphicsScene->addItem(item);
//    }
//    else if (Detector* det = dynamic_cast<Detector*>(elem))
//    {
//        QGraphicsLineItem *item = new QGraphicsLineItem(QLineF(det->p1(), det->p2()));
//        item->setPen(QPen(Qt::red, 3));
//        item->setData(0, QVariant::fromValue((void*)elem));
//        m_graphicsScene->addItem(item);
//    }
//}

void MainWindow::addElementToScene(OpticalElement* elem)
{
    QGraphicsItem* item = nullptr;
    if (StraightInterface* line = dynamic_cast<StraightInterface*>(elem)) {
        MovableLineItem* it = new MovableLineItem(elem, this, line->p1(), line->p2());
        m_graphicsScene->addItem(it);
        item = it;
    }
    else if (SphericalInterface* sphere = dynamic_cast<SphericalInterface*>(elem)) {
        MovableEllipseItem* it = new MovableEllipseItem(sphere, this, sphere->center(), sphere->radius());
        m_graphicsScene->addItem(it);
        item = it;
    }
    else if (ConicSurface* conic = dynamic_cast<ConicSurface*>(elem)) {
        ConicGraphicsItem* it = new ConicGraphicsItem(conic, this);
        m_graphicsScene->addItem(it);
        item = it;
    }
    else if (Detector* det = dynamic_cast<Detector*>(elem)) {
        MovableLineItem* it = new MovableLineItem(elem, this, det->p1(), det->p2());
        m_graphicsScene->addItem(it);
        item = it;
    }
    else if (Lens* lens = dynamic_cast<Lens*>(elem)) {
        LensGraphicsItem* it = new LensGraphicsItem(lens, this);
        m_graphicsScene->addItem(it);
        item = it;
    }
    if (item) {
        item->setFlag(QGraphicsItem::ItemIsMovable, false); // по умолчанию не двигается
        registerGraphicsItem(elem, item);
    }
}

void MainWindow::updateElementsList()
{
    QListWidget *list = findChild<QListWidget*>("listWidget");
    if (!list) return;
    list->clear();
    for (OpticalElement* elem : m_scene->elements())
    {
        QListWidgetItem *item = new QListWidgetItem(elem->name());
        item->setData(Qt::UserRole, QVariant::fromValue((void*)elem));
        list->addItem(item);
    }
}

void MainWindow::on_listWidget_itemClicked(QListWidgetItem* item)
{
    m_currentElement = (OpticalElement*)item->data(Qt::UserRole).value<void*>();
    setMovableForSelectedOnly();
    OpticalElement* elem = (OpticalElement*)item->data(Qt::UserRole).value<void*>();
    if (!elem) return;

    QLineEdit *nameEdit = findChild<QLineEdit*>("nameEdit");
    QCheckBox *reflectiveCheck = findChild<QCheckBox*>("reflectiveCheck");
    QDoubleSpinBox *x1Spin = findChild<QDoubleSpinBox*>("x1Spin");
    QDoubleSpinBox *y1Spin = findChild<QDoubleSpinBox*>("y1Spin");
    QDoubleSpinBox *x2Spin = findChild<QDoubleSpinBox*>("x2Spin");
    QDoubleSpinBox *y2Spin = findChild<QDoubleSpinBox*>("y2Spin");
    QDoubleSpinBox *n1Spin = findChild<QDoubleSpinBox*>("n1Spin");
    QDoubleSpinBox *n2Spin = findChild<QDoubleSpinBox*>("n2Spin");

    QDoubleSpinBox *diameterSpin = findChild<QDoubleSpinBox*>("diameterSpin");

    if (nameEdit) nameEdit->setText(elem->name());
    if (reflectiveCheck) reflectiveCheck->setChecked(elem->isReflective());
    if (n1Spin) n1Spin->setValue(elem->n1());
    if (n2Spin) n2Spin->setValue(elem->n2());

    if (StraightInterface* line = dynamic_cast<StraightInterface*>(elem))
    {
        if (x1Spin) { x1Spin->setValue(line->p1().x()); x1Spin->setEnabled(true); }
        if (y1Spin) { y1Spin->setValue(line->p1().y()); y1Spin->setEnabled(true); }
        if (x2Spin) { x2Spin->setValue(line->p2().x()); x2Spin->setEnabled(true); }
        if (y2Spin) { y2Spin->setValue(line->p2().y()); y2Spin->setEnabled(true); }
    }
    else if (SphericalInterface* sphere = dynamic_cast<SphericalInterface*>(elem))
    {
        if (x1Spin) { x1Spin->setValue(sphere->center().x()); x1Spin->setEnabled(true); }
        if (y1Spin) { y1Spin->setValue(sphere->center().y()); y1Spin->setEnabled(true); }
        if (x2Spin) { x2Spin->setValue(sphere->radius()); x2Spin->setEnabled(true); }
        if (y2Spin) y2Spin->setEnabled(false);
    }
    else if (Detector* det = dynamic_cast<Detector*>(elem))
    {
        if (x1Spin) { x1Spin->setValue(det->p1().x()); x1Spin->setEnabled(true); }
        if (y1Spin) { y1Spin->setValue(det->p1().y()); y1Spin->setEnabled(true); }
        if (x2Spin) { x2Spin->setValue(det->p2().x()); x2Spin->setEnabled(true); }
        if (y2Spin) { y2Spin->setValue(det->p2().y()); y2Spin->setEnabled(true); }
    }
    else if (ConicSurface* conic = dynamic_cast<ConicSurface*>(elem))   // <--- НОВАЯ ВЕТКА
    {
        if (x1Spin) { x1Spin->setValue(conic->vertex().x()); x1Spin->setEnabled(true); }
        if (y1Spin) { y1Spin->setValue(conic->vertex().y()); y1Spin->setEnabled(true); }
        if (x2Spin) { x2Spin->setValue(conic->radius()); x2Spin->setEnabled(true); }
        if (y2Spin) { y2Spin->setValue(conic->k()); y2Spin->setEnabled(true); }
        if (diameterSpin) { diameterSpin->setValue(conic->diameter()); diameterSpin->setEnabled(true); }
    }
    else if (Lens* lens = dynamic_cast<Lens*>(elem)) {
        if (x1Spin) { x1Spin->setValue(lens->center().x()); x1Spin->setEnabled(true); }
        if (y1Spin) { y1Spin->setValue(lens->center().y()); y1Spin->setEnabled(true); }
        if (x2Spin) { x2Spin->setValue(lens->radius1()); x2Spin->setEnabled(true); }
        if (y2Spin) { y2Spin->setValue(lens->radius2()); y2Spin->setEnabled(true); }
        if (diameterSpin) { diameterSpin->setValue(lens->diameter()); diameterSpin->setEnabled(true);}
        double n = lens->nLens();
        double R1 = lens->radius1();
        double R2 = lens->radius2();
        double f = 0;
        if (qAbs(R1) > 1e-9 && qAbs(R2) > 1e-9 && qAbs(n - 1.0) > 1e-9) {
            double invF = (n - 1.0) * (1.0 / R1 - 1.0 / R2);
            if (qAbs(invF) > 1e-9)
                f = 1.0 / invF;
        }
        QLabel* focalLabel = findChild<QLabel*>("focalLabel");
        if (focalLabel) {
            if (qAbs(f) > 1e-9)
                focalLabel->setText(QString("Фокусное расстояние: %1").arg(f, 0, 'f', 2));
            else
                focalLabel->setText("Фокусное расстояние: ∞ (плоскость)");
        }
    }

}

void MainWindow::on_applyButton_clicked()
{
    QListWidget *list = findChild<QListWidget*>("listWidget");
    if (!list || !list->currentItem()) return;

    OpticalElement* elem = (OpticalElement*)list->currentItem()->data(Qt::UserRole).value<void*>();
    if (!elem) return;

    QLineEdit *nameEdit = findChild<QLineEdit*>("nameEdit");
    QCheckBox *reflectiveCheck = findChild<QCheckBox*>("reflectiveCheck");
    QDoubleSpinBox *x1Spin = findChild<QDoubleSpinBox*>("x1Spin");
    QDoubleSpinBox *y1Spin = findChild<QDoubleSpinBox*>("y1Spin");
    QDoubleSpinBox *x2Spin = findChild<QDoubleSpinBox*>("x2Spin");
    QDoubleSpinBox *y2Spin = findChild<QDoubleSpinBox*>("y2Spin");
    QDoubleSpinBox *n1Spin = findChild<QDoubleSpinBox*>("n1Spin");
    QDoubleSpinBox *n2Spin = findChild<QDoubleSpinBox*>("n2Spin");

    QDoubleSpinBox *diameterSpin = findChild<QDoubleSpinBox*>("diameterSpin");

    if (nameEdit) elem->setName(nameEdit->text());
    if (reflectiveCheck) elem->setReflective(reflectiveCheck->isChecked());
    if (n1Spin) elem->setN1(n1Spin->value());
    if (n2Spin) elem->setN2(n2Spin->value());

    if (StraightInterface* line = dynamic_cast<StraightInterface*>(elem))
    {
        if (x1Spin && y1Spin && x2Spin && y2Spin)
            line->setPoints(QPointF(x1Spin->value(), y1Spin->value()),
                            QPointF(x2Spin->value(), y2Spin->value()));
    }
    else if (SphericalInterface* sphere = dynamic_cast<SphericalInterface*>(elem))
    {
        if (x1Spin && y1Spin && x2Spin)
        {
            sphere->setCenter(QPointF(x1Spin->value(), y1Spin->value()));
            sphere->setRadius(x2Spin->value());
        }
    }
    else if (Detector* det = dynamic_cast<Detector*>(elem))
    {
        if (x1Spin && y1Spin && x2Spin && y2Spin)
            det->setPoints(QPointF(x1Spin->value(), y1Spin->value()),
                           QPointF(x2Spin->value(), y2Spin->value()));
    }
    else if (ConicSurface* conic = dynamic_cast<ConicSurface*>(elem))   // <--- НОВАЯ ВЕТКА
    {
        if (x1Spin && y1Spin)
            conic->setVertex(QPointF(x1Spin->value(), y1Spin->value()));
        if (x2Spin)
            conic->setRadius(x2Spin->value());
        if (y2Spin)
            conic->setK(y2Spin->value());
        if (diameterSpin)
            conic->setDiameter(diameterSpin->value());
    }
    else if (Lens* lens = dynamic_cast<Lens*>(elem)) {
        if (x1Spin && y1Spin)
            lens->setCenter(QPointF(x1Spin->value(), y1Spin->value()));
        if (x2Spin)
            lens->setRadius1(x2Spin->value());
        if (y2Spin)
            lens->setRadius2(y2Spin->value());
        if (diameterSpin)
            lens->setDiameter(diameterSpin->value());
        QLabel* focalLabel = findChild<QLabel*>("focalLabel");
        if (focalLabel) {
            double n = lens->nLens();
            double R1 = lens->radius1();
            double R2 = lens->radius2();
            double f = 0;
            if (qAbs(R1) > 1e-9 && qAbs(R2) > 1e-9 && qAbs(n - 1.0) > 1e-9) {
                double invF = (n - 1.0) * (1.0 / R1 - 1.0 / R2);
                if (qAbs(invF) > 1e-9)
                    f = 1.0 / invF;
            }
            if (qAbs(f) > 1e-9)
                focalLabel->setText(QString("Фокусное расстояние: %1").arg(f, 0, 'f', 2));
            else
                focalLabel->setText("Фокусное расстояние: ∞");
        }
    }

    // Очистка графики
    clearRayGraphics();
    m_graphicsScene->clear();
    m_coordinateGrid = nullptr;

    // Пересоздать сетку
    setupGrid();

    createSourceItem();


    // Заново добавить элементы сцены
    for (OpticalElement* e : m_scene->elements())
        addElementToScene(e);

    // Перерисовать лучи
    regenerateRays();

    // Обновить список элементов
    updateElementsList();

    setMovableForSelectedOnly();
}

void MainWindow::on_applySource_clicked()
{
    QComboBox *sourceTypeCombo = findChild<QComboBox*>("sourceTypeCombo");
    QDoubleSpinBox *sourceOriginX = findChild<QDoubleSpinBox*>("sourceOriginX");
    QDoubleSpinBox *sourceOriginY = findChild<QDoubleSpinBox*>("sourceOriginY");
    QDoubleSpinBox *sourceDirX = findChild<QDoubleSpinBox*>("sourceDirX");
    QDoubleSpinBox *sourceDirY = findChild<QDoubleSpinBox*>("sourceDirY");
    QSpinBox *sourceRayCount = findChild<QSpinBox*>("sourceRayCount");
    QDoubleSpinBox *sourceSpacing = findChild<QDoubleSpinBox*>("sourceSpacing");
    QDoubleSpinBox *sourceAngleSpread = findChild<QDoubleSpinBox*>("sourceAngleSpread");

    if (sourceTypeCombo) m_sourceParams.type = sourceTypeCombo->currentIndex();
    if (sourceOriginX) m_sourceParams.origin.setX(sourceOriginX->value());
    if (sourceOriginY) m_sourceParams.origin.setY(sourceOriginY->value());
    if (sourceDirX && sourceDirY) m_sourceParams.direction = QVector2D(sourceDirX->value(), sourceDirY->value());
    if (sourceRayCount) m_sourceParams.rayCount = sourceRayCount->value();
    if (sourceSpacing) m_sourceParams.spacing = sourceSpacing->value();
    if (sourceAngleSpread) m_sourceParams.angleSpread = sourceAngleSpread->value();


    updateSourceItemGraphics();
    generateAndTraceRays();
}

void MainWindow::on_actionTraceRays_triggered()
{
    generateAndTraceRays();
}

void MainWindow::on_actionClearRays_triggered()
{
    m_rayTracer->clearRays();
    clearRayGraphics();
    m_hasLastRayParams = false;
}

void MainWindow::on_actionClearAll_triggered()
{
    m_scene->clearElements();
    m_elementToItem.clear();
    m_rayTracer->clearRays();
    clearRayGraphics();
    m_graphicsScene->clear();
    m_coordinateGrid = nullptr;

    setupGrid();
    createSourceItem();
    m_hasLastRayParams = false;
    updateElementsList();
}

void MainWindow::on_actionRemoveElement_triggered()
{
    QListWidget *list = findChild<QListWidget*>("listWidget");

    if (!list || !list->currentItem()) return;

    OpticalElement* elem = (OpticalElement*)list->currentItem()->data(Qt::UserRole).value<void*>();
    if (!elem) return;

    m_scene->removeElement(elem);

    clearRayGraphics();
\
    m_graphicsScene->clear();
    m_elementToItem.remove(elem);
    m_coordinateGrid = nullptr;

    setupGrid();
    createSourceItem();

    for (OpticalElement* e : m_scene->elements())
        addElementToScene(e);

    regenerateRays();

    updateElementsList();

    list->setCurrentItem(nullptr);
}

void MainWindow::drawRays()
{
    clearRayGraphics();

    QPen rayPen(Qt::green, 0.2);
    for (const Ray& ray : m_rayTracer->rays())
    {
        const QList<QPointF>& path = ray.path();
        for (int i = 0; i < path.size() - 1; ++i)
        {
            QGraphicsLineItem *line = new QGraphicsLineItem(QLineF(path[i], path[i+1]));
            line->setPen(rayPen);
            m_graphicsScene->addItem(line);
            m_rayGraphicsItems.append(line);
        }
    }
}

void MainWindow::regenerateRays()
{
    if (m_hasLastRayParams)
        generateAndTraceRays();
}

void MainWindow::generateAndTraceRays() {
    m_rayTracer->clearRays();
    clearRayGraphics();



    if (!m_sourceItem) {
        createSourceItem();
    } else {
        m_sourceItem->setPos(m_sourceParams.origin);
    }


    if (m_sourceParams.type == 0) { // параллельный пучок
        double xStart = m_sourceParams.origin.x();
        double yStart = m_sourceParams.origin.y();
        double spacing = m_sourceParams.spacing;
        QVector2D dir = m_sourceParams.direction.normalized();
        for (int i = 0; i < m_sourceParams.rayCount; ++i) {
            double y = yStart - i * spacing;
            Ray ray(QPointF(xStart, y), dir);
            m_rayTracer->addRay(ray);
        }
    } else { // точечный источник
        QPointF origin = m_sourceParams.origin;
        double angleSpread = m_sourceParams.angleSpread * M_PI / 180.0;
        double startAngle = -angleSpread/2;
        double step = (m_sourceParams.rayCount > 1) ? angleSpread/(m_sourceParams.rayCount-1) : 0;
        for (int i = 0; i < m_sourceParams.rayCount; ++i) {
            double angle = startAngle + i * step;
            QVector2D dir(qCos(angle), qSin(angle));
            Ray ray(origin, dir);
            m_rayTracer->addRay(ray);
        }
    }
    m_rayTracer->traceAll(50);
    m_hasLastRayParams = true;
}

void MainWindow::clearRayGraphics()
{
    for (QGraphicsItem* item : m_rayGraphicsItems)
    {
        m_graphicsScene->removeItem(item);
        delete item;
    }
    m_rayGraphicsItems.clear();

}

void MainWindow::createSourceItem()
{
    m_sourceItem = new MovableSourceItem(this, m_sourceParams.origin);
    m_graphicsScene->addItem(m_sourceItem);
}

void MainWindow::sourceMoved(const QPointF& newPos)
{
    m_sourceParams.origin = newPos;

    QDoubleSpinBox* originX = findChild<QDoubleSpinBox*>("sourceOriginX");
    QDoubleSpinBox* originY = findChild<QDoubleSpinBox*>("sourceOriginY");
    if (originX) originX->setValue(newPos.x());
    if (originY) originY->setValue(newPos.y());

    regenerateRays();
}

void MainWindow::updateSourceItemGraphics()
{
    if (m_sourceItem) {
        m_sourceItem->update(); // перерисовать
    }
}

void MainWindow::elementMoved(OpticalElement* element)
{

    regenerateRays();

    if (element == m_currentElement) {
        QDoubleSpinBox *x1Spin = findChild<QDoubleSpinBox*>("x1Spin");
        QDoubleSpinBox *y1Spin = findChild<QDoubleSpinBox*>("y1Spin");
        QDoubleSpinBox *x2Spin = findChild<QDoubleSpinBox*>("x2Spin");
        QDoubleSpinBox *y2Spin = findChild<QDoubleSpinBox*>("y2Spin");
        QDoubleSpinBox *diameterSpin = findChild<QDoubleSpinBox*>("diameterSpin");

        if (!x1Spin) return;

        if (auto* line = dynamic_cast<StraightInterface*>(element)) {
            x1Spin->setValue(line->p1().x());
            y1Spin->setValue(line->p1().y());
            x2Spin->setValue(line->p2().x());
            y2Spin->setValue(line->p2().y());
        } else if (auto* sphere = dynamic_cast<SphericalInterface*>(element)) {
            x1Spin->setValue(sphere->center().x());
            y1Spin->setValue(sphere->center().y());
            x2Spin->setValue(sphere->radius());

        } else if (auto* det = dynamic_cast<Detector*>(element)) {
            x1Spin->setValue(det->p1().x());
            y1Spin->setValue(det->p1().y());
            x2Spin->setValue(det->p2().x());
            y2Spin->setValue(det->p2().y());
        }
        else if (ConicSurface* conic = dynamic_cast<ConicSurface*>(element))
        {
            x1Spin->setValue(conic->vertex().x());
            y1Spin->setValue(conic->vertex().y());
            x2Spin->setValue(conic->radius());
            y2Spin->setValue(conic->k());
            if (diameterSpin) {
                diameterSpin->setValue(conic->diameter());
                diameterSpin->setEnabled(true);
            }
        }
        else if (Lens* lens = dynamic_cast<Lens*>(element)) {
            x1Spin->setValue(lens->center().x());
            y1Spin->setValue(lens->center().y());
            x2Spin->setValue(lens->radius1());
            y2Spin->setValue(lens->radius2());
            if (diameterSpin) {
                diameterSpin->setValue(lens->diameter());
                diameterSpin->setEnabled(true);
            }
        }
    }
}

void MainWindow::on_actionAddConic_triggered()
{
    // Пример: параболическое зеркало (k = -1) с радиусом 100, диаметром 50
    ConicSurface *surf = new ConicSurface(QPointF(0, 0), 100.0, -1.0, 50.0,
                                          1.0, 1.5, false, "Коническая поверхность");
    m_scene->addElement(surf);
    ConicGraphicsItem *item = new ConicGraphicsItem(surf, this);
    m_graphicsScene->addItem(item);
    updateElementsList();
    regenerateRays();
}

void MainWindow::on_actionSpotDiagram_triggered()
{
    if (!m_spotDialog) {
        m_spotDialog = new QDialog(this);
        m_spotDialog->setWindowTitle("Диаграмма рассеяния и гистограмма");
        QHBoxLayout *layout = new QHBoxLayout(m_spotDialog);
        QSplitter *splitter = new QSplitter(Qt::Horizontal);
        m_spotWidget = new SpotDiagramWidget;   // сохраняем как член класса
        m_histogramWidget = new HistogramWidget;
        splitter->addWidget(m_spotWidget);
        splitter->addWidget(m_histogramWidget);
        layout->addWidget(splitter);
        m_spotDialog->resize(900, 600);
    }
    updateSpotDiagram();
    m_spotDialog->show();
    m_spotDialog->raise();
}


void MainWindow::updateSpotDiagram()
{
    if (!m_spotDialog) return;
    QList<QPointF> allHits;
    for (OpticalElement* elem : m_scene->elements()) {
        if (Detector* det = dynamic_cast<Detector*>(elem))
            allHits.append(det->hits());
    }
    if (m_spotWidget) m_spotWidget->setPoints(allHits);
    if (m_histogramWidget) m_histogramWidget->setPoints(allHits);
}

void MainWindow::registerGraphicsItem(OpticalElement* elem, QGraphicsItem* item)
{
    m_elementToItem[elem] = item;
}

void MainWindow::setMovableForSelectedOnly()
{
    for (auto it = m_elementToItem.begin(); it != m_elementToItem.end(); ++it) {
        QGraphicsItem* item = it.value();
        bool movable = (it.key() == m_currentElement);
        item->setFlag(QGraphicsItem::ItemIsMovable, movable);
    }
}

void MainWindow::on_actionAddLens_triggered()
{
    // Создаём собирающую линзу (двояковыпуклую) с параметрами по умолчанию
    Lens *lens = new Lens(QPointF(0, 0), 10.0, 100.0, -100.0, 0.0, 0.0, 60.0, 1.5, "Линза");
    m_scene->addElement(lens);
    addElementToScene(lens);
    regenerateRays();
    updateElementsList();
    //refreshLinearMode(); // если используется линейный режим
}
