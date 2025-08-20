#ifndef GRAPHIC_VULKAN_H
#define GRAPHIC_VULKAN_H

#include <vulkan/vulkan.h>

#include <stdbool.h>

#include "game/window.h"
#include "game/graphic.h"

#define MTR_VULKAN_FRAMES_IN_FLIGHT 1

struct MTRGraphicObject {
    VkBuffer mesh_buffer;
    VkDeviceMemory mesh_memory;
    VkBuffer index_buffer;
    VkDeviceMemory index_memory;

    unsigned count;
};

struct MTRGraphic {
    VkSurfaceCapabilitiesKHR capabilities;
    VkSurfaceFormatKHR format;
    VkExtent2D extent;

    MTRWindow window;

    VkImage *images;
    VkImageView *views;

    VkInstance instance;
    VkSurfaceKHR surface;
    VkPhysicalDevice physical;
    VkDevice device;
    VkQueue graphic;
    VkQueue present;
    VkSwapchainKHR swapchain;
    VkCommandPool graphic_pool;
    VkCommandBuffer graphic_cmd;
    VkCommandBuffer extra_cmd;

    VkShaderModule base_vertex;
    VkShaderModule base_fragment;
    VkRenderPass base_render_pass;
    VkPipelineLayout base_layout;
    VkPipeline base_pipeline;
    VkFramebuffer *base_framebuffers;

    VkSemaphore *image_available;
    VkSemaphore *render_finished;
    VkFence *frame_ended;

    VkPresentModeKHR mode;
    unsigned graphic_index;
    unsigned present_index;
    unsigned image_count;
    unsigned frame_index;
};

enum MTRVulkanAppError {
    MTR_VULKAN_APP_ERROR_NO_GRAPHIC_FAMILY = 0x01,
    MTR_VULKAN_APP_ERROR_NO_PRESENT_FAMILY = 0x02,
    MTR_VULKAN_APP_ERROR_ALLOCATE = 0x03,
    MTR_VULKAN_APP_ERROR_MEMORY_FLAGS_NOT_FOUND = 0x04,
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

bool _mtr_load_vulkan_swapchain_images(VkImage **images, unsigned *count, VkSwapchainKHR swapchain, VkDevice device);
void _mtr_destroy_vulkan_swapchain_images(VkImage **images);

bool _mtr_create_vulkan_swapchain_views(VkImageView **views, const VkImage *images, VkDevice device, VkFormat format, unsigned count);
void _mtr_destroy_vulkan_swapchain_views(VkImageView **views, VkDevice device, unsigned count);

bool _mtr_create_vulkan_shader_module(VkShaderModule *module, VkDevice device, const char *code, unsigned size);
void _mtr_destroy_vulkan_shader_module(VkShaderModule module, VkDevice device);

bool _mtr_create_base_vulkan_render_pass(VkRenderPass *render_pass, VkDevice device, VkFormat format);
void _mtr_destroy_vulkan_render_pass(VkRenderPass render_pass, VkDevice device);

bool _mtr_create_base_vulkan_pipeline_layout(VkPipelineLayout *layout, VkDevice device);
void _mtr_destroy_vulkan_pipeline_layout(VkPipelineLayout layout, VkDevice device);

bool _mtr_create_base_vulkan_pipeline(VkPipeline *pipeline, VkDevice device, VkPipelineLayout layout, VkRenderPass render_pass, VkShaderModule vertex, VkShaderModule fragment, const VkExtent2D *extent);
void _mtr_destroy_vulkan_pipeline(VkPipeline pipeline, VkDevice device);

bool _mtr_create_vulkan_framebuffers(VkFramebuffer **framebuffers, VkDevice device, VkRenderPass render_pass, const VkImageView *views, const VkExtent2D *extent, unsigned count);
void _mtr_destroy_vulkan_framebuffers(VkFramebuffer **framebuffers, VkDevice device, unsigned count);

bool _mtr_create_vulkan_command_pool(VkCommandPool *pool, VkDevice device, unsigned queue_index);
void _mtr_destroy_vulkan_command_pool(VkCommandPool pool, VkDevice device);

bool _mtr_create_vulkan_command_buffer(VkCommandBuffer *cmd, VkDevice device, VkCommandPool pool);

bool _mtr_create_vulkan_semaphores(VkSemaphore **semaphores, VkDevice device, unsigned count);
void _mtr_destroy_vulkan_semaphores(VkSemaphore **semaphores, VkDevice device, unsigned count);

bool _mtr_create_vulkan_fences(VkFence **fences, VkDevice device, unsigned count);
void _mtr_destroy_vulkan_fences(VkFence **fences, VkDevice device, unsigned count);

bool _mtr_recreate_vulkan_swapchain(VkSwapchainKHR *swapchain, VkFramebuffer **framebuffers, VkImageView **views, VkImage **images, unsigned *image_count, VkSurfaceCapabilitiesKHR *capabilities, VkExtent2D *extent, VkSurfaceFormatKHR *format, VkPresentModeKHR *mode, VkDevice device, VkPhysicalDevice physical, VkSurfaceKHR surface, VkRenderPass render_pass, MTRWindow window, unsigned graphic_index, unsigned present_index);

bool _mtr_create_vulkan_buffer(VkBuffer *buffer, VkDevice device, VkBufferUsageFlags usage, unsigned size);
void _mtr_destroy_vulkan_buffer(VkBuffer buffer, VkDevice device);

bool _mtr_create_vulkan_buffer_memory(VkDeviceMemory *memory, VkDevice device, VkBuffer buffer, VkPhysicalDevice physical, VkMemoryPropertyFlags flags);
void _mtr_destroy_vulkan_memory(VkDeviceMemory memory, VkDevice device);

bool _mtr_copy_vulkan_buffers(VkBuffer dst, VkBuffer src, VkDevice device, VkCommandBuffer cmd, VkQueue queue, unsigned size);
bool _mtr_create_staged_vulkan_buffer(VkBuffer *buffer, VkDeviceMemory *memory, VkDevice device, VkPhysicalDevice physical, VkCommandBuffer cmd, VkQueue queue, VkBufferUsageFlags usage, VkMemoryPropertyFlags flags, const void *data, unsigned size);

// TODO: add default method
bool _mtr_query_vulkan_physical_presentation_support(bool *is_supported, VkPhysicalDevice physical, unsigned family, MTRWindow window);
void _mtr_query_vulkan_instance_extensions(const char ***exts, unsigned *count);

// If result is not an error, returns false
bool _mtr_throw_vulkan_error(VkResult result);
bool _mtr_throw_vulkan_app_error(MTRVulkanAppError code);

#endif
