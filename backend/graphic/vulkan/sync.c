#include <vulkan/vulkan.h>

#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

#include "backend/graphic/vulkan/vulkan.h"

bool _mtr_create_vulkan_semaphores(VkSemaphore **out, VkDevice device, unsigned count) {
    assert(out != NULL);
    assert(device != NULL);

    VkSemaphore *semaphores = calloc(count, sizeof(VkSemaphore));
    if (semaphores == NULL) return _mtr_throw_vulkan_app_error(MTR_VULKAN_APP_ERROR_ALLOCATE);

    *out = semaphores;

    VkSemaphoreCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
    };

    for (unsigned i = 0; i < count; ++i) {
        if (_mtr_throw_vulkan_error(vkCreateSemaphore(device, &info, NULL, semaphores + i))) return true;
    }

    return false;
}

void _mtr_destroy_vulkan_semaphores(VkSemaphore **semaphores, VkDevice device, unsigned count) {
    assert(semaphores != NULL);

    if (*semaphores == NULL) return;

    for (unsigned i = 0; i < count; ++i) {
        if ((*semaphores)[i] == NULL) break;

        vkDestroySemaphore(device, (*semaphores)[i], NULL);
    }

    free(*semaphores);

    *semaphores = NULL;
}

bool _mtr_create_vulkan_fences(VkFence **out, VkDevice device, unsigned count) {
    assert(out != NULL);
    assert(device != NULL);

    VkFence *fences = calloc(count, sizeof(VkFence));
    if (fences == NULL) return _mtr_throw_vulkan_app_error(MTR_VULKAN_APP_ERROR_ALLOCATE);

    *out = fences;

    VkFenceCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };

    for (unsigned i = 0; i < count; ++i) {
        if (_mtr_throw_vulkan_error(vkCreateFence(device, &info, NULL, fences + i))) return true;
    }

    return false;
}

void _mtr_destroy_vulkan_fences(VkFence **fences, VkDevice device, unsigned count) {
    assert(fences != NULL);

    if (*fences == NULL) return;

    for (unsigned i = 0; i < count; ++i) {
        if ((*fences)[i] == NULL) break;

        vkDestroyFence(device, (*fences)[i], NULL);
    }

    free(*fences);

    *fences = NULL;
}
