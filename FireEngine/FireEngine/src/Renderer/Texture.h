#pragma once

#include <string>

class Texture
{
public:
    Texture();
    ~Texture();

    bool LoadFromFile(const std::string& path, bool flipVertical = true);
    void Bind(unsigned int unit) const;

private:
    unsigned int m_id;
    int m_width;
    int m_height;
    int m_channels;
};
