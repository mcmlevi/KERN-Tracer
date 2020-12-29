#include "pch.h"
#pragma warning(push,0)
#pragma warning(disable: 26451 4201)
#include "Core/ObjLoader.h"
#include <glm/gtx/hash.hpp>
#include <tiny_obj_loader.h>
#include <Core/ResourceManager.h>
#include <Core/Material.h>
std::string getBaseDir(const std::string& str)
{
    size_t found = str.find_last_of("/\\");
    return str.substr(0, found);
}
std::shared_ptr<RT::ModelData> RT::ObjLoader::LoadModel(const std::string& filePath, ResourceManager& resourceManager)
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

    std::shared_ptr<RT::ModelData> model = std::make_shared<RT::ModelData>(); 
	if(materials.empty())
	{
        model->loadedInMaterial = resourceManager.GetMaterial("Default");
	}
    else
    {
       auto material =  resourceManager.GetMaterial(filePath);
       material->baseColor = { materials[0].diffuse[0],materials[0].diffuse[1] ,materials[0].diffuse[2] };
       // Check for texture
    	if(!materials[0].diffuse_texname.empty())
		{
          material->texture = resourceManager.GetTexture(getBaseDir(filePath) + '/' + materials[0].diffuse_texname);
		}
        model->loadedInMaterial = material;
    }

	
        for (const auto& shape : shapes)
        {
            std::vector<glm::vec3> vertices;
            std::vector<glm::vec2> texturecoords;
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
            	if(index.texcoord_index < 0 )
            	{
                    glm::vec2 texture
                    {
                                 {},{}
                    };
                    texturecoords.push_back(texture);
            	}
                else
                {
                    glm::vec2 texture
                    {
                                  glm::vec2(attrib.texcoords[2 * index.texcoord_index + 0],        //Texture
                                attrib.texcoords[2 * index.texcoord_index + 1])
                    };
                    texturecoords.push_back(texture);
                }
               
                vertices.push_back(vert);
               
            }
            for (int i = 0; i < vertices.size(); i += 3)
            {
                model->triangles.push_back(RT::Triangle{ {vertices[i],vertices[i + 1],vertices[i + 2] },{texturecoords[i],texturecoords[i + 1],texturecoords[i + 2]} });
                model->Bounds[0] = glm::min(model->Bounds[0], vertices[i]);
                model->Bounds[0] = glm::min(model->Bounds[0], vertices[i+1]);
                model->Bounds[0] = glm::min(model->Bounds[0], vertices[i+2]);
                model->Bounds[1] = glm::max(model->Bounds[1], vertices[i]);
                model->Bounds[1] = glm::max(model->Bounds[1], vertices[i + 1]);
                model->Bounds[1] = glm::max(model->Bounds[1], vertices[i + 2]);
            }
        }

    return model;
}
#pragma warning(pop)