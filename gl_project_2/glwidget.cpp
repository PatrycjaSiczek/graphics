#include "glwidget.h"
#include <QMouseEvent>

#include "glm/glm.hpp"
#include "glm/common.hpp"
#include <glm/ext/matrix_transform.hpp>
#include "primitives.h"


GLWidget::GLWidget()
{
    frame = 0;
    pos_x = pos_y = 0;
    zoom = 1.0f;
}

GLWidget::~GLWidget()
{
    //TODO: usuwanie shaderow, geometrii, buforow
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    m_logger = new QOpenGLDebugLogger(this);
    if (m_logger->initialize()) {
        connect(m_logger, &QOpenGLDebugLogger::messageLogged, this, [](const QOpenGLDebugMessage &msg) {
            if (msg.id() == 131185 || msg.id() == 131218)
                return;
            qDebug() << msg;
        });
        m_logger->startLogging();
    }

    createShaders();
    createGeometry();

    connect(&frame_timer, &QTimer::timeout, this, QOverload<>::of(&GLWidget::update));
    frame_timer.setInterval(10);
    frame_timer.start();
}

void GLWidget::createShaders()
{
    shaders["basic"] = new GLSLProgram;
    bool stat = shaders["basic"]->compileShaderFromFile("C:/Users/patry/OneDrive/Pulpit/Semestr6/Grafika/gl_project_2/shaders/vs.glsl", GL_VERTEX_SHADER);
    stat &= shaders["basic"]->compileShaderFromFile("C:/Users/patry/OneDrive/Pulpit/Semestr6/Grafika/gl_project_2/shaders/fs.glsl", GL_FRAGMENT_SHADER);
    stat &= shaders["basic"]->link();
    if (!stat) qDebug("Some problem with shader!");
}

void GLWidget::createGeometry()
{
    geometry["main_axes"] = newAxesGeometry();
    geometryMat["main_axes"] = glm::mat4(1);

    geometry["plane"] = newPlaneGeometry({0.1,0.1}, {0,0.5,0});
    geometryMat["plane"] = glm::scale(glm::mat4(1.0f), glm::vec3(0.5, 0.5, 0.5));

    geometry["box"] = newBoxGeometry({0.5,0.5,0.5}, {0,1,0});
    geometryMat["box"] = glm::scale(glm::mat4(1.0f), glm::vec3(0.5, 0.5, 0.5));

    geometry["circ"] = newCircleGeometry(0.75, 24, {1,0,0});
    geometryMat["circ"] = glm::mat4(1.0f);

    geometry["sun"] = newBoxGeometry({0.1, 0.1, 0.1}, {1, 1, 0});
    geometryMat["sun"] = glm::mat4(1.0f);
}

void GLWidget::mouseMoveEvent(QMouseEvent *e)
{
    pos_x = e->pos().x();
    pos_y = e->pos().y();
}

void GLWidget::wheelEvent(QWheelEvent *e)
{
    zoom += e->angleDelta().y()/10000.0f;
    zoom = glm::clamp(zoom, 0.1f, 10.0f);
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0,0,w,h);
}

void GLWidget::paintGL()
{
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glm::mat4 identity = glm::mat4(1.0f);


    glm::mat4 view = glm::rotate(identity, 0.1f*pos_x, glm::vec3(0, 1, 0));
    view = glm::rotate(view, 0.01f * pos_y, glm::vec3(1,0,0));
    view = glm::scale(view, glm::vec3(zoom, zoom, zoom));

    glm::mat4 tx = glm::translate(identity, glm::vec3(0.5, 0, 0));
    glm::mat4 rot = glm::rotate(identity, glm::radians(0.5f* frame), glm::vec3(0,0,1));
    glm::mat4 rot2 = glm::rotate(identity, -glm::radians(4.0f* frame), glm::vec3(0,0,1));

    glm::mat4 mat = rot * tx * rot2;

    if(shaders.contains("basic")) {
        shaders["basic"]->use();
        shaders["basic"]->setUniform("ViewMat", view);

        shaders["basic"]->setUniform("ModelMat", identity);
        geometry["main_axes"]->render();

        //slonce
        shaders["basic"]->setUniform("ModelMat", identity);
        shaders["basic"]->setUniform("Color", glm::vec3(1, 1, 0));
        geometry["sun"]->render();

        //planeta
        shaders["basic"]->setUniform("ModelMat", mat);
        shaders["basic"]->setUniform("Color", glm::vec3(0,1,0));
        geometry["plane"]->render();
        geometry["main_axes"]->render();

        // ksiezyc
        glm::mat4 sc = glm::scale(identity, glm::vec3(0.05, 0.05, 0.05));
        glm::mat4 tk = glm::translate(identity, glm::vec3(0.2, 0, 0));
        glm::mat4 rot_k = glm::rotate(identity, -glm::radians(1.5f * frame), glm::vec3(0,0,1));

        mat = mat * rot_k * tk * sc;

        shaders["basic"]->setUniform("ModelMat", mat);
        shaders["basic"]->setUniform("Color", glm::vec3(1,1,1));
        geometry["box"]->render();
        geometry["main_axes"]->render();
    }
    else {
        qDebug("WARNING: No shader program");
    }
    frame++;
}
