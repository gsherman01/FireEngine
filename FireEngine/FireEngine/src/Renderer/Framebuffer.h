#pragma once

class Framebuffer
{
public:
    Framebuffer();
    ~Framebuffer();

    bool Create(int width, int height);
    void Resize(int width, int height);

    void Bind() const;
    void Unbind() const;

    unsigned int GetHandle() const { return m_fbo; }
    unsigned int GetColorTexture() const { return m_colorTexture; }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

private:
    void Destroy();

    unsigned int m_fbo;
    unsigned int m_colorTexture;
    unsigned int m_depthRbo;
    int m_width;
    int m_height;
};
