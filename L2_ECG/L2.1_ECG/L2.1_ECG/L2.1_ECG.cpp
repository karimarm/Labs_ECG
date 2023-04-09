#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <cmath>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "math_3d.h"

GLuint VBO; // Указатель на буфер вершин
GLuint gWorldLocation; // Указатель для доступа к матрице

// Вершинный шейдер
static const char* pVS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
layout (location = 0) in vec3 Position;                                             \n\
                                                                                    \n\
//uniform mat4 gWorldScale, gWorldRotate, gWorldPosition, gWorld[2];                \n\
uniform mat4 gWorld[3];                                                             \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    gl_Position = gWorld[2] * (gWorld[1] * (gWorld[0] * vec4(Position, 1.0)));      \n\
}";

// рагментный шейдер
static const char* pFS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
out vec4 FragColor;                                                                 \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);                                           \n\
}";

static void RenderSceneCB()
{
    glClear(GL_COLOR_BUFFER_BIT); // Очистка буфера

    static float Scale = 0.0f;

    Scale += 0.0005f;

    // Матрица преобразования
    float matrix4[3][16];

    matrix4[0][0] = cosf(Scale);    matrix4[0][1] = -sinf(Scale);   matrix4[0][2] = 0.0f;    matrix4[0][3] = 0.0f;
    matrix4[0][4] = sinf(Scale);    matrix4[0][5] = cosf(Scale);    matrix4[0][6] = 0.0f;    matrix4[0][7] = 0.0f;
    matrix4[0][8] = 0.0f;           matrix4[0][9] = 0.0f;           matrix4[0][10] = 1.0f;   matrix4[0][11] = 0.0f;
    matrix4[0][12] = 0.0f;          matrix4[0][13] = 0.0f;          matrix4[0][14] = 0.0f;   matrix4[0][15] = 1.0f;

    matrix4[1][0] = sinf(Scale);    matrix4[1][1] = 0.0f;           matrix4[1][2] = 0.0f;           matrix4[1][3] = 0.0f;
    matrix4[1][4] = 0.0f;           matrix4[1][5] = sinf(Scale);    matrix4[1][6] = 0.0f;           matrix4[1][7] = 0.0f;
    matrix4[1][8] = 0.0f;           matrix4[1][9] = 0.0f;           matrix4[1][10] = sinf(Scale);   matrix4[1][11] = 0.0f;
    matrix4[1][12] = 0.0f;          matrix4[1][13] = 0.0f;          matrix4[1][14] = 0.0f;          matrix4[1][15] = 1.0f;

    matrix4[2][0] = 1.0f;    matrix4[2][1] = 0.0f;    matrix4[2][2] = 0.0f;     matrix4[2][3] = sinf(Scale);
    matrix4[2][4] = 0.0f;    matrix4[2][5] = 1.0f;    matrix4[2][6] = 0.0f;     matrix4[2][7] = 0.0f;
    matrix4[2][8] = 0.0f;    matrix4[2][9] = 0.0f;    matrix4[2][10] = 1.0f;    matrix4[2][11] = 0.0f;
    matrix4[2][12] = 0.0f;   matrix4[2][13] = 0.0f;   matrix4[2][14] = 0.0f;    matrix4[2][15] = 1.0f;

    glUniformMatrix4fv(gWorldLocation, 3, GL_TRUE, &matrix4[0][0]); // Загрузка массива в uniform-переменную шейдера

    glEnableVertexAttribArray(0); // Массив атрибутов вершин
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // Привязка буфера   
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // Восприятие данных внутри буфера

    glDrawArrays(GL_TRIANGLES, 0, 3); // Отрисовка

    glDisableVertexAttribArray(0); // Отключить атрибуты вершин

    glutSwapBuffers(); // Меняет буферы местами

    //glutPostRedisplay();
}


static void InitializeGlutCallbacks()
{
    glutDisplayFunc(RenderSceneCB); // Функция обратного вызова отрисовки одного кадра
    glutIdleFunc(RenderSceneCB); // Ленивая функция рендера
}

static void CreateVertexBuffer()
{
    // Массив вершин треугольника
    Vector3f Vertices[3];
    Vertices[0] = Vector3f(0.0f, 0.5f, 0.0f);
    Vertices[1] = Vector3f(-0.433f, -0.25f, 0.0f);
    Vertices[2] = Vector3f(0.433f, -0.25f, 0.0f);

    glGenBuffers(1, &VBO); // Функция для генерации объектов
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // Привязка буфера
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW); // Наполнение буфера данными
}

static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
    // Создание шейдера
    GLuint ShaderObj = glCreateShader(ShaderType);
    if (ShaderObj == 0) {
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        exit(0);
    }

    // Указание исходного кода шейдера
    const GLchar* p[1];
    p[0] = pShaderText;
    GLint Lengths[1];
    Lengths[0] = strlen(pShaderText);
    glShaderSource(ShaderObj, 1, p, Lengths);

    // Компилирование шейдера
    glCompileShader(ShaderObj);
    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
        exit(1);
    }

    glAttachShader(ShaderProgram, ShaderObj); // Присоединение скомпилированного объекта шейдера к объекту программы
}
static void CompileShaders()
{
    // Создание программного объекта в который будут слинкованы все шейдеры
    GLuint ShaderProgram = glCreateProgram();
    if (ShaderProgram == 0) {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

    // Создание шейдеров
    AddShader(ShaderProgram, pVS, GL_VERTEX_SHADER);
    AddShader(ShaderProgram, pFS, GL_FRAGMENT_SHADER);

    GLint Success = 0;
    GLchar ErrorLog[1024] = { 0 };

    // Линковка и проверка на ошибки
    glLinkProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
    if (Success == 0) {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    // Проверка программы
    glValidateProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    glUseProgram(ShaderProgram); // Назначение отлинкованной программы шейдеров конвейеру

    // Запрос и проверка uniform-переменной
    gWorldLocation = glGetUniformLocation(ShaderProgram, "gWorld");
    assert(gWorldLocation != 0xFFFFFFFF);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv); // Инициализация GLUT

    // Настройка опций GLUT
    // GLUT_DOUBLE - двойная буферизация
    // GLUT_RGBA - буфер цвета
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

    // Задание параметров окна и его создание с заголовком "Tutorial 03"
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Lab 2.1");

    // Отрисовка кадра
    InitializeGlutCallbacks();

    // Инициализация GLEW и проверка на ошибки
    GLenum res = glewInit();
    if (res != GLEW_OK) {
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        return 1;
    }

    // Цвет очистки буфера кадра
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Создание массива вершин треугольника
    CreateVertexBuffer();

    // Создание и компиляция шейдеров
    CompileShaders();

    // Передача контроля GLUT'у
    glutMainLoop();

    return 0;
}
