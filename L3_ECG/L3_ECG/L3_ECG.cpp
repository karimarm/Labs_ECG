#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "pipeline.h"
#include "camera.h"
#include "texture.h"
#include "lighting_technique.h"
#include "glut_backend.h"
#include "util.h"

#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 1024

// Координаты вершин и текстуры
struct Vertex
{
    Vector3f m_pos;
    Vector2f m_tex;
    Vector3f m_normal;

    Vertex() {}

    Vertex(Vector3f pos, Vector2f tex)
    {
        m_pos = pos;
        m_tex = tex;
        m_normal = Vector3f(0.0f, 0.0f, 0.0f);
    }
};

class Main : public ICallbacks
{
public:
    Main()
    {
        m_pGameCamera = NULL;
        m_pTexture = NULL;
        m_pEffect = NULL;
        m_scale = 0.0f;

        // Параметры рассеяноого освещения
        m_directionalLight.Color = Vector3f(1.0f, 1.0f, 1.0f);
        m_directionalLight.AmbientIntensity = 0.1f;
        m_directionalLight.DiffuseIntensity = 0.1f;
        m_directionalLight.Direction = Vector3f(-1.0f, -1.0f, 0.0f);
    }

    ~Main()
    {
        delete m_pEffect;
        delete m_pGameCamera;
        delete m_pTexture;
    }

    bool Init()
    {
        // Позиция камеры

        Vector3f Pos(-10.0f, 10.0f, -10.0f);
        Vector3f Target(1.0f, 0.0f, 1.0f);
        Vector3f Up(0.0, 1.0f, 0.0f);
        m_pGameCamera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT, Pos, Target, Up);

        // Массив индексов вершин
        unsigned int Indices[] = { 0, 2, 1,
                                   0, 3, 2 };

        CreateIndexBuffer(Indices, sizeof(Indices));

        CreateVertexBuffer(Indices, ARRAY_SIZE_IN_ELEMENTS(Indices));

        m_pEffect = new LightingTechnique();

        if (!m_pEffect->Init())
        {
            printf("Error initializing the lighting technique\n");
            return false;
        }

        m_pEffect->Enable();

        m_pEffect->SetTextureUnit(0);

        m_pTexture = new Texture(GL_TEXTURE_2D, "123.jpg");

        if (!m_pTexture->Load()) {
            return false;
        }

        return true;
    }

    void Run()
    {
        GLUTBackendRun(this);
    }

    virtual void RenderSceneCB()
    {
        m_pGameCamera->OnRender();

        glClear(GL_COLOR_BUFFER_BIT); // Очистка буфера

        m_scale += 0.01f;

        // Прожекторы
        SpotLight sl[2];
        sl[0].DiffuseIntensity = 15.0f;
        sl[0].Color = Vector3f(1.0f, 1.0f, 0.7f);
        sl[0].Position = Vector3f(-0.0f, -1.9f, -0.0f);
        sl[0].Direction = Vector3f(sinf(m_scale), 0.0f, cosf(m_scale));
        sl[0].Attenuation.Linear = 0.1f;
        sl[0].Cutoff = 20.0f;

        sl[1].DiffuseIntensity = 5.0f;
        sl[1].Color = Vector3f(0.0f, 1.0f, 1.0f);
        sl[1].Position = m_pGameCamera->GetPos();
        sl[1].Direction = m_pGameCamera->GetTarget();
        sl[1].Attenuation.Linear = 0.1f;
        sl[1].Cutoff = 10.0f;

         m_pEffect->SetSpotLights(2, sl);


         // Точечное освещение
        PointLight pl[3];
        pl[0].DiffuseIntensity = 0.5f;
        pl[0].Color = Vector3f(1.0f, 0.0f, 0.0f);
        pl[0].Position = Vector3f(-9.0f, 1.0f, -9.0f);
        pl[0].Attenuation.Linear = 0.1f;

        pl[1].DiffuseIntensity = 0.5f;
        pl[1].Color = Vector3f(0.0f, 1.0f, 0.0f);
        pl[1].Position = Vector3f(9.0f, 1.0f, 9.0f);
        pl[1].Attenuation.Linear = 0.1f;

        pl[2].DiffuseIntensity = 0.5f;
        pl[2].Color = Vector3f(0.0f, 0.0f, 1.0f);
        pl[2].Position = Vector3f(-9.0f, 1.0f, 9.0f);
        pl[2].Attenuation.Linear = 0.1f;

         m_pEffect->SetPointLights(3, pl);

         // Создаёт объект конвейера и настраивает его
        Pipeline p;
        p.Rotate(0.0f, 0.0f, 0.0f);
        p.WorldPos(0.0f, 0.0f, 1.0f);
        p.SetCamera(m_pGameCamera->GetPos(), m_pGameCamera->GetTarget(), m_pGameCamera->GetUp());
        p.SetPerspectiveProj(60.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 0.1f, 100.0f);
        m_pEffect->SetWVP(p.GetWVPTrans());
        const Matrix4f& WorldTransformation = p.GetWorldTrans();
        m_pEffect->SetWorldMatrix(WorldTransformation);
        m_pEffect->SetDirectionalLight(m_directionalLight);
        m_pEffect->SetEyeWorldPos(m_pGameCamera->GetPos());
        m_pEffect->SetMatSpecularIntensity(1.0f);
        m_pEffect->SetMatSpecularPower(32);

        glEnableVertexAttribArray(0);// Атрибуты вершин для координат вершин
        glEnableVertexAttribArray(1); // Атрибуты вершин для координат текстур
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);// Привязка буфера
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);// Восприятие данных внутри буфера
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12); // Восприятие данных внутри буфера
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)20); // Восприятие данных внутри буфера
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
        m_pTexture->Bind(GL_TEXTURE0);  // Привязываем текстуры к модулю
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // Отрисовка

        glDisableVertexAttribArray(0); // Отключить атрибуты
        glDisableVertexAttribArray(1); // Отключить атрибуты
        glDisableVertexAttribArray(2); // Отключить атрибуты

        glutSwapBuffers(); // Меняет буферы местами
    }

    virtual void IdleCB()
    {
        RenderSceneCB();
    }

    virtual void SpecialKeyboardCB(int Key, int x, int y)
    {
        m_pGameCamera->OnKeyboard(Key);
    }


    virtual void KeyboardCB(unsigned char Key, int x, int y)
    {
        switch (Key) {
        case 'q':
            glutLeaveMainLoop();
            break;

        case 'a':
            m_directionalLight.AmbientIntensity += 0.05f;
            break;

        case 's':
            m_directionalLight.AmbientIntensity -= 0.05f;
            break;

        case 'z':
            m_directionalLight.DiffuseIntensity += 0.05f;
            break;

        case 'x':
            m_directionalLight.DiffuseIntensity -= 0.05f;
            break;
        }
    }


    virtual void PassiveMouseCB(int x, int y)
    {
        m_pGameCamera->OnMouse(x, y);
    }

