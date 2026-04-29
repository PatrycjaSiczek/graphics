#include <iostream>
#include <vector>

#include <QApplication>
#if QT_VERSION_MAJOR >= 6
    #include <QtOpenGLWidgets/QOpenGLWidget>
#else
    #include <QOpenGLWidget>
#endif
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLDebugMessage>
#include <QDebug>

#include <vector>

//vertex shader
const char* vs_code = "#version 330\n"
                      "layout (location=13) in vec3 pos;\n"
                      "layout (location=14) in vec4 v_color;"
                      "out vec4 f_color;"
                      "void main(){"
                      "    gl_Position = vec4(pos, 1.0);"
                      "    f_color = v_color;"
                      "}";

//fragment shader
const char* fs_code = "#version 330\n"
                      "uniform vec4 v_color;"
                      "in vec4 f_color;"
                      "out vec4 frag_color;\n"
                      "void main(){"
                      "     frag_color = f_color;"
                      "}";

/**
 * @brief handleLoggedMessage - obsługa bledow wspomagana przez Qt
 */
void handleLoggedMessage(const QOpenGLDebugMessage &debugMessage)
{
    if (debugMessage.id() == 131185) return; //for nvidia buffer create message
    qDebug() << "GL message: " << debugMessage;
}


/// \brief The Window class - klasa, ktora stanowi okno naszej aplikacji
/// i zawiera w sobie kontekst opengl-a.
class Window : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    //!Pomocnicza funkcja wyswietlajaca potencjalne bledy kompilacji shadera
    bool checkShaderStatus(GLuint shader)
    {
        GLint stat;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &stat);
        if(stat == GL_FALSE)
        {
            GLchar infoLog[10240];
            glGetShaderInfoLog(shader, 10240, NULL, infoLog);
            qDebug() << "Shader ERROR: " << infoLog;
            return false;
        }
        return true;
    }

    //!Pomocnicza funkcja wyswietlajaca potencjalne bledy linkowania programu shadera
    bool checkProgramStatus(GLuint program)
    {
        GLint stat;
        glGetProgramiv(program, GL_LINK_STATUS, &stat);
        if(stat == GL_FALSE)
        {
            GLchar infoLog[10240];
            glGetProgramInfoLog(program, 10240, NULL, infoLog);
            qDebug() << "Program ERROR: " << infoLog;
            return false;
        }
        return true;
    }

    std::vector<float> triangle_vertices;
    GLuint triangle_vbo;
    GLuint vao;

    std::vector<float> triangle_colors;
    GLuint triangle_colors_vbo;


    GLuint shader_program;

protected:
    /**
     * @brief initializeGL - reimplementacja metody; wywolywana po konstrukcji obiektu klasy
     * ale przed pierwszym wywolaniem resizeGL() i paintGL().
     * W tej funkcji mamy gwarancje, ze kontekst opengl jest juz utworzony.
     * To jest odpowiednie miejsce do setup-u opengla i sceny.
     */

    void initializeGL() override
    {
        //!Wywolanie metody z klasy QOpenGLFunctions; niezbedne do utawienia odpowiednich wskaznikow do funkcji opengl
        initializeOpenGLFunctions();

        //!Wypisanie wersji opengl
        qDebug() << (const char*)glGetString(GL_VERSION);
        // triangle_vertices = {0,0,0,
        //                      1,0,0,
        //                      0,1,0};

        // triangle_colors = {1,0,0,1,
        //                    0,1,0,1,
        //                    1,1,0,1};

        triangle_vertices = {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            -0.5f,  0.5f, 0.0f,

            0.5f, -0.5f, 0.0f,
            0.5f,  0.5f, 0.0f,
            -0.5f,  0.5f, 0.0f
        };
        triangle_colors = {

            1.0f, 0.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f, 1.0f,


            0.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f, 1.0f
        };
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        {
            glGenBuffers(1, &triangle_colors_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, triangle_colors_vbo);
            {
                glBufferData(GL_ARRAY_BUFFER,
                             triangle_colors.size()*sizeof(float),
                             triangle_colors.data(),
                             GL_STATIC_DRAW
                             );
                glEnableVertexAttribArray(14);
                glVertexAttribPointer(14,
                                      4,
                                      GL_FLOAT,
                                      GL_FALSE,
                                      0, 0);
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);


            glGenBuffers(1, &triangle_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, triangle_vbo);
            {
                glBufferData(GL_ARRAY_BUFFER,
                             triangle_vertices.size()*sizeof(float),
                             triangle_vertices.data(),
                             GL_STATIC_DRAW
                             );
                glEnableVertexAttribArray(13);
                glVertexAttribPointer(13,
                                      3,
                                      GL_FLOAT,
                                      GL_FALSE,
                                      0, 0);
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        glBindVertexArray(0);

        shader_program = glCreateProgram();
        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(vs, 1, &vs_code, 0);
        glShaderSource(fs, 1, &fs_code, 0);
        glCompileShader(vs);
        checkShaderStatus(vs);
        glCompileShader(fs);
        checkShaderStatus(fs);
        glAttachShader(shader_program, vs);
        glAttachShader(shader_program, fs);
        glLinkProgram(shader_program);
        checkProgramStatus(shader_program);
    }

    /**
     * @brief paintGL - reimplementacja metody; wywolywana zawsze gdy jest potrzeba
     * odrysowania widgetu. Nie wywolujemy jej nigdy jawnie.
     * Jezeli jest potrzeba odrysowania sceny, wywolujemy te funkcje niejawnie,
     * poprzez metode update()
     */
    void paintGL() override
    {
        glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_program);

        GLuint loc = glGetUniformLocation(shader_program, "color");
        glUniform4f(loc, 0, 1, 0, 1);


        glBindVertexArray(vao);
        {
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        glBindVertexArray(0);
        glUseProgram(0);

    }

    /**
     * @brief resizeGL - reimplementacja metody; wywolywana jest zawsze gdy
     * okienko pojawia się lub zmienia swoj rozmiar.
     * Dobre miejsce do wywolania niezbednych funkcji, do setupu sceny zaleznych
     * od wielkosci okna, np. glViewport() lub aktualizacji macierzy projekcji.
     * @param w - w tym parametrze bedzie nowa szerokosc okna
     * @param h - w tym parametrze bedzie nowa wysokosc okna
     */
    void resizeGL(int w, int h) override
    {
        glViewport(0,0,w,h);
    }
};


int main(int argc, char *argv[])
{
    QSurfaceFormat format;
    format.setVersion(3, 3);                          // OpenGL version
    format.setProfile(QSurfaceFormat::CoreProfile);   // lub QSurfaceFormat::CompatibilityProfile
    QSurfaceFormat::setDefaultFormat(format);

    QApplication a(argc, argv);

    //!Utworzenie i wyswietlenie okna aplikacji
    Window w;
    w.show();

    QOpenGLDebugLogger *logger = new QOpenGLDebugLogger();
    logger->initialize();
    QObject::connect(logger, &QOpenGLDebugLogger::messageLogged, handleLoggedMessage);
    logger->startLogging();

    return a.exec();
}
