#pragma once

#include "AccelerationStructure.hpp"
#include "BvhAccerationStructure.hpp"
#include "Utilities/Glm.hpp"

namespace Assets
{
	class Procedural;
	class Scene;
}

namespace Vulkan
{
	class Buffer;
	class CommandPool;
	class DeviceMemory;
}

namespace Vulkan::RayTracing
{
	constexpr int field_b_bits = 3;
	constexpr int field_c_bits = 15 - field_b_bits;
	constexpr int max_node_in_cluster_size = (1 << field_c_bits);
	constexpr size_t max_trig_in_leaf_size = (1 << field_b_bits) - 1;
	constexpr int max_trig_in_cluster_size = max_node_in_cluster_size;
	constexpr int max_cluster_size = (1 << 15);

	class BvhAccerationStructure final : public AccelerationStructure
	{
	public:
		typedef bvh::Bvh<float> bvh_t;
		typedef bvh::Triangle<float> trig_t;
		typedef bvh::Vector3<float> vector_t;
		typedef bvh::BoundingBox<float> bbox_t;
		typedef bvh::SweepSahBuilder<bvh_t> builder_t;
		typedef bvh_t::Node node_t;

		BvhAccerationStructure(const BvhAccerationStructure &) = delete;
		BvhAccerationStructure &operator=(const BvhAccerationStructure &) = delete;
		BvhAccerationStructure &operator=(BvhAccerationStructure &&) = delete;

		BvhAccerationStructure(
			const class DeviceProcedures &deviceProcedures,
			const class RayTracingProperties &rayTracingProperties,
			const std::vector<trig_t> &trigs);
		BvhAccerationStructure(BvhAccerationStructure &&other) noexcept;
		~BvhAccerationStructure();

		bvh_t build_bvh(const std::vector<trig_t> &trigs);
		bvh_t bvh_;

		void Generate(Vulkan::CommandPool &commandPool,
					  std::vector<bvh_t::Node> &nodes_,
					  std::vector<size_t> &primitive_indices_,
					  std::vector<glm::uvec3> &offsets);

		const Vulkan::Buffer &BVH_NodeBuffer() const { return *bvh_nodes_Buffer_; }
		const Vulkan::Buffer &BVH_PrimitiveIndiceBuffer() const { return *bvh_primitive_indices_Buffer_; }
		const Vulkan::Buffer &BVH_OffsetsBuffer() const { return *bvh_offset_Buffer_; }

	private:
		std::unique_ptr<Buffer> bvh_nodes_Buffer_;
		std::unique_ptr<DeviceMemory> bvh_nodes_BufferMemory_;
		std::unique_ptr<Buffer> bvh_primitive_indices_Buffer_;
		std::unique_ptr<DeviceMemory> bvh_primitive_indices_BufferMemory_;
		std::unique_ptr<Buffer> bvh_offset_Buffer_;
		std::unique_ptr<DeviceMemory> bvh_offset_BufferMemory_;
	};
}
