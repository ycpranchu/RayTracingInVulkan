#include "Scene.hpp"
#include "Model.hpp"
#include "Sphere.hpp"
#include "Texture.hpp"
#include "TextureImage.hpp"
#include "Vulkan/BufferUtil.hpp"
#include "Vulkan/ImageView.hpp"
#include "Vulkan/Sampler.hpp"
#include "Utilities/Exception.hpp"
#include "Vulkan/SingleTimeCommands.hpp"

namespace Assets
{
	Scene::Scene(Vulkan::CommandPool &commandPool, std::vector<Model> &&models, std::vector<Texture> &&textures) : models_(std::move(models)),
																												   textures_(std::move(textures))
	{
		// Concatenate all the models
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		std::vector<trig_t> trigs;
		std::vector<Material> materials;

		// std::vector<glm::vec4> procedurals;
		// std::vector<glm::vec4> proceduralCubes;
		// std::vector<glm::vec4> proceduralCylinder;
		// std::vector<glm::vec4> proceduralMandelbulbs;
		// std::vector<VkAabbPositionsKHR> aabbs;
		// std::vector<VkAabbPositionsKHR> aabbcubes;
		// std::vector<VkAabbPositionsKHR> aabbCylinder;
		// std::vector<VkAabbPositionsKHR> aabbMandelbulbs;

		std::vector<glm::uvec3> offsets;

		for (const auto &model : models_)
		{
			// Remember the index, vertex offsets.
			const auto indexOffset = static_cast<uint32_t>(indices.size());
			const auto vertexOffset = static_cast<uint32_t>(vertices.size());
			const auto trigOffset = static_cast<uint32_t>(trigs.size());
			const auto materialOffset = static_cast<uint32_t>(materials.size());

			offsets.emplace_back(indexOffset, vertexOffset, trigOffset);

			// Copy model data one after the other.
			vertices.insert(vertices.end(), model.Vertices().begin(), model.Vertices().end());
			indices.insert(indices.end(), model.Indices().begin(), model.Indices().end());
			trigs.insert(trigs.end(), model.Trigs().begin(), model.Trigs().end());
			materials.insert(materials.end(), model.Materials().begin(), model.Materials().end());

			// Adjust the material id.
			for (size_t i = vertexOffset; i != vertices.size(); ++i)
			{
				vertices[i].MaterialIndex += materialOffset;
			}

			// // Add optional procedurals.
			// const auto *const sphere = dynamic_cast<const Sphere *>(model.Procedural());
			// if (sphere != nullptr)
			// {
			// 	const auto aabb = sphere->BoundingBox();
			// 	aabbs.push_back({aabb.first.x, aabb.first.y, aabb.first.z, aabb.second.x, aabb.second.y, aabb.second.z});
			// 	procedurals.emplace_back(sphere->Center, sphere->Radius);
			// }
			// else
			// {
			// 	aabbs.emplace_back();
			// 	procedurals.emplace_back();
			// }

			// // Add optional Cube procedurals.
			// const auto *const cube = dynamic_cast<const Cube *>(model.ProceduralCube());
			// if (cube != nullptr)
			// {
			// 	const auto aabb = cube->BoundingBox();
			// 	aabbcubes.push_back({aabb.first.x, aabb.first.y, aabb.first.z, aabb.second.x, aabb.second.y, aabb.second.z});
			// 	proceduralCubes.emplace_back(cube->Center, cube->Radius);
			// }
			// else
			// {
			// 	aabbcubes.emplace_back();
			// 	proceduralCubes.emplace_back();
			// }

			// // Add optional Cylinder procedurals.
			// const auto *const cylinder = dynamic_cast<const Cylinder *>(model.ProceduralCylinder());
			// if (cylinder != nullptr)
			// {
			// 	const auto aabb = cylinder->BoundingBox();
			// 	aabbCylinder.push_back({aabb.first.x, aabb.first.y, aabb.first.z, aabb.second.x, aabb.second.y, aabb.second.z});
			// 	proceduralCylinder.emplace_back(cylinder->Center, cylinder->Radius);
			// }
			// else
			// {
			// 	aabbCylinder.emplace_back();
			// 	proceduralCylinder.emplace_back();
			// }

			// // Add optional Mandelbulb procedurals.
			// const auto *const mandelbulb = dynamic_cast<const Mandelbulb *>(model.ProceduralMandelbulb());
			// if (mandelbulb != nullptr)
			// {
			// 	const auto aabb = mandelbulb->BoundingBox();
			// 	aabbMandelbulbs.push_back({aabb.first.x, aabb.first.y, aabb.first.z, aabb.second.x, aabb.second.y, aabb.second.z});
			// 	proceduralMandelbulbs.emplace_back(mandelbulb->Center, mandelbulb->Radius);
			// }
			// else
			// {
			// 	aabbMandelbulbs.emplace_back();
			// 	proceduralMandelbulbs.emplace_back();
			// }
		}

		constexpr auto flags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;

		Vulkan::BufferUtil::CreateDeviceBuffer(commandPool, "Vertices", VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | flags, vertices, vertexBuffer_, vertexBufferMemory_);
		Vulkan::BufferUtil::CreateDeviceBuffer(commandPool, "Indices", VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | flags, indices, indexBuffer_, indexBufferMemory_);

		printf("ycpin: CreateDeviceBuffer(Trigs) start\n");
		size_t size_of_element = sizeof(trig_t);
		size_t number_of_elements = trigs.size();
		size_t total_size = number_of_elements * size_of_element;
		printf("ycpin: CreateDeviceBuffer(Trigs) total size: %ld\n", total_size);

		Vulkan::BufferUtil::CreateDeviceBuffer(commandPool, "Trigs", flags, trigs, trigBuffer_, trigBufferMemory_);
		printf("ycpin: CreateDeviceBuffer(Trigs) finish\n");

		Vulkan::BufferUtil::CreateDeviceBuffer(commandPool, "Materials", flags, materials, materialBuffer_, materialBufferMemory_);
		Vulkan::BufferUtil::CreateDeviceBuffer(commandPool, "Offsets", flags, offsets, offsetBuffer_, offsetBufferMemory_);

		// Vulkan::BufferUtil::CreateDeviceBuffer(commandPool, "AABBs", VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | flags, aabbs, aabbBuffer_, aabbBufferMemory_);
		// Vulkan::BufferUtil::CreateDeviceBuffer(commandPool, "Procedurals", flags, procedurals, proceduralBuffer_, proceduralBufferMemory_);

		// Vulkan::BufferUtil::CreateDeviceBuffer(commandPool, "CubeAABBs", VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | flags, aabbcubes, aabbCubeBuffer_, aabbCubeBufferMemory_);
		// Vulkan::BufferUtil::CreateDeviceBuffer(commandPool, "ProceduralCubes", flags, proceduralCubes, proceduralCubeBuffer_, proceduralCubeBufferMemory_);

		// Vulkan::BufferUtil::CreateDeviceBuffer(commandPool, "CylinderAABBs", VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | flags, aabbCylinder, aabbCylinderBuffer_, aabbCylinderBufferMemory_);
		// Vulkan::BufferUtil::CreateDeviceBuffer(commandPool, "ProceduralCylinders", flags, proceduralCylinder, proceduralCylinderBuffer_, proceduralCylinderBufferMemory_);
		// Vulkan::BufferUtil::CreateDeviceBuffer(commandPool, "MandelbulbAABBs", VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | flags, aabbMandelbulbs, aabbMandelbulbBuffer_, aabbMandelbulbBufferMemory_);
		// Vulkan::BufferUtil::CreateDeviceBuffer(commandPool, "ProceduralMandelbulbs", flags, proceduralMandelbulbs, proceduralMandelbulbBuffer_, proceduralMandelbulbBufferMemory_);

		// Upload all textures
		textureImages_.reserve(textures_.size());
		textureImageViewHandles_.resize(textures_.size());
		textureSamplerHandles_.resize(textures_.size());

		for (size_t i = 0; i != textures_.size(); ++i)
		{
			textureImages_.emplace_back(new TextureImage(commandPool, textures_[i]));
			textureImageViewHandles_[i] = textureImages_[i]->ImageView().Handle();
			textureSamplerHandles_[i] = textureImages_[i]->Sampler().Handle();
		}
	}

