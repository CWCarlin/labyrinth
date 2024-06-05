#include "renderer/mesh.h"

#include <string.h>

#include "renderer/allocator.h"
#include "vulkan/vulkan_core.h"

LbrMeshBuffer lbrCreateMeshBuffer(VkCommandBuffer buffer, LbrDevice* p_device, LbrGPUAllocator* p_allocator, u32* indices, u32 index_count, LbrVertex* vertices, u32 vertex_count) {
  LbrMeshBuffer mesh = {0};

  u32 vertex_buffer_size = vertex_count * sizeof(LbrVertex);
  u32 index_buffer_size  = index_count * sizeof(u32);

  mesh.vertex_buffer =
      lbrCreateAllocatedBuffer(p_allocator, vertex_buffer_size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);

  VkBufferDeviceAddressInfo address_info = {0};
  address_info.sType                     = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
  address_info.buffer                    = mesh.vertex_buffer.buffer;
  mesh.vertex_address                    = vkGetBufferDeviceAddress(p_device->device, &address_info);

  mesh.index_buffer = lbrCreateAllocatedBuffer(p_allocator, index_buffer_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);

  LbrAllocatedBuffer stage = lbrCreateAllocatedBuffer(p_allocator, vertex_buffer_size + index_buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
  void* data               = stage.info.pMappedData;
  memcpy(data, vertices, vertex_buffer_size);
  memcpy((char*)data + vertex_buffer_size, indices, index_buffer_size);

  VkCommandBufferBeginInfo command_buffer_begin_info = {0};
  command_buffer_begin_info.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  command_buffer_begin_info.flags                    = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  vkBeginCommandBuffer(buffer, &command_buffer_begin_info);

  VkBufferCopy vertex_copy = {0};
  vertex_copy.dstOffset    = 0;
  vertex_copy.srcOffset    = 0;
  vertex_copy.size         = vertex_buffer_size;

  vkCmdCopyBuffer(buffer, stage.buffer, mesh.vertex_buffer.buffer, 1, &vertex_copy);

  VkBufferCopy index_copy = {0};
  index_copy.dstOffset    = 0;
  index_copy.srcOffset    = vertex_buffer_size;
  index_copy.size         = index_buffer_size;

  vkCmdCopyBuffer(buffer, stage.buffer, mesh.index_buffer.buffer, 1, &index_copy);

  vkEndCommandBuffer(buffer);

  VkFence fence;
  VkFenceCreateInfo fence_info = {0};
  fence_info.sType             = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  vkCreateFence(p_device->device, &fence_info, NULL, &fence);

  VkCommandBufferSubmitInfo cmdinfo = {0};
  cmdinfo.sType                     = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
  cmdinfo.commandBuffer             = buffer;

  VkSubmitInfo2 submit          = {0};
  submit.sType                  = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
  submit.pCommandBufferInfos    = &cmdinfo;
  submit.commandBufferInfoCount = 1;

  vkQueueSubmit2(p_device->graphics_queue.queue, 1, &submit, fence);
  vkWaitForFences(p_device->device, 1, &fence, true, (u64)1e9F);

  lbrDestroyAllocatedBuffer(&stage, p_allocator);

  return mesh;
}
