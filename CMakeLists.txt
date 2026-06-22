cmake_minimum_required(VERSION 3.16)
project(2D_Optics VERSION 1.0 LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)



find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Список исходных файлов
set(SOURCES
    main.cpp
    mainwindow.cpp
    ray.cpp
    opticalelement.cpp
    straightinterface.cpp
    sphericalinterface.cpp
    detector.cpp
    scene.cpp
    raytracer.cpp
    movableitem.cpp
    conicsurface.cpp
    conicgraphicsitem.cpp
    spotdiagramwidget.cpp
    histogramwidget.cpp
    spotdiagramwidget.cpp
)

set(HEADERS
    mainwindow.h
    constants.h
    ray.h
    opticalelement.h
    straightinterface.h
    sphericalinterface.h
    detector.h
    scene.h
    raytracer.h
    movableitem.h
    conicsurface.h
    conicgraphicsitem.h
    spotdiagramwidget.h
    histogramwidget.h
    spotdiagramwidget.h
)

# Если у вас есть файл .ui (mainwindow.ui), добавьте:
# set(UIS mainwindow.ui)

add_executable(2D_Optics ${SOURCES} ${HEADERS}
    graphicsview.h
    graphicsview.cpp
    coordgrid.h
    coordgrid.cpp
    movableitem.h
    movableitem.cpp
    conicsurface.h
    conicsurface.cpp
    conicgraphicsitem.h
    conicgraphicsitem.cpp
    spotdiagramwidget.h
    spotdiagramwidget.cpp
    histogramwidget.h
    histogramwidget.cpp
    lens.h
    lens.cpp
    lensgraphicsitem.h
    lensgraphicsitem.cpp
    meniscus.h
    meniscus.cpp


) # ${UIS} если есть

target_link_libraries(2D_Optics Qt6::Widgets)

# Включить текущую директорию для поиска заголовков
target_include_directories(2D_Optics PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
