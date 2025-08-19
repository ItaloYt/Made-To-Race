#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "backend/graphic/vulkan/vulkan.h"

bool _mtr_create_vulkan_instance(VkInstance *out) {
    static const char *const exts_default[] = {};
    static const unsigned count_default = sizeof(exts_default) / sizeof(const char *);

    assert(out != NULL);

    // Load query extensions and merge with default extensions
    const char **exts_query;
    unsigned count_query;

    _mtr_query_vulkan_instance_extensions(&exts_query, &count_query);
    
    unsigned count = count_default + count_query;
    const char *exts[count];

    (void) memcpy(exts, exts_query, count_query * sizeof(const char *));
    (void) memcpy(exts + count_query, exts_default, count_default * sizeof(const char *));

    // Create instance
    VkInstanceCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .pApplicationInfo = &(VkApplicationInfo) {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = NULL,
            .pApplicationName = "Made To Race",
            .applicationVersion = VK_MAKE_API_VERSION(0, 0, 0, 0),
            .pEngineName = NULL,
            .engineVersion = 0,
            .apiVersion = VK_API_VERSION_1_0,
        },
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = NULL,
        .enabledExtensionCount = count,
        .ppEnabledExtensionNames = exts,
    };

    return _mtr_throw_vulkan_error(vkCreateInstance(&info, NULL, out));
}

void _mtr_destroy_vulkan_instance(VkInstance instance) {
    vkDestroyInstance(instance, NULL);
}
