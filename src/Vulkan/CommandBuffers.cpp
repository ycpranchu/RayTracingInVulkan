#include "CommandBuffers.hpp"
#include "CommandPool.hpp"
#include "Device.hpp"
#include <stdio.h>

namespace Vulkan {

CommandBuffers::CommandBuffers(CommandPool& commandPool, const uint32_t size) :
	commandPool_(commandPool)
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool.Handle();
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = size;

	commandBuffers_.resize(size);

	printf("RTV(app): Allocating command buffers (vkAllocateCommandBuffers)...\n");
	Check(vkAllocateCommandBuffers(commandPool.Device().Handle(), &allocInfo, commandBuffers_.data()),
		"allocate command buffers");
}

CommandBuffers::~CommandBuffers()
{
	if (!commandBuffers_.empty())
	{
		vkFreeCommandBuffers(commandPool_.Device().Handle(), commandPool_.Handle(), static_cast<uint32_t>(commandBuffers_.size()), commandBuffers_.data());
		commandBuffers_.clear();
	}
}

VkCommandBuffer CommandBuffers::Begin(const size_t i)
{
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	beginInfo.pInheritanceInfo = nullptr; // Optional

	printf("RTV(app): Begin recording command buffer (vkBeginCommandBuffer)...\n");
	Check(vkBeginCommandBuffer(commandBuffers_[i], &beginInfo),
		"begin recording command buffer");

	return commandBuffers_[i];
}

void CommandBuffers::End(const size_t i)
{
	printf("RTV(app): End recording command buffer (vkEndCommandBuffer)...\n");
	Check(vkEndCommandBuffer(commandBuffers_[i]),
		"record command buffer");
}

}
