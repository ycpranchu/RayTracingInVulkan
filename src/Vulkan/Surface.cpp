#include "Surface.hpp"
#include "Instance.hpp"
#include "Window.hpp"

namespace Vulkan {

Surface::Surface(const class Instance& instance) :
	instance_(instance)
{
	#ifndef OFFSCREEN_RENDERING
	Check(glfwCreateWindowSurface(instance.Handle(), instance.Window().Handle(), nullptr, &surface_),
		"create window surface");
	#endif
}

Surface::~Surface()
{
	#ifndef OFFSCREEN_RENDERING
	if (surface_ != nullptr)
	{
		vkDestroySurfaceKHR(instance_.Handle(), surface_, nullptr);
		surface_ = nullptr;
	}
	#endif
}

}
