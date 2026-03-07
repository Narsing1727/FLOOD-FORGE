cmake_minimum_required(VERSION 3.16)

project(Flood3D VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network  OpenGL
    OpenGLWidgets Concurrent)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network  OpenGL
    OpenGLWidgets Concurrent)



set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Flood3D
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        dialog.h dialog.cpp dialog.ui
        mapcanvas2d.h mapcanvas2d.cpp
        rainfall_dialog.h rainfall_dialog.cpp rainfall_dialog.ui
        floodengineclient.h floodengineclient.cpp
        mapcanvas3d.h mapcanvas3d.cpp
        fragment.vert
        vertex.vert
        shaders.qrc
        resources.qrc
        intro_window.h intro_window.cpp intro_window.ui

    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Flood3D APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Flood3D SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Flood3D
            ${PROJECT_SOURCES}
        )
    endif()
endif()
# ---- GDAL manual config (OSGeo4W) ----
set(GDAL_INCLUDE_DIR "D:/OSGeoW4/include")
set(GDAL_LIBRARY "D:/OSGeoW4/lib/gdal_i.lib")
target_link_libraries(Flood3D
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::OpenGL
    Qt${QT_VERSION_MAJOR}::OpenGLWidgets
    ${GDAL_LIBRARY}
    Qt6::Concurrent
)


target_include_directories(Flood3D PRIVATE
    ${GDAL_INCLUDE_DIR}
)
# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Flood3D)
endif()
set_target_properties(Flood3D PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Flood3D
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Flood3D)
endif()
