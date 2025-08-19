#ifndef GRAPHIC_VULKAN_H
#define GRAPHIC_VULKAN_H

#include <vulkan/vulkan.h>

#include <stdbool.h>

#include "game/window.h"

struct MTRVulkanRenderPass {
    VkFramebuffer *framebuffers;

    VkRenderPass render_pass;

    unsigned count;
};
typedef struct MTRVulkanRenderPass MTRVulkanRenderPass;

struct MTRVulkanPipeline {
    VkShaderModule vertex;
    VkShaderModule fragment;
    VkPipelineLayout layout;
    VkPipeline pipeline;
};
typedef struct MTRVulkanPipeline MTRVulkanPipeline;

struct MTRGraphic {
    VkSurfaceCapabilitiesKHR capabilities;
    VkSurfaceFormatKHR format;
    VkExtent2D extent;
    MTRVulkanRenderPass base_render_pass;
    MTRVulkanPipeline base_pipeline;

    VkImage *images;
    VkImageView *views;

    VkInstance instance;
    VkSurfaceKHR surface;
    VkPhysicalDevice physical;
    VkDevice device;
    VkQueue graphic;
    VkQueue present;
    VkSwapchainKHR swapchain;

    VkPresentModeKHR mode;
    unsigned graphic_index;
    unsigned present_index;
    unsigned image_count;
};

enum MTRVulkanAppError {
    MTR_VULKAN_APP_ERROR_NO_GRAPHIC_FAMILY = 0x01,
    MTR_VULKAN_APP_ERROR_NO_PRESENT_FAMILY = 0x02,
    MTR_VULKAN_APP_ERROR_ALLOCATE = 0x03,
};
typedef enum MTRVulkanAppError MTRVulkanAppError;

bool _mtr_create_vulkan_instance(VkInstance *instance);
void _mtr_destroy_vulkan_instance(VkInstance instance);

bool _mtr_load_vulkan_physical(VkPhysicalDevice *physical, VkInstance instance);
bool _mtr_load_vulkan_queue_families(unsigned *graphic_index, unsigned *present_index, VkPhysicalDevice physical, MTRWindow window);

bool _mtr_query_vulkan_surface(VkSurfaceKHR *surface, MTRWindow window, VkInstance instance);
void _mtr_destroy_vulkan_surface(VkSurfaceKHR surface, VkInstance instance);
bool _mtr_load_vulkan_surface_capabilities(VkSurfaceCapabilitiesKHR *capabilities, VkExtent2D *extent, VkPhysicalDevice physical, VkSurfaceKHR surface, MTRWindow window);
bool _mtr_load_vulkan_surface_format(VkSurfaceFormatKHR *format, VkPhysicalDevice physical, VkSurfaceKHR surface);
bool _mtr_load_vulkan_surface_present_mode(VkPresentModeKHR *mode, VkPhysicalDevice physical, VkSurfaceKHR surface);

bool _mtr_create_vulkan_device(VkDevice *device, VkQueue *graphic, VkQueue *present, VkPhysicalDevice physical, unsigned graphic_index, unsigned present_index);
void _mtr_destroy_vulkan_device(VkDevice device);

bool _mtr_create_vulkan_swapchain(VkSwapchainKHR *swapchain, const VkSurfaceCapabilitiesKHR *capabilities, const VkSurfaceFormatKHR *format, const VkExtent2D *extent, VkDevice device, VkSurfaceKHR surface, VkPresentModeKHR mode, unsigned graphic_index, unsigned present_index);
void _mtr_destroy_vulkan_swapchain(VkSwapchainKHR swapchain, VkDevice device);

bool _mtr_load_vulkan_swapchain_images(VkImage **out, unsigned *count, VkSwapchainKHR swapchain, VkDevice device);
void _mtr_destroy_vulkan_swapchain_images(VkImage **images);

bool _mtr_create_vulkan_swapchain_views(VkImageView **out, const VkImage *images, VkDevice device, VkFormat format, unsigned count);
void _mtr_destroy_vulkan_swapchain_views(VkImageView **views, VkDevice device, unsigned count);

bool _mtr_create_vulkan_shader_module(VkShaderModule *module, VkDevice device, const char *code, unsigned size);
void _mtr_destroy_vulkan_shader_module(VkShaderModule module, VkDevice device);

// TODO: add default method
bool _mtr_query_vulkan_physical_presentation_support(bool *is_supported, VkPhysicalDevice physical, unsigned family, MTRWindow window);
void _mtr_query_vulkan_instance_extensions(const char ***exts, unsigned *count);

// If result is not an error, returns false
bool _mtr_throw_vulkan_error(VkResult result);
bool _mtr_throw_vulkan_app_error(MTRVulkanAppError code);

#endif
