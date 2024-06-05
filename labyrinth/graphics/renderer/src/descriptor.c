#include "renderer/descriptor.h"

#include <stddef.h>
#include <vulkan/vulkan.h>

#include "renderer/core.h"
#include "utils/logging.h"
#include "utils/types.h"
#include "vulkan/vulkan_core.h"

LbrDescriptorAllocator lbrCreateDescriptorAllocator(u32 sets_per_pool, u32 max_pool_count, LbrPoolRatio* p_ratios, u32 ratio_count, VkDescriptorPoolCreateFlags flags) {
  LbrDescriptorAllocator allocator;
  allocator.sets_per_pool   = sets_per_pool;
  allocator.max_pool_count  = max_pool_count;
  allocator.full_pool_count = 0;
  allocator.open_pool_count = 0;
  allocator.pool_size_count = ratio_count;
  allocator.full_pools      = malloc(max_pool_count * sizeof(VkDescriptorPool));
  allocator.open_pools      = malloc(max_pool_count * sizeof(VkDescriptorPool));
  allocator.pool_sizes      = malloc(ratio_count * sizeof(VkDescriptorPoolSize));
  allocator.create_flags    = flags;

  for (u32 i = 0; i < ratio_count; i++) {
    allocator.pool_sizes[i].type            = p_ratios[i].type;
    allocator.pool_sizes[i].descriptorCount = (u32)(p_ratios[i].ratio * (float)sets_per_pool);
  }

  return allocator;
}

void lbrDestroyDescriptorAllocator(LbrDescriptorAllocator* p_allocator, LbrDevice* p_device) {
  for (u32 i = 0; i < p_allocator->open_pool_count; i++) {
    vkDestroyDescriptorPool(p_device->device, p_allocator->open_pools[i], NULL);
  }

  for (u32 i = 0; i < p_allocator->full_pool_count; i++) {
    vkDestroyDescriptorPool(p_device->device, p_allocator->full_pools[i], NULL);
  }

  free(p_allocator->full_pools);
  free(p_allocator->open_pools);
  free(p_allocator->pool_sizes);
}

void lbrDescriptorAllocatorAllocatePool(LbrDescriptorAllocator* p_allocator, LbrDevice* p_device) {
  LBR_ASSERT(p_allocator->full_pool_count + p_allocator->open_pool_count < p_allocator->max_pool_count);
  VkDescriptorPoolCreateInfo pool_info = {0};
  pool_info.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.flags                      = p_allocator->create_flags;
  pool_info.maxSets                    = p_allocator->sets_per_pool;
  pool_info.poolSizeCount              = p_allocator->pool_size_count;
  pool_info.pPoolSizes                 = p_allocator->pool_sizes;

  LBR_VK_CHECK(vkCreateDescriptorPool(p_device->device, &pool_info, NULL, &p_allocator->open_pools[p_allocator->open_pool_count]));
  p_allocator->open_pool_count++;
}

void lbrDescriptorAllocatorClearPools(LbrDescriptorAllocator* p_allocator, LbrDevice* p_device) {
  for (u32 i = 0; i < p_allocator->open_pool_count; i++) {
    LBR_VK_CHECK(vkResetDescriptorPool(p_device->device, p_allocator->open_pools[i], 0));
  }

  for (u32 i = 0; i < p_allocator->full_pool_count; i++) {
    LBR_VK_CHECK(vkResetDescriptorPool(p_device->device, p_allocator->full_pools[i], 0));
    p_allocator->open_pools[p_allocator->open_pool_count] = p_allocator->full_pools[i];
    p_allocator->open_pool_count++;
  }
  p_allocator->full_pool_count = 0;
}

LbrDescriptorLayout lbrCreateDescriptorLayout(u32 binding_count) {
  LbrDescriptorLayout layout = {0};
  layout.bindings            = malloc(binding_count * sizeof(VkDescriptorSetLayoutBinding));

  return layout;
}

void lbrDestroyDescriptorLayout(LbrDescriptorLayout* p_layout) { free(p_layout->bindings); }

void lbrAddDescriptorBinding(LbrDescriptorLayout* p_layout, u32 binding, VkDescriptorType type) {
  VkDescriptorSetLayoutBinding layout_binding = {0};
  layout_binding.binding                      = binding;
  layout_binding.descriptorCount              = 1;
  layout_binding.descriptorType               = type;

  p_layout->bindings[p_layout->binding_count] = layout_binding;
  p_layout->binding_count++;
}

VkDescriptorSetLayout lbrBuildDescriptorLayout(LbrDescriptorLayout* p_layout, LbrDevice* p_device, VkShaderStageFlags shader_stages) {
  VkDescriptorSetLayout set_layout;

  for (u32 i = 0; i < p_layout->binding_count; i++) {
    p_layout->bindings[i].stageFlags |= shader_stages;
  }

  VkDescriptorSetLayoutCreateInfo descriptor_layout_info = {0};
  descriptor_layout_info.sType                           = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  descriptor_layout_info.pBindings                       = p_layout->bindings;
  descriptor_layout_info.bindingCount                    = p_layout->binding_count;

  LBR_VK_CHECK(vkCreateDescriptorSetLayout(p_device->device, &descriptor_layout_info, NULL, &set_layout));

  return set_layout;
}

