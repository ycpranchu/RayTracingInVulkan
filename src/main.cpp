
#include "Vulkan/Enumerate.hpp"
#include "Vulkan/Strings.hpp"
#include "Vulkan/SwapChain.hpp"
#include "Vulkan/Version.hpp"
#include "Utilities/Console.hpp"
#include "Utilities/Exception.hpp"
#include "Options.hpp"
#include "RayTracer.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>

namespace
{
	UserSettings CreateUserSettings(const Options &options);
	void PrintVulkanSdkInformation();
	void PrintVulkanInstanceInformation(const Vulkan::Application &application, bool benchmark);
	void PrintVulkanLayersInformation(const Vulkan::Application &application, bool benchmark);
	void PrintVulkanDevices(const Vulkan::Application &application);
	void PrintVulkanSwapChainInformation(const Vulkan::Application &application, bool benchmark);
	void SetVulkanDevice(Vulkan::Application &application);
}

int main(int argc, const char *argv[]) noexcept
{
	try
	{
		// 參數解析與設置
		const Options options(argc, argv);
		const UserSettings userSettings = CreateUserSettings(options);

		// Vulkan 視窗配置
		const Vulkan::WindowConfig windowConfig{
			"Vulkan Window",
			options.Width,
			options.Height,
			options.Benchmark && options.Fullscreen,
			options.Fullscreen,
			!options.Fullscreen};

		// 初始化
		RayTracer application(userSettings, windowConfig, static_cast<VkPresentModeKHR>(options.PresentMode));

		PrintVulkanSdkInformation();

		// Vulkan SDK Header Version: 283

		PrintVulkanInstanceInformation(application, options.Benchmark);

		// Vulkan Instance Extensions:
		// - VK_KHR_device_group_creation (0.0.1)
		// - VK_KHR_external_fence_capabilities (0.0.1)
		// - VK_KHR_external_memory_capabilities (0.0.1)
		// - VK_KHR_external_semaphore_capabilities (0.0.1)
		// - VK_KHR_get_physical_device_properties2 (0.0.2)
		// - VK_KHR_get_surface_capabilities2 (0.0.1)
		// - VK_KHR_surface (0.0.25)
		// - VK_KHR_surface_protected_capabilities (0.0.1)
		// - VK_KHR_xcb_surface (0.0.6)
		// - VK_KHR_xlib_surface (0.0.6)
		// - VK_EXT_debug_report (0.0.10)
		// - VK_EXT_debug_utils (0.0.2)
		// - VK_KHR_portability_enumeration (0.0.1)
		// - VK_LUNARG_direct_driver_loading (0.0.1)

		PrintVulkanLayersInformation(application, options.Benchmark);

		// Vulkan Instance Layers:
		// - VK_LAYER_MESA_device_select (1.2.73) : Linux device selection layer
		// - VK_LAYER_NV_optimus (1.3.278) : NVIDIA Optimus layer
		// - VK_LAYER_KHRONOS_synchronization2 (1.3.283) : Khronos Synchronization2 layer
		// - VK_LAYER_MESA_overlay (1.1.73) : Mesa Overlay layer
		// - VK_LAYER_KHRONOS_profiles (1.3.283) : Khronos Profiles layer
		// - VK_LAYER_KHRONOS_shader_object (1.3.283) : Khronos Shader object layer
		// - VK_LAYER_LUNARG_gfxreconstruct (1.3.283) : GFXReconstruct Capture Layer Version 1.0.4-unknown
		// - VK_LAYER_LUNARG_monitor (1.3.283) : Execution Monitoring Layer
		// - VK_LAYER_KHRONOS_validation (1.3.283) : Khronos Validation Layer
		// - VK_LAYER_LUNARG_screenshot (1.3.283) : LunarG image capture layer
		// - VK_LAYER_LUNARG_api_dump (1.3.283) : LunarG API dump layer

		PrintVulkanDevices(application);

		// Vulkan Devices:
		// - [0] UnknownVendor 'llvmpipe (LLVM 10.0.0, 256 bits)' (CPU: vulkan 1.3.254, driver llvmpipe Mesa 23.2.0-devel (git-b8f3e3980e) (LLVM 10.0.0) - 0.0.1)

		SetVulkanDevice(application);

		PrintVulkanSwapChainInformation(application, options.Benchmark);

		application.Run();

		return EXIT_SUCCESS;
	}

	catch (const Options::Help &)
	{
		return EXIT_SUCCESS;
	}

	catch (const std::exception &exception)
	{
		Utilities::Console::Write(Utilities::Severity::Fatal, [&exception]()
								  {
			const auto stacktrace = boost::get_error_info<traced>(exception);

			std::cerr << "FATAL: " << exception.what() << std::endl;

			if (stacktrace)
			{
				std::cerr << '\n' << *stacktrace << '\n';
			} });
	}

	catch (...)
	{
		Utilities::Console::Write(Utilities::Severity::Fatal, []()
								  { std::cerr << "FATAL: caught unhandled exception" << std::endl; });
	}

	return EXIT_FAILURE;
}

namespace
{

