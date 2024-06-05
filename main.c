#include <stdio.h>
#include <time.h>
#include <vulkan/vulkan_core.h>
#include <windef.h>
#include <windows.h>
#include <winnt.h>

#include "fabric/fabric.h"
#include "fabric/synchronize.h"
#include "renderer/allocator.h"
#include "renderer/core.h"
#include "renderer/frame.h"
#include "renderer/image.h"
#include "renderer/mesh.h"
#include "renderer/shader.h"
#include "renderer/swapchain.h"
#include "renderer/window.h"
#include "utils/types.h"

void foo() { printf("foo\n"); }

void baz(const usize* i) {
  LbrTask new_tasks[10];
  for (usize i = 0; i < 10; i++) {
    new_tasks[i] = LBR_TASK(foo, NULL);
  }

  LbrSemaphore* p_sem = NULL;
  lbrFabricQueueTasks(new_tasks, 10, &p_sem);
  lbrFabricAwaitSemaphore(p_sem, 0);
  lbrFabricFreeSemaphore(&p_sem);
  printf("heee %zu\n", *i);
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {  // NOLINT
  (void)hInstance;
  (void)hPrevInstance;
  (void)lpCmdLine;
  (void)nShowCmd;

  // lbrInitializeFabric();
  // LbrTask tasks[10];
  // usize in[10];
  // for (usize i = 0; i < 10; i++) {
  //   in[i]    = i;
  //   tasks[i] = LBR_TASK(baz, &in[i]);
  // }

  // LbrSemaphore* semm;
  // lbrFabricQueueTasks(tasks, 10, &semm);

  LbrWindowCreateInfo window_info = {0};
  window_info.height              = 720;
  window_info.width               = 1280;
  window_info.resizeable          = false;
  window_info.show_console        = true;
  window_info.name                = "Labyrinth";

  u32 valid_device_count;
  LbrVulkanCore core     = lbrCreateVulkanCore(&window_info);
  LbrGPU* valid_gpus     = lbrEnumerateGPUs(&core, &valid_device_count);
  LbrDevice device       = lbrCreateDevice(&core, &valid_gpus[0], 1, 1);
  LbrGPUAllocator alloc  = lbrCreateGPUAllocator(&core, &device);
  LbrSwapchain swapchain = lbrCreateSwapchain(&core, &valid_gpus[0], &device, &alloc);

  LbrShader shader = lbrCreateShader(&device, "1", VK_SHADER_STAGE_COMPUTE_BIT);
  LbrShader* shaders[1];
  shaders[0]             = &shader;
  LbrShaderEffect effect = lbrCreateShaderEffect(&device, shaders, 1);
  LbrShaderPass pass     = lbrCreateShaderPass(&device, &effect, LBR_COMPUTE_PIPELINE, NULL);

  LbrFrameData frame_data = lbrCreateFrameData(&device);

  LbrShader* tri_shaders[2];
  LbrShader vert = lbrCreateShader(&device, "tri", VK_SHADER_STAGE_VERTEX_BIT);
  LbrShader frag = lbrCreateShader(&device, "tri", VK_SHADER_STAGE_FRAGMENT_BIT);

  LbrShader* mesh_shaders[2];
  LbrShader mesh  = lbrCreateShader(&device, "mesh", VK_SHADER_STAGE_VERTEX_BIT);
  mesh_shaders[0] = &mesh;
  mesh_shaders[1] = &frag;

  tri_shaders[0]              = &vert;
  tri_shaders[1]              = &frag;
  LbrShaderEffect tri_effect  = lbrCreateShaderEffect(&device, tri_shaders, 2);
  LbrShaderEffect mesh_effect = lbrCreateShaderEffect(&device, mesh_shaders, 2);

  LbrGraphicsConfig tri_config;
  tri_config.topology           = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  tri_config.polygon_mode       = VK_POLYGON_MODE_FILL;
  tri_config.cull_mode          = VK_CULL_MODE_NONE;
  tri_config.color_format       = swapchain.draw_image.format;
  tri_config.depth_format       = VK_FORMAT_UNDEFINED;
  tri_config.disable_blending   = true;
  tri_config.disable_depth_test = true;
  tri_config.disable_msaa       = true;
  LbrShaderPass tri_pass        = lbrCreateShaderPass(&device, &tri_effect, LBR_GRAPHICS_PIPELINE, &tri_config);
  LbrShaderPass mesh_pass       = lbrCreateShaderPass(&device, &mesh_effect, LBR_GRAPHICS_PIPELINE, &tri_config);
  (void)mesh_pass;

  u32 frame_count = 0;
  f32 delta_time  = 1;

  LARGE_INTEGER frequency;
  QueryPerformanceFrequency(&frequency);

  LbrVertex rect_vertices[4];
  u32 rect_indices[6];

  rect_vertices[0].position[0] = 0.5F;
  rect_vertices[0].position[1] = -0.5F;
  rect_vertices[0].position[2] = 0.0F;

  rect_vertices[1].position[0] = 0.5F;
  rect_vertices[1].position[1] = 0.5F;
  rect_vertices[1].position[2] = 0.0F;

  rect_vertices[2].position[0] = -0.5F;
  rect_vertices[2].position[1] = -0.5F;
  rect_vertices[2].position[2] = 0.0F;

  rect_vertices[3].position[0] = -0.5F;
  rect_vertices[3].position[1] = 0.5F;
  rect_vertices[3].position[2] = 0.0F;

  rect_vertices[0].color[0] = 0.5F;
  rect_vertices[0].color[1] = 1.0F;
  rect_vertices[0].color[2] = 0.5F;
  rect_vertices[0].color[3] = 1.0F;

  rect_vertices[1].color[0] = 0.25F;
  rect_vertices[1].color[1] = 0.5F;
  rect_vertices[1].color[2] = 0.25F;
  rect_vertices[1].color[3] = 1.0F;

  rect_vertices[2].color[0] = 0.25F;
  rect_vertices[2].color[1] = 0.5F;
  rect_vertices[2].color[2] = 0.25F;
  rect_vertices[2].color[3] = 1.0F;

  rect_vertices[3].color[0] = 0.0F;
  rect_vertices[3].color[1] = 1.0F;
  rect_vertices[3].color[2] = 0.0F;
  rect_vertices[3].color[3] = 1.0F;

  rect_indices[0] = 0;
  rect_indices[1] = 1;
  rect_indices[2] = 2;
  rect_indices[3] = 2;
  rect_indices[4] = 1;
  rect_indices[5] = 3;

  LbrMeshBuffer mesh_buffer = lbrCreateMeshBuffer(lbrFrameDataGetBuffer(&frame_data, LBR_GRAPHICS_COMMAND), &device, &alloc, rect_indices, 6, rect_vertices, 4);

  struct {
    f32 one[4];
    f32 two[4];
    f32 three[4];
    f32 four[4];
  } push;

  struct {
    f32 mat4[16];
    VkDeviceAddress vertex_buffer;
  } mesh_constants;

  mesh_constants.vertex_buffer = mesh_buffer.vertex_address;

  for (;;) {
    LARGE_INTEGER start;
    QueryPerformanceCounter(&start);
    lbrWindowPollEvents(&core.window);

    vkWaitForFences(device.device, 1, &frame_data.render_fence, true, (u64)1e9F);

    lbrResetFrameData(&frame_data, &device);

    u32 swapchain_image_index;
    vkAcquireNextImageKHR(device.device, swapchain.swapchain, (u64)1e9F, frame_data.swapchain_semaphore, VK_NULL_HANDLE, &swapchain_image_index);
    LbrImage current_image;
    current_image.extent = swapchain.draw_image.extent;
    current_image.image  = swapchain.frame_images[swapchain_image_index];

    VkCommandBuffer command_buffer                     = lbrFrameDataGetBuffer(&frame_data, LBR_GRAPHICS_COMMAND);
    VkCommandBufferBeginInfo command_buffer_begin_info = {0};
    command_buffer_begin_info.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    command_buffer_begin_info.flags                    = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info);

    lbrImageTransition(command_buffer, &swapchain.draw_image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, pass.pipeline);
    vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, pass.p_effect->layout, 0, 1, &swapchain.draw_image_descriptors, 0, NULL);

    push.one[0] = (f32)frame_count;
    push.one[1] = delta_time;
    push.one[2] = 1.0F;
    push.one[3] = 1.0F;

    push.two[0] = 0;
    push.two[1] = 1.0F;
    push.two[2] = 0;
    push.two[3] = 1.0F;

    vkCmdPushConstants(command_buffer, pass.p_effect->layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(push), &push);
    vkCmdDispatch(command_buffer, window_info.width / 16, window_info.height / 16, 1);

    lbrImageTransition(command_buffer, &swapchain.draw_image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    VkRenderingAttachmentInfo color_attachment = {0};
    color_attachment.sType                     = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    color_attachment.imageView                 = swapchain.draw_image.image_view;
    color_attachment.imageLayout               = VK_IMAGE_LAYOUT_GENERAL;
    color_attachment.loadOp                    = VK_ATTACHMENT_LOAD_OP_LOAD;
    color_attachment.storeOp                   = VK_ATTACHMENT_STORE_OP_STORE;

    VkRenderingInfo render_info      = {0};
    render_info.sType                = VK_STRUCTURE_TYPE_RENDERING_INFO;
    render_info.renderArea           = (VkRect2D){(VkOffset2D){0}, swapchain.extent};
    render_info.layerCount           = 1;
    render_info.colorAttachmentCount = 1;
    render_info.pColorAttachments    = &color_attachment;

    vkCmdBeginRendering(command_buffer, &render_info);

    VkViewport viewport = {0};
    viewport.x          = 0;
    viewport.y          = 0;
    viewport.width      = (f32)swapchain.extent.width;
    viewport.height     = (f32)swapchain.extent.height;
    viewport.minDepth   = 0.0F;
    viewport.maxDepth   = 1.0F;

    vkCmdSetViewport(command_buffer, 0, 1, &viewport);

    VkRect2D scissor = {0};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent   = swapchain.extent;

    vkCmdSetScissor(command_buffer, 0, 1, &scissor);

    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mesh_pass.pipeline);
    vkCmdPushConstants(command_buffer, mesh_pass.p_effect->layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mesh_constants), &mesh_constants);
    vkCmdBindIndexBuffer(command_buffer, mesh_buffer.index_buffer.buffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(command_buffer, 6, 1, 0, 0, 0);

    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, tri_pass.pipeline);
    vkCmdPushConstants(command_buffer, tri_pass.p_effect->layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(push), &push);
    vkCmdDraw(command_buffer, 3, 1, 0, 0);

    vkCmdEndRendering(command_buffer);

    lbrImageTransition(command_buffer, &swapchain.draw_image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    lbrImageTransition(command_buffer, &current_image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    lbrImageToImageCopy(command_buffer, &swapchain.draw_image, &current_image);
    lbrImageTransition(command_buffer, &current_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    vkEndCommandBuffer(command_buffer);

    VkCommandBufferSubmitInfo command_buffer_submit_info = {0};
    command_buffer_submit_info.sType                     = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    command_buffer_submit_info.commandBuffer             = command_buffer;
    command_buffer_submit_info.deviceMask                = 0;

    VkSemaphoreSubmitInfo semaphore_submit_info_1 = {0};
    semaphore_submit_info_1.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    semaphore_submit_info_1.semaphore             = frame_data.swapchain_semaphore;
    semaphore_submit_info_1.stageMask             = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
    semaphore_submit_info_1.deviceIndex           = 0;
    semaphore_submit_info_1.value                 = 1;

    VkSemaphoreSubmitInfo semaphore_submit_info_2 = {0};
    semaphore_submit_info_2.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    semaphore_submit_info_2.semaphore             = frame_data.render_semaphore;
    semaphore_submit_info_2.stageMask             = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT;
    semaphore_submit_info_2.deviceIndex           = 0;
    semaphore_submit_info_2.value                 = 1;

    VkSubmitInfo2 submit_info            = {0};
    submit_info.sType                    = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    submit_info.waitSemaphoreInfoCount   = 1;
    submit_info.pWaitSemaphoreInfos      = &semaphore_submit_info_1;
    submit_info.signalSemaphoreInfoCount = 1;
    submit_info.pSignalSemaphoreInfos    = &semaphore_submit_info_2;
    submit_info.commandBufferInfoCount   = 1;
    submit_info.pCommandBufferInfos      = &command_buffer_submit_info;

    vkQueueSubmit2(device.graphics_queue.queue, 1, &submit_info, frame_data.render_fence);

    VkPresentInfoKHR present_info   = {0};
    present_info.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.pSwapchains        = &swapchain.swapchain;
    present_info.swapchainCount     = 1;
    present_info.pWaitSemaphores    = &frame_data.render_semaphore;
    present_info.waitSemaphoreCount = 1;
    present_info.pImageIndices      = &swapchain_image_index;

    vkQueuePresentKHR(device.present_queue.queue, &present_info);

    frame_count++;
    LARGE_INTEGER end;
    QueryPerformanceCounter(&end);
    u64 cpu_time_used = (end.QuadPart - start.QuadPart);

    // printf("fps: %f\n", 1 / ((float)cpu_time_used / ((float)frequency.QuadPart)));
    delta_time = ((float)cpu_time_used / ((float)frequency.QuadPart));
  }

  return 0;
}
