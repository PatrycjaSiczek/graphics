QT = gui
QT += widgets
QT += opengl
equals(QT_MAJOR_VERSION, 6) {
    QT += openglwidgets
}
CONFIG += c++11
SOURCES += main.cpp
