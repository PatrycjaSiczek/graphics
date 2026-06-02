#include "glwidget.h"
#include <QMouseEvent>
#include <QKeyEvent>

#include "glm/glm.hpp"
#include "glm/common.hpp"
#include <glm/ext/matrix_transform.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "primitives.h"



GLWidget::GLWidget()
{
    frame = 0;
    pos_x = pos_y = 0;
    zoom = 1.0f;
    camera.pos = glm::vec3(0.0f, 1.5f, 5.0f);
    texture = nullptr;
    uv_timer = 0.0f;

    current_shader = "basic";
}

GLWidget::~GLWidget()
{
    makeCurrent();
    if(texture) {
        texture->destroy();
        delete texture;
    }

    for (GLSLProgram* shader : shaders.values()) {
        delete shader;
    }
    shaders.clear();

    for (Geometry* geom : geometry.values()) {
        delete geom;
    }
    geometry.clear();

    doneCurrent();
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
    createTexture();

    connect(&frame_timer, &QTimer::timeout, this, QOverload<>::of(&GLWidget::update));
    frame_timer.setInterval(10);
    frame_timer.start();

}


void GLWidget::createShaders()
{
    bool stat;

    shaders["basic"] = new GLSLProgram;
    stat = shaders["basic"]->compileShaderFromFile("shaders/vs.glsl", GL_VERTEX_SHADER);
    stat &= shaders["basic"]->compileShaderFromFile("shaders/fs.glsl", GL_FRAGMENT_SHADER);
    stat &= shaders["basic"]->link();
    if (!stat) qDebug("Problem z shaderem: basic (vs/fs)");

    shaders["ads"] = new GLSLProgram;
    stat = shaders["ads"]->compileShaderFromFile("shaders/ads_vs.glsl", GL_VERTEX_SHADER);
    stat &= shaders["ads"]->compileShaderFromFile("shaders/ads_fs.glsl", GL_FRAGMENT_SHADER);
    stat &= shaders["ads"]->link();
    if (!stat) qDebug("Problem z shaderem: ads");

    shaders["text_ads"] = new GLSLProgram;
    stat = shaders["text_ads"]->compileShaderFromFile("shaders/text_ads_vs.glsl", GL_VERTEX_SHADER);
    stat &= shaders["text_ads"]->compileShaderFromFile("shaders/text_ads_fs.glsl", GL_FRAGMENT_SHADER);
    stat &= shaders["text_ads"]->link();
    if (!stat) qDebug("Problem z shaderem: text_ads");
}

void GLWidget::createTexture()
{
    textures["wood"] = new Texture2D();
    if(!textures["wood"]->loadFromFile("shaders/drewno.jpg")) {
        qDebug() << "Błąd wczytywania: shaders/drewno.jpg";
    }

    textures["metal"] = new Texture2D();
    if(!textures["metal"]->loadFromFile("shaders/metal2.jpg")) {
        qDebug() << "Błąd wczytywania: shaders/metal2.jpg";
    }

    textures["chmura"] = new Texture2D();
    if(!textures["chmura"]->loadFromFile("shaders/clouds2.jpg")) {
        qDebug() << "Błąd wczytywania: shaders/clouds2.jpg";
    }

    textures["tree"] = new Texture2D();
    if(!textures["tree"]->loadFromFile("shaders/tree_02.png")) {
        qDebug() << "Błąd wczytywania: shaders/tree_02.png";
    }

    textures["window"] = new Texture2D();
    if(!textures["window"]->loadFromFile("shaders/blending_transparent_window.png")) {
        qDebug() << "Błąd wczytywania: shaders/blending_transparent_window.png";
    }
}

void GLWidget::createGeometry()
{
    geometry["main_axes"] = newAxesGeometry();
    geometry["cylinder"] = newCylinderGeometry(0.5f, 32, 2.0f, {1.0f, 1.0f, 1.0f});
    geometry["sphere"] = newSphereGeometry(0.8f, 32, 32, {1.0f, 1.0f, 1.0f});
}

void GLWidget::mousePressEvent(QMouseEvent *e)
{
    lastPos = e->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *e)
{
    int dx = e->pos().x() - lastPos.x();
    int dy = e->pos().y() - lastPos.y();

    lastPos = e->pos();

    pos_x += dx;
    pos_y += dy;
}

void GLWidget::wheelEvent(QWheelEvent *e)
{
    zoom += e->angleDelta().y()/10000.0f;
    zoom = glm::clamp(zoom, 0.1f, 10.0f);
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0,0,w,h);
    float aspect = w/(float)h;
    ProjMat = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 100.0f);
}

