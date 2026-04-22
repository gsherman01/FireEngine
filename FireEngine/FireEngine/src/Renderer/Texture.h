#pragma once

#include <string>
#include <vector>

class Texture
{
public:
    Texture();
    ~Texture();

    bool LoadFromFile(const std::string& path, bool flipVertical = true);
    bool LoadCubemap(const std::vector<std::string>& faces);

    void Bind(unsigned int unit) const;
    void BindCubemap(unsigned int unit) const;

    unsigned int GetId() const { return m_id; }

private:
    unsigned int m_id;
    int m_width;
    int m_height;
    int m_channels;
    bool m_isCubemap;
};
