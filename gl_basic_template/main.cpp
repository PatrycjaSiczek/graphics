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



////////////////////////////////////////////////////////////////////////////////
//!Pomocnicze struktury do przechowywania pozycji i atrybutow wierzcholkow.
struct vec3 {float x,y,z;};
struct vec4 {float x,y,z,w;};


////////////////////////////////////////////////////////////////////////////////
/// \brief vertex_shader_src
/// Kody shaderow nalezy umieszczac w osobnych plikach i wczytywac je w miare potrzeby.
/// Tu sa hardcoded dla uproszczenia kodu
const char* vertex_shader_src = "#version 330 core\n"
                                "layout (location=11) in vec3 pos;\n"
                                "layout (location=12) in vec4 color;\n"
                                "out vec4 v_color;\n"
                                "void main() {"
                                "v_color = color;"
                                "gl_Position = vec4(pos, 1.0);"
                                "}";


const char* fragment_shader_src = "#version 330 core\n"
                                  "uniform vec3 object_color = vec3(1,0,1);\n"
                                  "in vec4 v_color;\n"
                                  "out vec4 f_color;\n"
                                  "void main() {f_color = v_color;}";


/**
 * @brief handleLoggedMessage - obsługa bledow wspomagana przez Qt
 */
void handleLoggedMessage(const QOpenGLDebugMessage &debugMessage)
{
    if (debugMessage.id() == 131185) return; //for nvidia buffer create message
    qDebug() << "GL message: " << debugMessage;
}