VkDescriptorSet lbrCreateDescriptorSet(LbrDescriptorAllocator* p_allocator, LbrDevice* p_device, VkDescriptorSetLayout layout) {
  VkDescriptorPool* pool = &p_allocator->open_pools[p_allocator->open_pool_count - 1];

  VkDescriptorSetAllocateInfo allocate_info = {0};
  allocate_info.sType                       = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocate_info.descriptorPool              = *pool;
  allocate_info.descriptorSetCount          = 1;
  allocate_info.pSetLayouts                 = &layout;

  VkDescriptorSet set;
  VkResult result = vkAllocateDescriptorSets(p_device->device, &allocate_info, &set);

  if (result == VK_ERROR_OUT_OF_POOL_MEMORY || result == VK_ERROR_FRAGMENTED_POOL) {
    p_allocator->full_pools[p_allocator->full_pool_count] = *pool;
    p_allocator->full_pool_count++;
    p_allocator->open_pool_count--;

    if (p_allocator->open_pool_count == 0) {
      lbrDescriptorAllocatorAllocatePool(p_allocator, p_device);
    }
    allocate_info.descriptorPool = p_allocator->open_pools[p_allocator->open_pool_count];

    LBR_VK_CHECK(vkAllocateDescriptorSets(p_device->device, &allocate_info, &set));
  }

  return set;
}

LbrDescriptorWriter lbrCreateDescriptorWriter(u32 max_info_count) {
  LbrDescriptorWriter writer    = {0};
  writer.image_infos            = calloc(max_info_count, sizeof(VkDescriptorImageInfo));
  writer.buffer_infos           = calloc(max_info_count, sizeof(VkDescriptorBufferInfo));
  writer.descriptor_writes      = calloc((usize)2 * max_info_count, sizeof(VkWriteDescriptorSet));
  writer.max_count              = max_info_count;
  writer.image_info_count       = 0;
  writer.buffer_info_count      = 0;
  writer.descriptor_write_count = 0;

  return writer;
}

void lbrDestroyDescriptorWriter(LbrDescriptorWriter* p_writer) {
  free(p_writer->image_infos);
  free(p_writer->buffer_infos);
  free(p_writer->descriptor_writes);
}

void lbrDescriptorWriterWriteBuffer(LbrDescriptorWriter* p_writer, u32 binding, LbrAllocatedBuffer* p_buffer, VkDescriptorType type) {
  LBR_ASSERT(p_writer->buffer_info_count < p_writer->max_count);
  LBR_ASSERT(p_writer->descriptor_write_count < (2 * p_writer->max_count));

  VkDescriptorBufferInfo* buffer_info = &p_writer->buffer_infos[p_writer->buffer_info_count];
  buffer_info->buffer                 = p_buffer->buffer;
  buffer_info->offset                 = p_buffer->info.offset;
  buffer_info->range                  = p_buffer->info.size;
  p_writer->buffer_info_count++;

  VkWriteDescriptorSet* write = &p_writer->descriptor_writes[p_writer->descriptor_write_count];
  write->sType                = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write->pNext                = NULL;
  write->dstBinding           = binding;
  write->dstSet               = VK_NULL_HANDLE;
  write->descriptorCount      = 1;
  write->descriptorType       = type;
  write->pBufferInfo          = buffer_info;
  p_writer->descriptor_write_count++;
}

void lbrDescriptorWriterWriteImage(LbrDescriptorWriter* p_writer, u32 binding, LbrImage* p_image, VkSampler sampler, VkImageLayout layout, VkDescriptorType type) {
  LBR_ASSERT(p_writer->image_info_count < p_writer->max_count);
  LBR_ASSERT(p_writer->descriptor_write_count < (2 * p_writer->max_count));
  VkDescriptorImageInfo* image_info = &p_writer->image_infos[p_writer->image_info_count];
  image_info->sampler               = sampler;
  image_info->imageView             = p_image->image_view;
  image_info->imageLayout           = layout;
  p_writer->image_info_count++;

  VkWriteDescriptorSet* write = &p_writer->descriptor_writes[p_writer->descriptor_write_count];
  write->sType                = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write->pNext                = NULL;
  write->dstBinding           = binding;
  write->dstSet               = VK_NULL_HANDLE;
  write->descriptorCount      = 1;
  write->descriptorType       = type;
  write->pImageInfo           = image_info;
  p_writer->descriptor_write_count++;
}

void lbrDescriptorWriterClear(LbrDescriptorWriter* p_writer) {
  p_writer->buffer_info_count      = 0;
  p_writer->image_info_count       = 0;
  p_writer->descriptor_write_count = 0;
}

void lbrDescriptorWriterUpdateSet(LbrDescriptorWriter* p_writer, LbrDevice* p_device, VkDescriptorSet set) {
  for (u32 i = 0; i < p_writer->descriptor_write_count; i++) {
    p_writer->descriptor_writes[i].dstSet = set;
  }

  vkUpdateDescriptorSets(p_device->device, p_writer->descriptor_write_count, p_writer->descriptor_writes, 0, NULL);
}
