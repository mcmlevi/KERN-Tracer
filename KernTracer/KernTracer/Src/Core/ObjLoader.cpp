#include "pch.h"
#include "Core/ObjLoader.h"
#include <glm/gtx/hash.hpp>
#include <tiny_obj_loader.h>

std::string getBaseDir(const std::string& str)
{
    size_t found = str.find_last_of("/\\");
    return str.substr(0, found);
}
std::unique_ptr<RT::Model> RT::ObjLoader::LoadModel(const std::string& filePath)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn;
    std::string err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str(), getBaseDir(filePath).c_str());

    if (!warn.empty())
    {
        printf("%s", warn.substr(0, warn.size() - 1).c_str());
    }

    if (!err.empty())
    {
        printf("%s", err.substr(0, err.size() - 1).c_str());
    }

    if (!ret)
    {
        printf("Something went wrong while trying to load the obj");
    }

    std::unique_ptr<RT::Model> model = std::make_unique<RT::Model>(); \

        for (const auto& shape : shapes)
        {
            std::vector<glm::vec3> vertices;
            std::vector<uint16_t> indicies;
            int materialCounter{};
            int materialIndex{ 0 };
            for (const auto& index : shape.mesh.indices)
            {

                glm::vec3 vert =
                {
                    glm::vec3(attrib.vertices[3 * index.vertex_index + 0],        //Position
                            attrib.vertices[3 * index.vertex_index + 1],
                            attrib.vertices[3 * index.vertex_index + 2]),
                };
                vertices.push_back(vert);
            }
            for (int i = 0; i < vertices.size(); i += 3)
            {
                model->triangles.push_back(RT::Triangle{ {vertices[i],vertices[i + 1],vertices[i + 2] } });
            }
        }

    return model;
}