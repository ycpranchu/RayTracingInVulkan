#include "Model.hpp"
#include "CornellBox.hpp"
#include "Procedural.hpp"
#include "Sphere.hpp"
#include "Utilities/Exception.hpp"
#include "Utilities/Console.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/hash.hpp>

#include <tiny_obj_loader.h>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <unordered_map>
#include <vector>

void main()
{
    std::cout << "- loading '" << filename << "'... " << std::flush;

    const auto timer = std::chrono::high_resolution_clock::now();
    const std::string materialPath = std::filesystem::path(filename).parent_path().string();

    tinyobj::ObjReader objReader;

    if (!objReader.ParseFromFile(filename))
    {
        Throw(std::runtime_error("failed to load model '" + filename + "':\n" + objReader.Error()));
    }

    if (!objReader.Warning().empty())
    {
        Utilities::Console::Write(Utilities::Severity::Warning, [&objReader]()
                                  { std::cout << "\nWARNING: " << objReader.Warning() << std::flush; });
    }

    // Materials
    std::vector<Material> materials;

    for (const auto &material : objReader.GetMaterials())
    {
        Material m{};

        m.Diffuse = vec4(material.diffuse[0], material.diffuse[1], material.diffuse[2], 1.0);
        m.DiffuseTextureId = -1;

        materials.emplace_back(m);
    }

    if (materials.empty())
    {
        Material m{};

        m.Diffuse = vec4(0.7f, 0.7f, 0.7f, 1.0);
        m.DiffuseTextureId = -1;

        materials.emplace_back(m);
    }

    // Geometry
    const auto &objAttrib = objReader.GetAttrib();

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::unordered_map<Vertex, uint32_t> uniqueVertices(objAttrib.vertices.size());

    for (const auto &shape : objReader.GetShapes())
    {
        size_t faceId = 0;
        const auto &mesh = shape.mesh;
        for (const auto &index : mesh.indices)
        {
            Vertex vertex = {};

            vertex.Position =
                {
                    objAttrib.vertices[3 * index.vertex_index + 0],
                    objAttrib.vertices[3 * index.vertex_index + 1],
                    objAttrib.vertices[3 * index.vertex_index + 2],
                };

            if (!objAttrib.normals.empty())
            {
                vertex.Normal =
                    {
                        objAttrib.normals[3 * index.normal_index + 0],
                        objAttrib.normals[3 * index.normal_index + 1],
                        objAttrib.normals[3 * index.normal_index + 2]};
            }

            if (!objAttrib.texcoords.empty())
            {
                vertex.TexCoord =
                    {
                        objAttrib.texcoords[2 * index.texcoord_index + 0],
                        1 - objAttrib.texcoords[2 * index.texcoord_index + 1]};
            }

            size_t material_id_index = faceId / 3;
            int32_t material_index = mesh.material_ids[material_id_index];
            faceId++;
            vertex.MaterialIndex = std::max(0, material_index);

            if (uniqueVertices.count(vertex) == 0)
            {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }

            indices.push_back(uniqueVertices[vertex]);
        }
    }

    // If the model did not specify normals, then create smooth normals that conserve the same number of vertices.
    // Using flat normals would mean creating more vertices than we currently have, so for simplicity and better visuals we don't do it.
    // See https://stackoverflow.com/questions/12139840/obj-file-averaging-normals.
    if (objAttrib.normals.empty())
    {
        std::vector<vec3> normals(vertices.size());

        for (size_t i = 0; i < indices.size(); i += 3)
        {
            const auto normal = normalize(cross(
                vec3(vertices[indices[i + 1]].Position) - vec3(vertices[indices[i]].Position),
                vec3(vertices[indices[i + 2]].Position) - vec3(vertices[indices[i]].Position)));

            vertices[indices[i + 0]].Normal += normal;
            vertices[indices[i + 1]].Normal += normal;
            vertices[indices[i + 2]].Normal += normal;
        }

        for (auto &vertex : vertices)
        {
            vertex.Normal = normalize(vertex.Normal);
        }
    }

    const auto elapsed = std::chrono::duration<float, std::chrono::seconds::period>(std::chrono::high_resolution_clock::now() - timer).count();

    std::cout << "(" << objAttrib.vertices.size() << " vertices, " << uniqueVertices.size() << " unique vertices, " << materials.size() << " materials) ";
    std::cout << elapsed << "s" << std::endl;

    return Model(std::move(vertices), std::move(indices), std::move(materials), nullptr);
}