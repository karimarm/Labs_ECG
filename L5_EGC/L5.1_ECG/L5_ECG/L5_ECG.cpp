#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "engine_common.h"
#include "util.h"
#include "pipeline.h"
#include "camera.h"
#include "texture.h"
#include "lighting_technique.h"
#include "glut_backend.h"
#include "mesh.h"
#include "billboard_list.h"

#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 1024


class Tutorial27 : public ICallbacks
{
public:

    Tutorial27()
    {
        m_pLightingTechnique = NULL;
        m_pGameCamera = NULL;
        m_pGround = NULL;
        m_pTexture = NULL;
        m_pNormalMap = NULL;

        m_dirLight.AmbientIntensity = 0.2f;
        m_dirLight.DiffuseIntensity = 0.8f;
        m_dirLight.Color = Vector3f(1.0f, 1.0f, 1.0f);
        m_dirLight.Direction = Vector3f(1.0f, 0.0f, 0.0f);

        m_persProjInfo.FOV = 60.0f;
        m_persProjInfo.Height = WINDOW_HEIGHT;
        m_persProjInfo.Width = WINDOW_WIDTH;
        m_persProjInfo.zNear = 1.0f;
        m_persProjInfo.zFar = 100.0f;
    }


    ~Tutorial27()
    {
        SAFE_DELETE(m_pLightingTechnique);
        SAFE_DELETE(m_pGameCamera);
        SAFE_DELETE(m_pGround);
        SAFE_DELETE(m_pTexture);
        SAFE_DELETE(m_pNormalMap);
    }


    bool Init()
    {
        Vector3f Pos(0.0f, 1.0f, -1.0f);
        Vector3f Target(0.0f, -0.5f, 1.0f);
        Vector3f Up(0.0, 1.0f, 0.0f);

        m_pGameCamera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT, Pos, Target, Up);

        m_pLightingTechnique = new LightingTechnique();

        if (!m_pLightingTechnique->Init()) {
            printf("Error initializing the lighting technique\n");
            return false;
        }

        m_pLightingTechnique->Enable();
        m_pLightingTechnique->SetDirectionalLight(m_dirLight);
        m_pLightingTechnique->SetColorTextureUnit(0);
        m_pLightingTechnique->SetNormalMapTextureUnit(2);

        m_pGround = new Mesh();

        if (!m_pGround->LoadMesh("quad.obj")) {
            return false;
        }

        if (!m_billboardList.Init("monster_hellknight.png")) {
            return false;
        }

        m_pTexture = new Texture(GL_TEXTURE_2D, "bricks.jpg");

        if (!m_pTexture->Load()) {
            return false;
        }

        m_pTexture->Bind(COLOR_TEXTURE_UNIT);

        m_pNormalMap = new Texture(GL_TEXTURE_2D, "normal_map.jpg");

        if (!m_pNormalMap->Load()) {
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

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_pLightingTechnique->Enable();

        m_pTexture->Bind(COLOR_TEXTURE_UNIT);
        m_pNormalMap->Bind(NORMAL_TEXTURE_UNIT);

        Pipeline p;
        p.Scale(20.0f, 20.0f, 1.0f);
        p.Rotate(90.0f, 0.0, 0.0f);
        p.SetCamera(m_pGameCamera->GetPos(), m_pGameCamera->GetTarget(), m_pGameCamera->GetUp());
        p.SetPerspectiveProj(m_persProjInfo);

        m_pLightingTechnique->SetWVP(p.GetWVPTrans());
        m_pLightingTechnique->SetWorldMatrix(p.GetWorldTrans());
        m_pGround->Render();

        m_billboardList.Render(p.GetVPTrans(), m_pGameCamera->GetPos());
        glutSwapBuffers();
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
        }
    }


    virtual void PassiveMouseCB(int x, int y)
    {
        m_pGameCamera->OnMouse(x, y);
    }

private:

    LightingTechnique* m_pLightingTechnique;
    Camera* m_pGameCamera;
    DirectionalLight m_dirLight;
    Mesh* m_pGround;
    Texture* m_pTexture;
    Texture* m_pNormalMap;
    PersProjInfo m_persProjInfo;
    BillboardList m_billboardList;
};


int main(int argc, char** argv)
{
    Magick::InitializeMagick(*argv);
    GLUTBackendInit(argc, argv);

    if (!GLUTBackendCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, 32, false, "Tutorial 27")) {
        return 1;
    }

    Tutorial27* pApp = new Tutorial27();

    if (!pApp->Init()) {
        return 1;
    }

    pApp->Run();

    delete pApp;

    return 0;
}