private:

    void CalcNormals(const unsigned int* pIndices, unsigned int IndexCount,
        Vertex* pVertices, unsigned int VertexCount) {
        for (unsigned int i = 0; i < IndexCount; i += 3) {
            unsigned int Index0 = pIndices[i];
            unsigned int Index1 = pIndices[i + 1];
            unsigned int Index2 = pIndices[i + 2];
            Vector3f v1 = pVertices[Index1].m_pos - pVertices[Index0].m_pos;
            Vector3f v2 = pVertices[Index2].m_pos - pVertices[Index0].m_pos;
            Vector3f Normal = v1.Cross(v2);
            Normal.Normalize();

            pVertices[Index0].m_normal += Normal;
            pVertices[Index1].m_normal += Normal;
            pVertices[Index2].m_normal += Normal;
        }

        for (unsigned int i = 0; i < VertexCount; i++) {
            pVertices[i].m_normal.Normalize();
        }
    }


    void CreateVertexBuffer(const unsigned int* pIndices, unsigned int IndexCount)
    {
        Vertex Vertices[4] = { Vertex(Vector3f(-10.0f, -2.0f, -10.0f), Vector2f(0.0f, 0.0f)),
                               Vertex(Vector3f(10.0f, -2.0f, -10.0f), Vector2f(1.0f, 0.0f)),
                               Vertex(Vector3f(10.0f, -2.0f, 10.0f), Vector2f(1.0f, 1.0f)),
                               Vertex(Vector3f(-10.0f, -2.0f, 10.0f), Vector2f(0.0f, 1.0f)) };

        unsigned int VertexCount = ARRAY_SIZE_IN_ELEMENTS(Vertices);

        CalcNormals(pIndices, IndexCount, Vertices, VertexCount);

        glGenBuffers(1, &m_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
    }

    void CreateIndexBuffer(const unsigned int* pIndices, unsigned int SizeInBytes)
    {
        glGenBuffers(1, &m_IBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, SizeInBytes, pIndices, GL_STATIC_DRAW);
    }


    GLuint m_VBO;
    GLuint m_IBO;
    LightingTechnique* m_pEffect;
    Texture* m_pTexture;
    Camera* m_pGameCamera;
    float m_scale;
    DirectionalLight m_directionalLight;
};


int main(int argc, char** argv)
{
    Magick::InitializeMagick(*argv);
    GLUTBackendInit(argc, argv);

    if (!GLUTBackendCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, 32, false, "OpenGL tutors")) {
        return 1;
    }

    Main* pApp = new Main();

    if (!pApp->Init()) {
        return 1;
    }

    pApp->Run();

    delete pApp;

    return 0;
}
