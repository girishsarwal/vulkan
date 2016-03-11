/** THIS IS A WINDOWS APP **/

#define VK_USE_PLATFORM_WIN32_KHR		/** THIS IS REQUIRED. use before including vulkan.h to 
activate all WIN32 specific control structures available **/
#include <Windows.h>
#include <vulkan.h>
#include <conio.h>
#include <stdio.h>
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define APP_SHORT_NAME "Vulkan03"
#define APP_SHORT_NAME_UNICODE L"Vulkan03"

uint8_t idx = -1;
HMODULE hModule;
VkResult res;

LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int nShowCmd){

	/** Window Creation, the nostalgic windows way **/
	hModule = GetModuleHandle(NULL);

	WNDCLASSEX wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC) WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wcex.lpszClassName = APP_SHORT_NAME_UNICODE;
	wcex.lpszMenuName = NULL;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hIconSm = LoadIcon(NULL, IDI_WINLOGO);


	if(!RegisterClassEx(&wcex)){
		printf("There was a problem registering the class. Aborting...\n");
		exit(-1);
	}

	RECT rt = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
	AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, FALSE);

	HWND hWnd = CreateWindowEx(0,
		wcex.lpszClassName, 
		APP_SHORT_NAME_UNICODE,
		WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_SYSMENU,
		100, 
		100, 
		rt.right - rt.left,
		rt.bottom - rt.top,
		NULL,
		NULL,
		hModule,
		NULL);

	ShowWindow(hWnd, nShowCmd);


	/** Window creation ends, all we need is the hModule, hWnd, width and height to proceed **/
	/** Vulkan stuff **/

	VkApplicationInfo ai = {};
	ai.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	ai.pNext = NULL;
	ai.pApplicationName = APP_SHORT_NAME;
	ai.applicationVersion = 1;
	ai.pEngineName = APP_SHORT_NAME;
	ai.engineVersion = 1;
	ai.apiVersion = VK_API_VERSION;

	const char* extensions[] = {VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME};	/** these are just defines arouns string extension names **/

	VkInstanceCreateInfo ici = {};
	ici.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	ici.pNext = NULL;
	ici.enabledExtensionCount = 2;
	ici.ppEnabledExtensionNames = extensions;
	ici.pApplicationInfo = &ai;
	ici.enabledLayerCount = 0;
	ici.flags = 0;
	ici.ppEnabledLayerNames = NULL;

	VkInstance instance;
	res = vkCreateInstance(&ici, NULL, &instance);

	if(res == VK_ERROR_INCOMPATIBLE_DRIVER){
		printf("Cannot create instance. Cannot find a Vulkan compatible ICD. Aborting...\n");
		exit(-1);
	} else if(res){
		printf("Cannot create instance. Something just didn't work! Aborting...\n");
		exit(-1);
	}

	/** readup the GPUs **/
	uint32_t gpuCount = 0;
	res = vkEnumeratePhysicalDevices(instance, &gpuCount, NULL);
	printf("found %d gpus", gpuCount);

	VkPhysicalDevice* gpus = new VkPhysicalDevice[gpuCount];
	res = vkEnumeratePhysicalDevices(instance, &gpuCount, gpus);

	idx = -1;
	while(++idx <gpuCount){
		VkPhysicalDeviceProperties props = {};
		vkGetPhysicalDeviceProperties(gpus[idx], &props);
	}


	/** create a surface <-- THIS IS IMPORTANT. We will be querying our queues 
	subsequently to see if any of the queues support GRAPHICS and  also support 
	SURFACE PRESENTATION**/

	VkWin32SurfaceCreateInfoKHR sci = {};
	sci.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	sci.pNext = NULL;
	sci.hinstance = hModule;
	sci.hwnd = hWnd;

	VkSurfaceKHR surface;

	res = vkCreateWin32SurfaceKHR(instance, &sci, NULL, &surface);
	if(res){
		printf("Cannot create render surface. Aborting...\n");
		exit(-1);
	}


	/** query for all queues available **/
	uint32_t queueCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(gpus[0], &queueCount, NULL);
	if(queueCount <=0){
		printf("No Available Queues\n");
		exit(-1);
	}

	VkQueueFamilyProperties* queue_properties = new VkQueueFamilyProperties[queueCount];
	vkGetPhysicalDeviceQueueFamilyProperties(gpus[0], &queueCount, queue_properties);
	
	uint32_t selectedQueueFamilyIndex = 0;
	
	
	idx = -1;
	while(++idx < queueCount){
		VkBool32 supportPresentation = false;
		if(queue_properties[idx].queueFlags & VK_QUEUE_GRAPHICS_BIT){
			/** check if this also support the PRESENT SURFACE **/
			vkGetPhysicalDeviceSurfaceSupportKHR(gpus[0], idx, surface, &supportPresentation);
			if(supportPresentation == VK_TRUE){
				selectedQueueFamilyIndex = idx;
			}
			break;
		}
	}



	/** lets make a queue **/
	VkDeviceQueueCreateInfo qci = {};
	float queue_prirorites[1] = {0.0};

	qci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	qci.pNext = NULL;
	qci.queueCount = 1;
	qci.pQueuePriorities = queue_prirorites;
	qci.queueFamilyIndex = selectedQueueFamilyIndex;

	/** create a device **/

	VkDeviceCreateInfo dci = {};
	dci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	dci.pNext = NULL;
	dci.queueCreateInfoCount = 1;
	dci.pQueueCreateInfos = &qci;
	dci.enabledExtensionCount = 0;
	dci.ppEnabledExtensionNames = NULL;
	dci.enabledLayerCount = 0;
	dci.ppEnabledLayerNames = NULL;
	dci.pEnabledFeatures = NULL;

	VkDevice device;
	res = vkCreateDevice(gpus[0], &dci, NULL, &device);
	if(res){
		printf("There was a problem creating the device. Aborting...\n");
		exit(-1);
	}


	/** get the VkFormats that the surface supports **/
	uint32_t formatsCount;
	res = vkGetPhysicalDeviceSurfaceFormatsKHR(gpus[0], surface, &formatsCount, NULL);
	VkSurfaceFormatKHR* formats = new VkSurfaceFormatKHR[formatsCount];
	
	res = vkGetPhysicalDeviceSurfaceFormatsKHR(gpus[0], surface, &formatsCount, formats);

	if(formatsCount <=0){
		printf("Cannot find adequate formats on device. Aborting...\n");
		exit(-1);
	}

	if(formatsCount == 1 && formats[0].format == VK_FORMAT_UNDEFINED){
		
	}

	/** get physical device capabilities **/
	VkSurfaceCapabilitiesKHR capabilities;
	res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpus[0], surface, &capabilities);

	if(res){
		printf("Cannot read device case. Aborting...\n");
		exit(-1);
	}

	/** get present modes for surface**/
	uint32_t presentModeCount = 0;
	res = vkGetPhysicalDeviceSurfacePresentModesKHR(gpus[0], surface, &presentModeCount, NULL);
	VkPresentModeKHR* presentModes = (VkPresentModeKHR*) malloc (presentModeCount * sizeof(VkPresentModeKHR));
	
	res = vkGetPhysicalDeviceSurfacePresentModesKHR(gpus[0], surface, &presentModeCount, presentModes);
	if(res){
		printf("Cannot get present modes. Aborting...\n");
		exit(-1);
	}



	VkCommandPoolCreateInfo cpci = {};
	cpci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cpci.pNext = NULL;
	cpci.queueFamilyIndex = qci.queueFamilyIndex;
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

	
	/** PHEW! **/


	
	/** End Vulkan stuff **/

	/** fall into the main loop **/
	MSG msg;
	while(true){
		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) == TRUE){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if(msg.message == WM_QUIT){
				break;
		}
	}

	
	/** no, we never leave the crap behind **/

	vkFreeCommandBuffers(device, cp, 1, commandBuffer);
	vkDestroyCommandPool(device, cp, NULL);
	vkDestroyDevice(device, NULL);
	printf("Device created");
	vkDestroyInstance(instance, NULL);

	delete[] formats;
	delete[] queue_properties;
	delete[] gpus;

	return msg.wParam;
}

LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg){	
	case WM_CLOSE:
		DestroyWindow(hWnd);
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}



