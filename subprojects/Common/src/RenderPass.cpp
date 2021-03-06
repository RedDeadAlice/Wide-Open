/*MIT License

Copyright (c) 2021 Mohammad Issawi

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/
#include <RenderPass.h>
using namespace Common;
RenderPass::RenderPass() {}
RenderPass::~RenderPass() {}
void RenderPass::init(Renderer *renderer)
{
    this->renderer = renderer;
    setupAttachments();
    setupSubPasses();
    if (!(attachments && attachmentsRefs && subPasses && depenedncies))
    {
        LOG.error("Attachments or subpasses were not setup properly");
    }
    VkRenderPassCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.attachmentCount = attachmentsCount;
    createInfo.pAttachments = attachments;
    createInfo.subpassCount = subPassesCount;
    createInfo.pSubpasses = subPasses;
    createInfo.dependencyCount = dependenciesCount;
    createInfo.pDependencies = depenedncies;
    if (vkCreateRenderPass(renderer->getDevice(), &createInfo, ALLOCATOR, &renderPass) != VK_SUCCESS)
    {
        LOG.error("Failed to create renderpass");
    }
    LOG.log("Created renderpass successfully");
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.commandBufferCount = 1;
    allocInfo.commandPool = renderer->getCmdPool();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBuffers = new VkCommandBuffer[renderer->getSwapchainImagesCount()];
    for (uint32_t i = 0; i < renderer->getSwapchainImagesCount(); ++i)
    {
        if (vkAllocateCommandBuffers(renderer->getDevice(), &allocInfo, &cmdBuffers[i]) != VK_SUCCESS)
        {
            LOG.error("Failed to allocate command buffer");
        }
    }
    setupFramebuffers();
    if (!framebuffers)
    {
        LOG.error("Framebuffers were not setup properly");
    }
}
VkCommandBuffer *RenderPass::getCmdBuffers()
{
    return cmdBuffers;
}
VkRenderPass RenderPass::getRenderPass()
{
    return renderPass;
}
VkFramebuffer RenderPass::getFrameBuffer(uint32_t index)
{
    return framebuffers[index].framebuffer;
}
void RenderPass::terminate()
{
    vkDestroyRenderPass(renderer->getDevice(), renderPass, ALLOCATOR);
    for (uint32_t i = 0; i < framebuffersCount; ++i)
    {
        vkDestroyFramebuffer(renderer->getDevice(), framebuffers[i].framebuffer, ALLOCATOR);
        for (uint32_t j = 0; j < framebuffers[i].imagesCount; ++j)
        {
            vkDestroyImageView(renderer->getDevice(), framebuffers[i].imageViews[j], ALLOCATOR);
            vkDestroyImage(renderer->getDevice(), framebuffers[i].images[j], ALLOCATOR);
            vkFreeMemory(renderer->getDevice(), framebuffers[i].imagesMemory[j], ALLOCATOR);
        }
    }
    delete[] framebuffers;
    delete[] attachments;
    delete[] attachmentsRefs;
    delete[] subPasses;
    delete[] depenedncies;
    delete[] cmdBuffers;
}