void GLWidget::keyPressEvent(QKeyEvent *e)
{
    keys.insert(e->key());
}

void GLWidget::keyReleaseEvent(QKeyEvent *e)
{
    keys.remove(e->key());
}

void GLWidget::processCamera()
{
    if(keys.contains(Qt::Key_W)){
        camera.pos += 0.1f*camera.forward;
    }
    if(keys.contains(Qt::Key_S)){
        camera.pos -= 0.1f*camera.forward;
    }
    if(keys.contains(Qt::Key_A)){
        camera.pos -= 0.1f*camera.s();
    }
    if(keys.contains(Qt::Key_D)){
        camera.pos += 0.1f*camera.s();
    }
    if(keys.contains(Qt::Key_E)){
        camera.pos += 0.1f*camera.up;
    }
    if(keys.contains(Qt::Key_Q)){
        camera.pos -= 0.1f*camera.up;
    }

    float sensitivity = 0.002f;
    float pitch = pos_y * sensitivity;
    pitch = glm::clamp(pitch, -1.5f, 1.5f);

    glm::mat4 rot = glm::rotate(glm::mat4(1.0f), pos_x * sensitivity, glm::vec3(0,1,0));
    rot = glm::rotate(rot, pitch, glm::vec3(1,0,0));

    camera.up = rot * glm::vec4(0,1,0,1);
    camera.forward = rot * glm::vec4(0,0,-1,1);
}

void GLWidget::paintGL()
{
    glm::vec3 fogColor = glm::vec3(0.9f, 0.92f, 0.95f);
    glClearColor(fogColor.r, fogColor.g, fogColor.b, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDepthMask(GL_FALSE);

    processCamera();
    glm::mat4 viewMat = camera.matrix();

    if(shaders.contains("text_ads")) {
        GLSLProgram* program = shaders["text_ads"];
        program->use();
        program->setUniform("ProjectionMat", ProjMat);

        glm::vec3 lightPosWorld = glm::vec3(0.0f, 5.0f, 5.0f);
        glm::vec3 lightPosEye = glm::vec3(viewMat * glm::vec4(lightPosWorld, 1.0f));
        program->setUniform("LightPosition", lightPosEye);
        program->setUniform("LightColor", glm::vec3(1.0f, 1.0f, 1.0f));


        program->setUniform("FogColor", fogColor);
        program->setUniform("FogDensity", 0.15f);

        program->setUniform("EnableUVAnimation", false);
        program->setUniform("FlipTree", false);

        Frame sphere_frame;
        sphere_frame.pos = glm::vec3(-1.5f, 0.0f, -4.0f);
        glm::mat4 sphereMV = viewMat * sphere_frame.matrix();

        program->setUniform("ModelViewMatrix", sphereMV);
        program->setUniform("NormalMatrix", glm::mat3(glm::transpose(glm::inverse(sphereMV))));

        program->setUniform("MaterialAmbient", glm::vec3(0.4f));
        program->setUniform("MaterialDiffuse", glm::vec3(1.0f));
        program->setUniform("MaterialSpecular", glm::vec3(0.0f));
        program->setUniform("MaterialShiness", 1.0f);

        if(textures.contains("window")) {
            textures["window"]->bind(0);
            program->setUniform("TextureSampler", 0);
            program->setUniform("UseTexture", true);
        } else {
            program->setUniform("UseTexture", false);
        }
        geometry["sphere"]->render();


        program->setUniform("FlipTree", true);

        Frame cyl_frame;
        cyl_frame.pos = glm::vec3(1.5f, 0.0f, -4.0f);
        glm::mat4 cylModel = cyl_frame.matrix();
        cylModel = glm::rotate(cylModel, frame * 0.02f, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 cylMV = viewMat * cylModel;

        program->setUniform("ModelViewMatrix", cylMV);
        program->setUniform("NormalMatrix", glm::mat3(glm::transpose(glm::inverse(cylMV))));

        program->setUniform("MaterialAmbient", glm::vec3(0.4f));
        program->setUniform("MaterialDiffuse", glm::vec3(1.0f));
        program->setUniform("MaterialSpecular", glm::vec3(0.0f));
        program->setUniform("MaterialShiness", 1.0f);

        if(textures.contains("tree")) {
            textures["tree"]->bind(0);
            program->setUniform("TextureSampler", 0);
            program->setUniform("UseTexture", true);
        } else {
            program->setUniform("UseTexture", false);
        }

        geometry["cylinder"]->render();

        program->setUniform("FlipTree", false);
    }
    else {
        qDebug("WARNING: No shader program");
    }
    glDepthMask(GL_TRUE);

    frame++;
}
