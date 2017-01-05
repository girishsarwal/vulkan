#include <Windows.h>
#include <vulkan.h>
#include <conio.h>
#include <stdio.h>

#define APP_SHORT_NAME "Vulkan01"
uint8_t idx = -1;
int main(){
	printf("This code initializes a Vulkan instance");
	/**
	Steps required
	1. Create Application Info
	2. Create Instance info
	3. Create the instance
	**/
	
	/** 1. Create Application Info**/
	VkApplicationInfo appInfo = {};
	
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = NULL;
	appInfo.pApplicationName = APP_SHORT_NAME;
	appInfo.applicationVersion = 1;
	appInfo.pEngineName = APP_SHORT_NAME;
	appInfo.engineVersion = 1;
	appInfo.apiVersion = VK_API_VERSION;

	/** 2. Create Instance Information **/
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

	/** 3. create the instance **/
	res = vkCreateInstance(&icInfo, NULL, &instance);
	if(res == VK_ERROR_INCOMPATIBLE_DRIVER){
		printf("Cannot find a Vulkan Compatible ICD\n");
		exit(-1);
	} else if(res){
		printf("Some error occured\n");
		exit(-1);
	}
	printf("Yay! Vulkan is initialized\n");

	/** Never forget to clean up **/
	vkDestroyInstance(instance, NULL);
	
	return 0;
}





