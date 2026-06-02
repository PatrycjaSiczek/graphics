#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "glslprogram.h"
#include "geometry.h"
#include "texture2d.h"

#if QT_VERSION_MAJOR >= 6
#include <QtOpenGLWidgets/QOpenGLWidget>
#else
#include <QOpenGLWidget>
#endif

#include <QOpenGLFunctions_4_2_Core>
#include <QOpenGLDebugLogger>
#include <QMap>
#include <QDebug>
#include <QTimer>
#include "glm/glm.hpp"

#include <QOpenGLTexture>

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_2_Core
{
    QMap<QString, GLSLProgram*> shaders;
    QMap<QString, Geometry*> geometry;
    QMap<QString, glm::mat4> geometryMat;
    QMap<QString, Texture2D*> textures;

    glm::mat4 MVMat;
    glm::mat4 ProjMat;

    Camera camera;

    QOpenGLDebugLogger *m_logger = nullptr;
    QTimer frame_timer;
    int frame;

    void createShaders();
    void createGeometry();
    void createTexture();
    int pos_x, pos_y;
    float zoom;
    QString current_shader;
    QSet<int> keys;
    void processCamera();
    QPoint lastPos;

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *event);
    QOpenGLTexture* texture;
    float uv_timer = 0.0f;
public:
    GLWidget();
    ~GLWidget();
};


#endif // GLWIDGET_H