////////////////////////////////////////////////////////////////////////////////
/// \brief The Window class - klasa, ktora stanowi okno naszej aplikacji
/// i zawiera w sobie kontekst opengl-a.
/// Najlepiej przeniesc do osobnych plikow .h i .cpp
class Window : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    std::vector<vec3> pos; //!tablica zawierajaca wspolrzedne wierzcholkow
    GLuint pos_buffer; //!uchwyt do bufora wierzcholkow po stronie gpu, liczba reprezentujaca dany zasob opengl
    int pos_index; //!index, pod ktorym bedzie powiazany bufor wierzcholkow z atrybutem w shaderze

    std::vector<vec4> colors;
    GLuint color_buffer;
    GLuint color_index;

    GLuint vao; //!uchwyt do tablicy obiektow buforowych, w nim beda zagregowane wszystkie bufory

    GLuint shaderProgram; //!uchwyt do programu shadera

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



        ////////////////////////////////////////////////////////
        // BUFORY
        ////////////////////////////////////////////////////////

        //!tablica z pozycjami wierzcholkow w naszej geometrii;
        //!tu 3 wierzcholki, kazdy jest wektorem 3-elementowym
        pos = {{0.0, 0.0, 0.0},
               {1.0, 0.0, 0.0},
               {0.0, 1.0, 0.0}};

        //!Numer, ktory bedzie identyfikowal bufor z pozycjami wierzcholkow, na potrzeby shadera;
        //!dowolna ale unikalna liczba w skali programu shadera
        pos_index = 11;

        //!Generuje jeden uchwyt dla tablicy buforow VAO i zapisuje go w zmiennej vao;
        //! bedzie ona agregowala nasza geometrie i jej atrybuty
        glGenVertexArrays(1, &vao);

        //!Związanie podanej tablicy VAO;
        //!od tego momentu mozna dodawac bufory z wierzcholkami do tej tablicy vao
        glBindVertexArray(vao);

        //!Generuje jeden uchwyt dla bufora z wierzcholkami (VBO) dla pozycji i zapisuje go w zmiennej pos_buffer
        glGenBuffers(1, &pos_buffer);

        //!Związanie bufora pos_buffer do punktu wiazania ARRAY_BUFFER.
        //! W tym momencie jest tworzony pusty bufor po stronie gpu.
        //! Do danego punktu wiazania (tu ARRAY_BUFFER) moze byc zwiazany w danej chwili tylko jeden bufor
        //! stad dalsze funkcje "wiedza", o ktory bufor nam chodzi (czyli o pos_buffer)
        glBindBuffer(GL_ARRAY_BUFFER, pos_buffer);

        //!Aktywowanie i przypisanie aktualnie zwiazanemu buforowi VBO numeru pos_index
        glEnableVertexAttribArray(pos_index);

        //!Zdefiniowanie cech (polozenia i typu danych) naszego, aktualnie zwiazanego bufora vbo
        //!Zobacz definicje tej funkcji w specyfikacji opengl
        glVertexAttribPointer(
            pos_index,   // numer (polozenie) naszego bufora
            3,           // rozmiar pojedynczego elementu, tu 3 bo ten bufor ma wektory z trzema wspolrzednymi
            GL_FLOAT,    // typ danych w buforze (tu float)
            GL_FALSE,    // czy normalizowac (nie)
            0, 0         // stride (odstep pomiedzy kolejnymi wierzcholkami) i offset (od ktorego miejsca) w buforze
            );

        //!Przeslanie danych wierzcholkow do aktualnie zwiazanego bufora vbo.
        //!Pamiętaj, ze aktualnie z ARRAY_BUFFER jest zwiazany bufor pos_buffer, dlatego ta funkcja upload-uje dane do tego wlasnie bufora.
        //!Zobacz definicje tej funkcji w specyfikacji opengl
        glBufferData(
            GL_ARRAY_BUFFER,          // punkt wiazania
            pos.size()*sizeof(vec3),  // rozmiar w bajtach tablicy z danymi
            pos.data(),               // wskaznik na tablice z danymi
            GL_STATIC_DRAW            // sposob rysowania
            );

        //!Odwiązanie aktualnego bufora ARRAY_BUFFER
        glBindBuffer(GL_ARRAY_BUFFER, 0);


        //!Bufor dla kolorow
        colors = {{1,0,0,1}, {0,1,0,1}, {0,0,1,1}};
        color_index = 12;
        glGenBuffers(1, &color_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
        glBufferData(GL_ARRAY_BUFFER, colors.size()*sizeof(vec4), colors.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(color_index);
        glVertexAttribPointer(color_index, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        //!Odwiązanie aktualnej tablicy VAO
        glBindVertexArray(0);





        ////////////////////////////////////////////////////////
        // SHADERS
        ////////////////////////////////////////////////////////

        //!Utworzenie programu shadera
        shaderProgram = glCreateProgram();

        //!Utworzenie shaderow vertexow i fragmentow
        GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

        //!Przekazanie kodow zrodlowych do shaderow
        //!Zobacz definicje tej funkcji w specyfikacji opengl
        glShaderSource(vertex_shader, 1, (const GLchar**)&vertex_shader_src, NULL);
        glShaderSource(fragment_shader, 1, (const GLchar**)&fragment_shader_src, NULL);
        //!Kompilacja shaderow
        glCompileShader(vertex_shader);
        glCompileShader(fragment_shader);
        //!Sprawdzenie statusu kompilacji shaderow
        checkShaderStatus(vertex_shader);
        checkShaderStatus(fragment_shader);

        //!Dodanie skompilowanych shaderow do programu
        glAttachShader(shaderProgram, vertex_shader);
        glAttachShader(shaderProgram, fragment_shader);

        //!Polaczenie atrybutu o numerze pos_index z atrybutem "pos",
        //! ktory jest atrybutem wejsciowym (zmienna) w shaderze wierzcholkow podanego programu shadera;
        //! takie polaczenie musi byc wykonane przed zlinkowaniem programu
        //! Alternatywnie mozna podac to polaczenie w kodzie shadera przez 'layout (location=index) in ...
        glBindAttribLocation(shaderProgram, pos_index, "pos");

        //!Zlinkowanie programu shadera w kod wykonywalny
        glLinkProgram(shaderProgram);

        //!Sprawdzenie statusu linkowania shaderow
        checkProgramStatus(shaderProgram);
        ////////////////////////////////////////////////////////

    }

    /**
     * @brief paintGL - reimplementacja metody; wywolywana zawsze gdy jest potrzeba
     * odrysowania widgetu. Nie wywolujemy jej nigdy jawnie.
     * Jezeli jest potrzeba odrysowania sceny, wywolujemy te funkcje niejawnie,
     * poprzez metode update()
     */
    void paintGL() override
    {
        //!Kolor czyszczenia bufora kolorów, (r,g,b,a) in <0.0; 1.0>
        glClearColor(0.5, 0.5, 1.0, 1.0);

        //!Wyczyszczenie bufora kolorow i glebi
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //!Uruchomienie programu shadera; cala geometria, od tego momentu,
        //!bedzie rysowana za pomoca tego shadera;
        //!w danej chwili moze byc aktywny tylko jeden program shadera.
        glUseProgram(shaderProgram);

        //!Uzyskanie uchwytu dla zmiennej uniform o nazwie "object_color",
        //!ktora jest gdzies w aktywnym programie shadera
        GLuint object_color_loc = glGetUniformLocation(shaderProgram, "object_color");

        //!Ustawienie wartosci dla tej zmiennej uniform;
        //! musimy wiedziec jakiego typu jest to zmienna i
        //! wybrac odpowiednia wersje funkcji glUniformXXX();
        //! tutaj wiemy ze "object_color" jest typu vec3
        glUniform3f(object_color_loc, 0, 1, 0);


        //!Do narysowania geometrii potrzebujemy związac (aktywowac) wybrana tablice obiektow VAO,
        //! ktora zawiera w sobie te geometrie

        //!Zwiazanie tablicy obiektow VAO z geometria
        glBindVertexArray(vao);

        //!Narysowanie calej geometrii, ktora jest wrzucona do aktualnie
        //! zwiazanej tablicy obiektow VAO
        glDrawArrays(
            GL_TRIANGLES,  // rysuj elementy geometrii jako trojkaty
            0,             // zacznij od pierwszego wierzcholka z geometrii
            3              // narysuj trzy kolejne wierzcholki (tu w sumie jeden trojkat)
            );

        //!Odwiazanie VAO
        glBindVertexArray(0);

        //!Deaktywacja programu shadera
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
        glViewport(0,0, w, h);
    }
};





////////////////////////////////////////////////////////////////////////////////
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
