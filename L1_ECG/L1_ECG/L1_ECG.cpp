#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "math_3d.h"

// Глобальная переменная для хранения указателя на буфер вершин
GLuint VBO;

static void RenderSceneCB()
{
    // Очистка буфера
    glClear(GL_COLOR_BUFFER_BIT);

    glEnableVertexAttribArray(0); // Массив атрибутов вершин
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // Привязка буфера   
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // Восприятие данных внутри буфера

    glDrawArrays(GL_TRIANGLES, 0, 3); // Отрисовка

    glDisableVertexAttribArray(0); // Отключить атрибуты вершин

    // Меняет буферы местами
    glutSwapBuffers();
}


static void InitializeGlutCallbacks()
{
    // Функция обратного вызова отрисовки одного кадра
    glutDisplayFunc(RenderSceneCB);
}

static void CreateVertexBuffer()
{
    // Массив вершин треугольника
    Vector3f Vertices[3];
    Vertices[0] = Vector3f(-1.0f, -1.0f, 0.0f);
    Vertices[1] = Vector3f(1.0f, -1.0f, 0.0f);
    Vertices[2] = Vector3f(0.0f, 1.0f, 0.0f);

    glGenBuffers(1, &VBO);// Функция для генерации объектов
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // Привязка буфера
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW); // Наполнение буфера данными
}


int main(int argc, char** argv)
{
    // Инициализация GLUT
    glutInit(&argc, argv);

    // Настройка опций GLUT
    // GLUT_DOUBLE - двойная буферизация
    // GLUT_RGBA - буфер цвета
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

    // Задание параметров окна и его создание с заголовком "Lab 1"
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Lab 1");

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

    CreateVertexBuffer();

    // Передача контроля GLUT'у
    glutMainLoop();

    return 0;
}
