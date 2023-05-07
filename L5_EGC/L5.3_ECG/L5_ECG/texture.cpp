#include <iostream>
#include "texture.h"

Texture::Texture(GLenum TextureTarget, const std::string& FileName)
{
    m_textureTarget = TextureTarget;
    m_fileName = FileName;
    m_pImage = NULL;
}

bool Texture::Load()
{
    try {
        m_pImage = new Magick::Image(m_fileName);
        m_pImage->write(&m_blob, "RGBA");
    }
    catch (Magick::Error& Error) {
        std::cout << "Error loading texture '" << m_fileName << "': " << Error.what() << std::endl;
        return false;
    }

    glGenTextures(1, &m_textureObj); // Генерирует объекты текстур и помещает их в указатель на массив GLuint
    glBindTexture(m_textureTarget, m_textureObj); //Ссообщает OpenGL объект текстуры
    glTexImage2D(m_textureTarget, 0, GL_RGB, m_pImage->columns(), m_pImage->rows(), -0.5, GL_RGBA, GL_UNSIGNED_BYTE, m_blob.data()); // Загрузка главной части объекта текстуры
    glTexParameterf(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Управление аспектами операции выборки текстур
    glTexParameterf(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Управление аспектами операции выборки текстур

    return true;
}

void Texture::Bind(GLenum TextureUnit)
{
    // Активирование модуля текстур и привязка объекат текстур к модулю
    glActiveTexture(TextureUnit);
    glBindTexture(m_textureTarget, m_textureObj);
}
