#include <Windows.h>
#include <vulkan.h>
#include <conio.h>
#include <stdio.h>

#define APP_SHORT_NAME "Vulkan02"
uint8_t idx = -1;
int main(){
	printf("This code initializes a Vulkan device");
	/**
	Steps required
	1. Create Vulkan Instance
	2. Enumerate the GPUs
	3. Query Queues on the GPU (Queues represent on what 'channels' will the data process, query the queue
		for COMPUTE type queue or GRAPHICS type queue
	4. Create a queue priority
	5. Create a device with information from 2, 3 and 4
	**/

	/** 1. Create Vulkan Instance **/
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
		exit(-1);
	} else if(res){
		printf("Some error occured\n");
		exit(-1);
	}
	printf("Yay! Vulkan is initialized\n");

	/** 2. Enumerate the GPUs **/
	uint32_t gpuCount = 0;

	res = vkEnumeratePhysicalDevices(instance, &gpuCount, NULL);
	printf("found %d gpus\n", gpuCount);
	VkPhysicalDevice* gpus = new VkPhysicalDevice[gpuCount];

	printf("Listing gpus...\n", gpuCount);
	res = vkEnumeratePhysicalDevices(instance, &gpuCount, gpus);

	while(++idx < gpuCount){
		VkPhysicalDeviceProperties props= {};
		vkGetPhysicalDeviceProperties(gpus[idx], &props);
		printf("%d-%d-%d-%d-%s\n", props.apiVersion, props.driverVersion, props.vendorID, props.deviceID, props.deviceName);
	}


	/** 3. Query for the supported queues **/
	uint32_t queue_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(gpus[0], &queue_count, NULL);
	if(queue_count <= 0){
		printf("No Queues found.. aborting\n");
		exit(-1);
	}

	VkQueueFamilyProperties* queue_props = new VkQueueFamilyProperties[queue_count];

	vkGetPhysicalDeviceQueueFamilyProperties(gpus[0], &queue_count, queue_props);

	float queue_priorities[1] = {0.0};		/** 4. Create a queue priority **/
	VkDeviceQueueCreateInfo qi = {};
	qi.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	qi.pNext = NULL;
	qi.queueCount = 1;
	qi.pQueuePriorities = queue_priorities;

	idx = -1;
	while(++idx < queue_count){
		if(queue_props[idx].queueFlags & VK_QUEUE_GRAPHICS_BIT){
			qi.queueFamilyIndex = idx;
			break;
		}
	}

	/** 5. Create a device with information from 2, 3 and 4 **/

	VkDeviceCreateInfo dci = {};
	dci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	dci.pNext = NULL;
	dci.queueCreateInfoCount = 1;
	dci.pQueueCreateInfos = &qi;
	dci.enabledExtensionCount = 0;
	dci.ppEnabledExtensionNames = NULL;
	dci.enabledLayerCount = 0;
	dci.ppEnabledLayerNames = NULL;
	dci.pEnabledFeatures = NULL;

	VkDevice device;
	res = vkCreateDevice(gpus[0], &dci, NULL, &device);
	if(res){
		printf("There was a problem creating the device");
		exit(-1);
	}

	/** All's great, first thing lets create a command buffer **/

	VkCommandPoolCreateInfo cpci = {};
	cpci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cpci.pNext = NULL;
	cpci.queueFamilyIndex = qi.queueFamilyIndex;
	cpci.flags = 0;

	VkCommandPool cp;
	res = vkCreateCommandPool(device, &cpci, NULL, &cp);
	if(res){
		printf("There was a problem creating a command pool");
		exit(-1);
	}

	/** Create a command buffer from the command pool **/
	VkCommandBufferAllocateInfo cbai = {};
	cbai.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cbai.pNext = NULL;
	cbai.commandPool = cp;
	cbai.commandBufferCount = 1;
	cbai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

	VkCommandBuffer* commandBuffer = new VkCommandBuffer[1];	//single command

	res = vkAllocateCommandBuffers(device, &cbai, commandBuffer);

	if(res){
		printf("There was a problem creating a command buffer");
		exit(-1);
	}

	/** no, we never leave the crap behind **/

	vkFreeCommandBuffers(device, cp, 1, commandBuffer);
	vkDestroyCommandPool(device, cp, NULL);
	vkDestroyDevice(device, NULL);
	printf("Device created");
	vkDestroyInstance(instance, NULL);

	delete[] gpus;

	return 0;
}
