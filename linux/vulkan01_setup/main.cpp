/*
 * main.cpp
 *
 *  Created on: 05-Jan-2017
 *      Author: gsarwal
 */

#include <stdio.h>
#include <vulkan/vulkan.h>
#define APP_SHORT_NAME "Vulkan01"
uint8_t idx = -1;
int main(){
	/** Refer to https://vulkan-tutorial.com/Overview
	 *
	 *
	 * Things to know
	 *
	 * - Create basic instances
	 * - All operations are submitted to a queue
	 *
	 * Things to Do
	 *
	 * 1. Setup VkInstance
	 * 2. Choose a VkDevice using VkPhysicalDeviceFeatures
	 * 3. Setup a windows surface (GLFW maybe) and setup a VkSwapChainKHR
	 * 4. Setup a VkImageView (used to get render target from swap chain) and VkFrameBuffer
	 * 5. Setup Render Passes
	 * 6. Setup the Graphic Pipeline using VkPipeline - this is where things get different, you *have* to
	 * 		create all possible states of a pipeline, so changing a shader also means a new pipeline, sounds
	 * 		unnatural but that's the way it is. there is some good news to this.
	 * 7. All operation need to be submitted to a VkQueue. Before these are submitted they must be recorded to a
	 * 		command vkCommandBuffer, these buffers need to be allocated from a VkCommandPool. I like to think of
	 * 		this like glDisplayLists
	 * 8. Now we fall into our main loop (game loop):
	 * 		vkAcquireNextImageKHR - acquires the next image from the swap chain
	 * 		vkQueueSubmit - select the appropriate command buffer and submit to the queue
	 * 		vkQueuePresentKHR - Present?
	 */

	VkApplicationInfo appInfo = {};

	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = NULL;
	appInfo.pApplicationName = APP_SHORT_NAME;
	appInfo.applicationVersion = 1;
	appInfo.pEngineName = APP_SHORT_NAME;
	appInfo.engineVersion = 1;
	appInfo.apiVersion = VK_API_VERSION_1_0;

	/** 2. Create Instance Information **/
	VkInstanceCreateInfo icInfo = {};
	icInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	icInfo.pNext = NULL;
	icInfo.flags = 0;
	icInfo.pApplicationInfo = & appInfo;
	icInfo.enabledExtensionCount = 0;
	icInfo.ppEnabledExtensionNames = NULL;
	icInfo.enabledLayerCount = 0;

	VkInstance instance;
	VkResult res;

	/** 3. create the instance **/
	res = vkCreateInstance(&icInfo, NULL, &instance);
	if(res) {
		if(res == VK_ERROR_INCOMPATIBLE_DRIVER){
			printf("Cannot find a Vulkan Compatible ICD\n");
			return VK_ERROR_INITIALIZATION_FAILED;
		}
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	printf("Yay! Vulkan is initialized\n");

	/** Never forget to clean up **/
	vkDestroyInstance(instance, NULL);

	return 0;
}


