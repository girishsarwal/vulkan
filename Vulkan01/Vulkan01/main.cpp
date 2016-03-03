#include <Windows.h>
#include <vulkan.h>
#include <conio.h>
#include <stdio.h>
#include <vector>

#define APP_SHORT_NAME "Vulkan01"

int main(){
	VkApplicationInfo appInfo = {};
	
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = NULL;
	appInfo.pApplicationName = APP_SHORT_NAME;
	appInfo.applicationVersion = 1;
	appInfo.pEngineName = APP_SHORT_NAME;
	appInfo.engineVersion = 1;
	appInfo.apiVersion = VK_API_VERSION;

	VkInstanceCreateInfo icInfo = {};
	icInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	icInfo.pNext = NULL;
	icInfo.flags = 0;
	icInfo.pApplicationInfo = & appInfo;
	icInfo.enabledExtensionCount = 0;
	icInfo.ppEnabledExtensionNames = NULL;
	icInfo.enabledLayerCount = 0;
	icInfo.enabledLayerCount = NULL;

	VkInstance instance;
	VkResult res;

	res = vkCreateInstance(&icInfo, NULL, &instance);
	if(res == VK_ERROR_INCOMPATIBLE_DRIVER){
		printf("Cannot find a Vulkan Compatible ICD\n");
		return -1;
	} else if(res){
		printf("Some error occured\n");
		return (-1);
	}
	printf("Yay! Vulkan is initialized\n");

	uint32_t gpuCount = 1;
	
	res = vkEnumeratePhysicalDevices(instance, &gpuCount, NULL);
	printf("found %d gpus\n", gpuCount);
	VkPhysicalDevice* devices = new VkPhysicalDevice[gpuCount];

	printf("Listing gpus...\n", gpuCount);
	res = vkEnumeratePhysicalDevices(instance, &gpuCount, devices);
	uint8_t idx = -1;
	while(++idx < gpuCount){
		VkPhysicalDeviceProperties props= {};
		vkGetPhysicalDeviceProperties(devices[idx], &props);
		printf("%d-%d-%d-%d-%s", props.apiVersion, props.driverVersion, props.vendorID, props.deviceID, props.deviceName);
	}
	delete[] devices;
	vkDestroyInstance(instance, NULL);
	return 0;
}





