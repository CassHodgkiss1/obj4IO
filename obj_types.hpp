#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <string>

namespace obj
{
    struct index_t 
    {
        uint32_t vertex_index;
        uint32_t normal_index;
        uint32_t texcoord_index;
    };

    struct mesh_t
    {
        std::vector<index_t> indeces;
    };

    struct attrib_t
    {
        std::vector<float> vertices;
        std::vector<float> normals;
        std::vector<float> texcoords;
    };

    struct shape_t {
        std::string name;
        mesh_t mesh;
    };
    
    struct vert4_t
    {
        glm::vec4 position;
        glm::vec4 normal;
        glm::vec3 uv;

        bool operator==(const vert4_t& other) const
        {
            return 
                position == other.position && 
                normal == other.normal && 
                uv == other.uv;
        }
    };

    struct face3_t
    {
        uint32_t a, b, c;

        bool operator==(const face3_t& other) const
        {
            return 
                a == other.a && 
                b == other.b && 
                c == other.c;
        }
    };
}