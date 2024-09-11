#pragma once

#include "Vulkan/Vulkan.hpp"

#include <bvh/triangle.hpp>
#include <bvh/sweep_sah_builder.hpp>

namespace Vulkan
{
	class Buffer;
	class Device;
	class DeviceMemory;
}

namespace Vulkan::RayTracing
{
	class DeviceProcedures;

	typedef bvh::Bvh<float> bvh_t;
	typedef bvh::Triangle<float> trig_t;
	typedef bvh::Vector3<float> vector_t;
	typedef bvh::BoundingBox<float> bbox_t;
	typedef bvh::SweepSahBuilder<bvh_t> builder_t;
	typedef bvh_t::Node node_t;

	class AccelerationStructure
	{
	public:
		AccelerationStructure(const AccelerationStructure &) = delete;
		AccelerationStructure &operator=(const AccelerationStructure &) = delete;
		AccelerationStructure &operator=(AccelerationStructure &&) = delete;

		AccelerationStructure(AccelerationStructure &&other) noexcept;
		virtual ~AccelerationStructure();

		const class Device &Device() const { return device_; }
		const class DeviceProcedures &DeviceProcedures() const { return deviceProcedures_; }
		const VkAccelerationStructureBuildSizesInfoKHR BuildSizes() const { return buildSizesInfo_; }

		static void MemoryBarrier(VkCommandBuffer commandBuffer);

	protected:
		explicit AccelerationStructure(const class DeviceProcedures &deviceProcedures, const class RayTracingProperties &rayTracingProperties);

		VkAccelerationStructureBuildSizesInfoKHR GetBuildSizes(const uint32_t *pMaxPrimitiveCounts) const;
		VkAccelerationStructureBuildSizesInfoKHR BVH_GetBuildSizes(bvh_t *bvh) const;

		void CreateAccelerationStructure(Buffer &resultBuffer, VkDeviceSize resultOffset);

		const class DeviceProcedures &deviceProcedures_;
		const VkBuildAccelerationStructureFlagsKHR flags_;

		VkAccelerationStructureBuildGeometryInfoKHR buildGeometryInfo_{};
		VkAccelerationStructureBuildSizesInfoKHR buildSizesInfo_{};

	private:
		const class Device &device_;
		const class RayTracingProperties &rayTracingProperties_;

		VULKAN_HANDLE(VkAccelerationStructureKHR, accelerationStructure_)
	};

}