	UserSettings CreateUserSettings(const Options &options)
	{
		UserSettings userSettings{};

		userSettings.Benchmark = options.Benchmark;
		userSettings.BenchmarkNextScenes = options.BenchmarkNextScenes;
		userSettings.BenchmarkMaxTime = options.BenchmarkMaxTime;

		userSettings.SceneIndex = options.SceneIndex;

		userSettings.Width = options.Width;
		userSettings.Height = options.Height;

		userSettings.IsRayTraced = true;
		userSettings.AccumulateRays = true;
		userSettings.NumberOfSamples = options.Samples;
		userSettings.NumberOfBounces = options.Bounces;
		userSettings.NumberOfShadows = options.ShadowRays;
		userSettings.MaxNumberOfSamples = options.MaxSamples;

		userSettings.ShaderType = options.ShaderType;

		userSettings.ShowSettings = !options.Benchmark;
		userSettings.ShowOverlay = true;

		userSettings.ShowHeatmap = false;
		userSettings.HeatmapScale = 1.5f;

		return userSettings;
	}

	void PrintVulkanSdkInformation()
	{
		std::cout << "Vulkan SDK Header Version: " << VK_HEADER_VERSION << std::endl;
		std::cout << std::endl;
	}

	void PrintVulkanInstanceInformation(const Vulkan::Application &application, const bool benchmark)
	{
		if (benchmark)
		{
			return;
		}

		std::cout << "Vulkan Instance Extensions: " << std::endl;

		for (const auto &extension : application.Extensions())
		{
			std::cout << "- " << extension.extensionName << " (" << Vulkan::Version(extension.specVersion) << ")" << std::endl;
		}

		std::cout << std::endl;
	}

	void PrintVulkanLayersInformation(const Vulkan::Application &application, const bool benchmark)
	{
		if (benchmark)
		{
			return;
		}

		std::cout << "Vulkan Instance Layers: " << std::endl;

		for (const auto &layer : application.Layers())
		{
			std::cout
				<< "- " << layer.layerName
				<< " (" << Vulkan::Version(layer.specVersion) << ")"
				<< " : " << layer.description << std::endl;
		}

		std::cout << std::endl;
	}

	void PrintVulkanDevices(const Vulkan::Application &application)
	{
		std::cout << "Vulkan Devices: " << std::endl;

		for (const auto &device : application.PhysicalDevices())
		{
			VkPhysicalDeviceDriverProperties driverProp{};
			driverProp.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES;

			VkPhysicalDeviceProperties2 deviceProp{};
			deviceProp.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
			deviceProp.pNext = &driverProp;

			vkGetPhysicalDeviceProperties2(device, &deviceProp);

			VkPhysicalDeviceFeatures features;
			vkGetPhysicalDeviceFeatures(device, &features);

			const auto &prop = deviceProp.properties;

			const Vulkan::Version vulkanVersion(prop.apiVersion);
			const Vulkan::Version driverVersion(prop.driverVersion, prop.vendorID);

			std::cout << "- [" << prop.deviceID << "] ";
			std::cout << Vulkan::Strings::VendorId(prop.vendorID) << " '" << prop.deviceName;
			std::cout << "' (";
			std::cout << Vulkan::Strings::DeviceType(prop.deviceType) << ": ";
			std::cout << "vulkan " << vulkanVersion << ", ";
			std::cout << "driver " << driverProp.driverName << " " << driverProp.driverInfo << " - " << driverVersion;
			std::cout << ")" << std::endl;
		}

		std::cout << std::endl;
	}

	void PrintVulkanSwapChainInformation(const Vulkan::Application &application, const bool benchmark)
	{
		const auto &swapChain = application.SwapChain();

		std::cout << "Swap Chain: " << std::endl;
		std::cout << "- image count: " << swapChain.Images().size() << std::endl;
		std::cout << "- present mode: " << swapChain.PresentMode() << std::endl;
		std::cout << std::endl;
	}

	void SetVulkanDevice(Vulkan::Application &application)
	{
		const auto &physicalDevices = application.PhysicalDevices();
		const auto result = std::find_if(physicalDevices.begin(), physicalDevices.end(), [](const VkPhysicalDevice &device)
										 {
			// We want a device with geometry shader support.
			VkPhysicalDeviceFeatures deviceFeatures;
			vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

			if (!deviceFeatures.geometryShader)
			{
				return false;
			}

			// We want a device with a graphics queue.
			const auto queueFamilies = Vulkan::GetEnumerateVector(device, vkGetPhysicalDeviceQueueFamilyProperties);
			const auto hasGraphicsQueue = std::find_if(queueFamilies.begin(), queueFamilies.end(), [](const VkQueueFamilyProperties& queueFamily)
			{
				return queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT;
			});

			return hasGraphicsQueue != queueFamilies.end(); });

		if (result == physicalDevices.end())
		{
			Throw(std::runtime_error("cannot find a suitable device"));
		}

		VkPhysicalDeviceProperties2 deviceProp{};
		deviceProp.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
		vkGetPhysicalDeviceProperties2(*result, &deviceProp);

		std::cout << "Setting Device [" << deviceProp.properties.deviceID << "]: " << deviceProp.properties.deviceName << std::endl;

		// Setting Device [0]: llvmpipe (LLVM 10.0.0, 256 bits)

		application.SetPhysicalDevice(*result);

		std::cout << std::endl;
	}

}
