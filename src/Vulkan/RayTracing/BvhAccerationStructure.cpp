#include "BvhAccerationStructure.hpp"
#include "DeviceProcedures.hpp"
#include "Assets/Scene.hpp"
#include "Assets/Vertex.hpp"
#include "Utilities/Exception.hpp"
#include "Vulkan/Buffer.hpp"
#include "Vulkan/BufferUtil.hpp"

namespace Vulkan::RayTracing
{

	BvhAccerationStructure::BvhAccerationStructure(
		const class DeviceProcedures &deviceProcedures,
		const class RayTracingProperties &rayTracingProperties,
		const std::vector<trig_t> &trigs) : AccelerationStructure(deviceProcedures, rayTracingProperties)
	{
		// buildGeometryInfo_.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
		// buildGeometryInfo_.flags = flags_;
		// buildGeometryInfo_.geometryCount = static_cast<uint32_t>(geometries_.Geometry().size());
		// buildGeometryInfo_.pGeometries = geometries_.Geometry().data();
		// buildGeometryInfo_.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
		// buildGeometryInfo_.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
		// buildGeometryInfo_.srcAccelerationStructure = nullptr;

		// std::vector<uint32_t> maxPrimCount(geometries_.BuildOffsetInfo().size());

		// for (size_t i = 0; i != maxPrimCount.size(); ++i)
		// {
		// 	maxPrimCount[i] = geometries_.BuildOffsetInfo()[i].primitiveCount;
		// }

		bvh_ = build_bvh(trigs);
		buildSizesInfo_ = BVH_GetBuildSizes(&bvh_);
	}

	BvhAccerationStructure::BvhAccerationStructure(BvhAccerationStructure &&other) noexcept : AccelerationStructure(std::move(other))
	{
	}

	BvhAccerationStructure::~BvhAccerationStructure()
	{
		bvh_nodes_Buffer_.reset();
		bvh_nodes_BufferMemory_.reset();
		bvh_primitive_indices_Buffer_.reset();
		bvh_primitive_indices_BufferMemory_.reset();
		bvh_offset_Buffer_.reset();
		bvh_offset_BufferMemory_.reset();
	}

	bvh_t BvhAccerationStructure::build_bvh(const std::vector<trig_t> &trigs)
	{
		auto [bboxes, centers] = bvh::compute_bounding_boxes_and_centers(trigs.data(), trigs.size());
		auto global_bbox = bvh::compute_bounding_boxes_union(bboxes.get(), trigs.size());

		printf("global_bbox = (%f, %f, %f), (%f, %f, %f)\n",
			   global_bbox.min[0], global_bbox.min[1], global_bbox.min[2],
			   global_bbox.max[0], global_bbox.max[1], global_bbox.max[2]);

		// Building the BVH tree
		bvh_t bvh;
		builder_t builder(bvh);
		builder.max_leaf_size = max_trig_in_leaf_size;
		builder.build(global_bbox, bboxes.get(), centers.get(), trigs.size());

		return bvh;
	}

	void BvhAccerationStructure::Generate(Vulkan::CommandPool &commandPool,
										  std::vector<bvh_t::Node> &nodes_,
										  std::vector<size_t> &primitive_indices_,
										  std::vector<glm::uvec3> &offsets)
	{
		constexpr auto flags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;

		Vulkan::BufferUtil::CreateDeviceBuffer(commandPool, "bvh_nodes", flags, nodes_, bvh_nodes_Buffer_, bvh_nodes_BufferMemory_);
		Vulkan::BufferUtil::CreateDeviceBuffer(commandPool, "bvh_primitive_indices", flags, primitive_indices_, bvh_primitive_indices_Buffer_, bvh_primitive_indices_BufferMemory_);
		Vulkan::BufferUtil::CreateDeviceBuffer(commandPool, "bvh_offsets", flags, offsets, bvh_offset_Buffer_, bvh_offset_BufferMemory_);
	}
}
