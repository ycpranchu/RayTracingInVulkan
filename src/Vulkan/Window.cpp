#include "Window.hpp"
#include "Utilities/Exception.hpp"
#include "Utilities/StbImage.hpp"
#include <iostream>

namespace Vulkan
{

	namespace
	{
		void GlfwErrorCallback(const int error, const char *const description)
		{
			std::cerr << "ERROR: GLFW: " << description << " (code: " << error << ")" << std::endl;
		}

		void GlfwKeyCallback(GLFWwindow *window, const int key, const int scancode, const int action, const int mods)
		{
			auto *const this_ = static_cast<Window *>(glfwGetWindowUserPointer(window));
			if (this_->OnKey)
			{
				this_->OnKey(key, scancode, action, mods);
			}
		}

		void GlfwCursorPositionCallback(GLFWwindow *window, const double xpos, const double ypos)
		{
			auto *const this_ = static_cast<Window *>(glfwGetWindowUserPointer(window));
			if (this_->OnCursorPosition)
			{
				this_->OnCursorPosition(xpos, ypos);
			}
		}

		void GlfwMouseButtonCallback(GLFWwindow *window, const int button, const int action, const int mods)
		{
			auto *const this_ = static_cast<Window *>(glfwGetWindowUserPointer(window));
			if (this_->OnMouseButton)
			{
				this_->OnMouseButton(button, action, mods);
			}
		}

		void GlfwScrollCallback(GLFWwindow *window, const double xoffset, const double yoffset)
		{
			auto *const this_ = static_cast<Window *>(glfwGetWindowUserPointer(window));
			if (this_->OnScroll)
			{
				this_->OnScroll(xoffset, yoffset);
			}
		}
	}

	Window::Window(const WindowConfig &config) : config_(config)
	{
#ifdef OFFSCREEN_RENDERING
		window_ = nullptr;
#else
		glfwSetErrorCallback(GlfwErrorCallback);

		if (!glfwInit())
		{
			Throw(std::runtime_error("glfwInit() failed"));
		}

		if (!glfwVulkanSupported())
		{
			Throw(std::runtime_error("glfwVulkanSupported() failed"));
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, config.Resizable ? GLFW_TRUE : GLFW_FALSE);

		auto *const monitor = config.Fullscreen ? glfwGetPrimaryMonitor() : nullptr;

		window_ = glfwCreateWindow(config.Width, config.Height, config.Title.c_str(), monitor, nullptr);
		if (window_ == nullptr)
		{
			Throw(std::runtime_error("failed to create window"));
		}

		GLFWimage icon;
		icon.pixels = stbi_load("../assets/textures/Vulkan.png", &icon.width, &icon.height, nullptr, 4);
		if (icon.pixels == nullptr)
		{
			Throw(std::runtime_error("failed to load icon"));
		}

		glfwSetWindowIcon(window_, 1, &icon);
		stbi_image_free(icon.pixels);

		if (config.CursorDisabled)
		{
			glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}

		glfwSetWindowUserPointer(window_, this);
		glfwSetKeyCallback(window_, GlfwKeyCallback);
		glfwSetCursorPosCallback(window_, GlfwCursorPositionCallback);
		glfwSetMouseButtonCallback(window_, GlfwMouseButtonCallback);
		glfwSetScrollCallback(window_, GlfwScrollCallback);
#endif
	}

	Window::~Window()
	{
#ifndef OFFSCREEN_RENDERING
		if (window_ != nullptr)
		{
			glfwDestroyWindow(window_);
			window_ = nullptr;
		}

		glfwTerminate();
		glfwSetErrorCallback(nullptr);
#endif
	}

	float Window::ContentScale() const
	{
		float xscale = 1.0f;
		float yscale = 1.0f;

#ifndef OFFSCREEN_RENDERING
		glfwGetWindowContentScale(window_, &xscale, &yscale);
#endif

		return xscale;
	}

	VkExtent2D Window::FramebufferSize() const
	{
		int width = config_.Width, height = config_.Height;

#ifndef OFFSCREEN_RENDERING
		glfwGetFramebufferSize(window_, &width, &height);
#endif

		return VkExtent2D{static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
	}

	VkExtent2D Window::WindowSize() const
	{
		int width = config_.Width, height = config_.Height;

#ifndef OFFSCREEN_RENDERING
		glfwGetWindowSize(window_, &width, &height);
#endif

		return VkExtent2D{static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
	}

	const char *Window::GetKeyName(const int key, const int scancode) const
	{
#ifdef OFFSCREEN_RENDERING
		return NULL;
#else
		return glfwGetKeyName(key, scancode);
#endif
	}

	std::vector<const char *> Window::GetRequiredInstanceExtensions() const
	{
#ifdef OFFSCREEN_RENDERING
		return std::vector<const char *>();
#else
		uint32_t glfwExtensionCount = 0;
		const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		return std::vector<const char *>(glfwExtensions, glfwExtensions + glfwExtensionCount);
#endif
	}

	double Window::GetTime() const
	{
		return glfwGetTime();
	}

	void Window::Close()
	{
#ifndef OFFSCREEN_RENDERING
		glfwSetWindowShouldClose(window_, 1);
#endif
	}

	bool Window::IsMinimized() const
	{
#ifdef OFFSCREEN_RENDERING
		return false;
#else
		const auto size = FramebufferSize();
		return size.height == 0 && size.width == 0;
#endif
	}

	void Window::Run()
	{
		glfwSetTime(0.0);

#ifdef OFFSCREEN_RENDERING
		while (true)
#else
		while (!glfwWindowShouldClose(window_))
#endif
		{
			glfwPollEvents();

			if (DrawFrame)
			{
				DrawFrame();
				// fflush(stdout);
				// abort();
			}
		}
	}

	void Window::WaitForEvents() const
	{
#ifndef OFFSCREEN_RENDERING
		glfwWaitEvents();
#endif
	}

}
