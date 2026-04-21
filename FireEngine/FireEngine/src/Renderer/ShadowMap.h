#pragma once

class ShadowMap
{
public:
    ShadowMap();
    ~ShadowMap();

    bool Create(int resolution);
    void BindForWriting() const;
    void BindForReading(unsigned int textureUnit) const;

    unsigned int GetDepthTexture() const { return m_depthTexture; }
    int GetResolution() const { return m_resolution; }

private:
    void Destroy();

    unsigned int m_fbo;
    unsigned int m_depthTexture;
    int m_resolution;
};