	Scene::~Scene()
	{
		textureSamplerHandles_.clear();
		textureImageViewHandles_.clear();
		textureImages_.clear();
		proceduralBuffer_.reset();
		proceduralBufferMemory_.reset(); // release memory after bound buffer has been destroyed
		proceduralCubeBuffer_.reset();
		proceduralCubeBufferMemory_.reset(); // release memory after bound buffer has been destroyed
		proceduralCylinderBuffer_.reset();
		proceduralCylinderBufferMemory_.reset(); // release memory after bound buffer has been destroyed
		// proceduralMandelbulbBuffer_.reset();
		// proceduralMandelbulbBufferMemory_.reset(); // release memory after bound buffer has been destroyed
		aabbBuffer_.reset();
		aabbBufferMemory_.reset(); // release memory after bound buffer has been destroyed
		aabbCubeBuffer_.reset();
		aabbCubeBufferMemory_.reset(); // release memory after bound buffer has been destroyed
		aabbCylinderBuffer_.reset();
		aabbCylinderBufferMemory_.reset(); // release memory after bound buffer has been destroyed
		// aabbMandelbulbBuffer_.reset();
		// aabbMandelbulbBufferMemory_.reset(); // release memory after bound buffer has been destroyed
		offsetBuffer_.reset();
		offsetBufferMemory_.reset(); // release memory after bound buffer has been destroyed
		materialBuffer_.reset();
		materialBufferMemory_.reset(); // release memory after bound buffer has been destroyed
		indexBuffer_.reset();
		indexBufferMemory_.reset(); // release memory after bound buffer has been destroyed
		vertexBuffer_.reset();
		vertexBufferMemory_.reset(); // release memory after bound buffer has been destroyed

		trigBuffer_.reset();
		trigBufferMemory_.reset(); // release memory after bound buffer has been destroyed
	}

}
