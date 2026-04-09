#pragma once

#include <vector>

struct Vertex
{
    float position[3];
    float normal[3];
    float uv[2];
};

class Mesh
{
public:
    Mesh();
    ~Mesh();

    void SetData(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    void Draw() const;

private:
    unsigned int m_vao;
    unsigned int m_vbo;
    unsigned int m_ebo;
    unsigned int m_indexCount;
};
