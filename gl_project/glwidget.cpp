#include "glwidget.h"
#include <QMouseEvent>

#include "glm/common.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "primitives.h"


GLWidget::GLWidget()
{
    frame = 0;
    pos_x = pos_y = 0;
    zoom = 5.0f;
}

GLWidget::~GLWidget()
{
    makeCurrent();
    for(auto p : shaders) delete p;
    for(auto g : geometry) delete g;

    shaders.clear();
    geometry.clear();
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
    bool stat = shaders["basic"]->compileShaderFromFile("shaders\\vs.glsl", GL_VERTEX_SHADER);
    stat &= shaders["basic"]->compileShaderFromFile("shaders\\fs.glsl", GL_FRAGMENT_SHADER);
    stat &= shaders["basic"]->link();
    if (!stat) qDebug("Some problem with shader!");
}

void GLWidget::createGeometry()
{

    geometry["main_axes"] = newAxesGeometry();
    geometryMat["main_axes"] = glm::mat4(1.0f);

    geometry["plane"] = newPlaneGeometry({1.0f, 1.0f}, {1.0f, 1.0f, 0.0f});
    geometryMat["plane"] = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));


    geometry["box"] = newBoxGeometry({1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f});
    geometryMat["box"] = glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 0.0f, 0.0f));

    geometry["circle"] = newCircleGeometry(0.5f, 30, {0.0f, 0.0f, 1.0f});
    geometryMat["circle"] = glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, 0.0f, 0.0f));
}

void GLWidget::mouseMoveEvent(QMouseEvent *e)
{
    if(e->buttons() & Qt::LeftButton) {
        pos_x = e->pos().x();
        pos_y = e->pos().y();
        update();
    }
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

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    if(shaders.contains("basic"))
    {
        shaders["basic"]->use();

        float aspect = (float)width() / (float)height();
        glm::mat4 PMat = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
        shaders["basic"]->setUniform("PMat", PMat);

        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -zoom));
        view = glm::rotate(view, glm::radians(15.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        shaders["basic"]->setUniform("MVMat", view);
        if(geometry.contains("main_axes")) {
            geometry["main_axes"]->render();
        }

        if(geometry.contains("plane")) {
            glm::mat4 model = geometryMat["plane"];
            shaders["basic"]->setUniform("MVMat", view * model);
            geometry["plane"]->render();
        }

        if(geometry.contains("box")) {
            glm::mat4 model = geometryMat["box"];
            shaders["basic"]->setUniform("MVMat", view * model);
            geometry["box"]->render();
        }

        if(geometry.contains("circle")) {
            glm::mat4 model = geometryMat["circle"];
            shaders["basic"]->setUniform("MVMat", view * model);
            geometry["circle"]->render();
        }
    }
    else
    {
        qDebug("WARNING: No shader program");
    }

    frame++;
}
