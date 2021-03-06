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
#include<RenderPassBPWSLight.h>
using namespace WideOpenBPWS;
RenderPassBPWSLight::RenderPassBPWSLight(){}
RenderPassBPWSLight& RenderPassBPWSLight::instance(){
    static RenderPassBPWSLight renderpass;
    return renderpass;
}
void RenderPassBPWSLight::setupAttachments(){
    attachmentsCount=1;
    attachmentRefsCount=1;
    attachments=new VkAttachmentDescription[attachmentsCount];
    attachmentsRefs=new VkAttachmentReference[attachmentRefsCount];
    attachments[0].finalLayout=VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL;
    attachments[0].format=VK_FORMAT_D16_UNORM;
    attachments[0].initialLayout=VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[0].loadOp=VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[0].samples=VK_SAMPLE_COUNT_1_BIT;
    attachments[0].stencilLoadOp=VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp=VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].storeOp=VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].flags=0;
    attachmentsRefs[0].attachment=0;
    attachmentsRefs[0].layout=VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
}
void RenderPassBPWSLight::setupSubPasses(){
    subPassesCount=1;
    dependenciesCount=1;
    subPasses=new VkSubpassDescription[subPassesCount];
    depenedncies=new VkSubpassDependency[dependenciesCount];
    subPasses[0].colorAttachmentCount=0;
    subPasses[0].inputAttachmentCount=0;
    subPasses[0].flags=0;
    subPasses[0].pDepthStencilAttachment=attachmentsRefs;
    subPasses[0].pipelineBindPoint=VK_PIPELINE_BIND_POINT_GRAPHICS;
    subPasses[0].preserveAttachmentCount=0;
    subPasses[0].pResolveAttachments=nullptr;
    depenedncies[0].srcSubpass=0;
    depenedncies[0].dstSubpass=VK_SUBPASS_EXTERNAL;
    depenedncies[0].srcStageMask=VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    depenedncies[0].dstStageMask=VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    depenedncies[0].srcAccessMask=VK_ACCESS_MEMORY_WRITE_BIT;
    depenedncies[0].dstAccessMask=VK_ACCESS_MEMORY_READ_BIT;
}
void RenderPassBPWSLight::setupFramebuffers(){
    framebuffersCount=1;
    framebuffers=new Framebuffer[1];
    framebuffers[0].imagesCount=1;
    framebuffers[0].images=new VkImage[1];
    framebuffers[0].imagesMemory=new VkDeviceMemory[1];
    framebuffers[0].imageViews=new VkImageView[1];
    framebuffers[0].swapchainImage=nullptr;
    uint32_t graphicsQueueFamilyIndex=renderer->getGraphicsQueueFamilyIndex();
    VkImageCreateInfo imageCreateInfo{};
    imageCreateInfo.arrayLayers=1;
    imageCreateInfo.extent={1024,1024,1};
    imageCreateInfo.flags=0;
    imageCreateInfo.format=VK_FORMAT_D16_UNORM;
    imageCreateInfo.imageType=VK_IMAGE_TYPE_2D;
    imageCreateInfo.initialLayout=VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.mipLevels=1;
    imageCreateInfo.pNext=nullptr;
    imageCreateInfo.queueFamilyIndexCount=1;
    imageCreateInfo.pQueueFamilyIndices=&graphicsQueueFamilyIndex;
    imageCreateInfo.samples=VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.sharingMode=VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.sType=VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.tiling=VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.usage=VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT|VK_IMAGE_USAGE_SAMPLED_BIT;
    if(vkCreateImage(renderer->getDevice(),&imageCreateInfo,ALLOCATOR,&framebuffers[0].images[0])!=VK_SUCCESS){
        LOG.error("Failed to create depth image in light renderpass");
    }
    VkMemoryRequirements memReq;
    vkGetImageMemoryRequirements(renderer->getDevice(),framebuffers[0].images[0],&memReq);
    framebuffers[0].imagesMemory[0]=renderer->allocateMemory(memReq,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vkBindImageMemory(renderer->getDevice(),framebuffers[0].images[0],framebuffers[0].imagesMemory[0],0);
    VkImageViewCreateInfo imageViewCreateInfo{};
    imageViewCreateInfo.components.a=VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.b=VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.g=VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.r=VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.format=VK_FORMAT_D16_UNORM;
    imageViewCreateInfo.image=framebuffers[0].images[0];
    imageViewCreateInfo.sType=VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.subresourceRange.aspectMask=VK_IMAGE_ASPECT_DEPTH_BIT;
    imageViewCreateInfo.subresourceRange.baseArrayLayer=0;
    imageViewCreateInfo.subresourceRange.baseMipLevel=0;
    imageViewCreateInfo.subresourceRange.layerCount=1;
    imageViewCreateInfo.subresourceRange.levelCount=1;
    imageViewCreateInfo.viewType=VK_IMAGE_VIEW_TYPE_2D;
    if(vkCreateImageView(renderer->getDevice(),&imageViewCreateInfo,ALLOCATOR,&framebuffers[0].imageViews[0])!=VK_SUCCESS){
        LOG.error("Failed to create image view for first framebuffer");
    }
    VkFramebufferCreateInfo framebufferCreateInfo{};
    framebufferCreateInfo.attachmentCount=1;
    framebufferCreateInfo.flags=0;
    framebufferCreateInfo.height=1024;
    framebufferCreateInfo.layers=1;
    framebufferCreateInfo.pAttachments=&framebuffers[0].imageViews[0];
    framebufferCreateInfo.renderPass=renderPass;
    framebufferCreateInfo.sType=VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.width=1024;
    if(vkCreateFramebuffer(renderer->getDevice(),&framebufferCreateInfo,ALLOCATOR,&framebuffers[0].framebuffer)!=VK_SUCCESS){
        LOG.error("Failed to create depth framebuffer");
    }
}
VkImageView RenderPassBPWSLight::getDepthImageView(){
    return framebuffers[0].imageViews[0];
}
void RenderPassBPWSLight::debugRecord(VkCommandBuffer mainCmdBuffer,MeshBPWS mesh[2]){
    vkCmdBindPipeline(mainCmdBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,LightPipeline::instance().getPipeline());
    VkDescriptorSet set=mesh[0].getDescriptorSet();
    vkCmdBindDescriptorSets(mainCmdBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,LightPipeline::instance().getLayout(),0,1,&set,0,nullptr);
    VkDeviceSize offsets=0;
    vkCmdBindVertexBuffers(mainCmdBuffer,0,1,mesh[0].getVertexBuffer(),&offsets);
    vkCmdBindIndexBuffer(mainCmdBuffer,mesh[0].getIndexBuffer(),0,VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(mainCmdBuffer,mesh[0].getIndicesCount(),1,0,0,0);
    set=mesh[1].getDescriptorSet();
    vkCmdBindDescriptorSets(mainCmdBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,LightPipeline::instance().getLayout(),0,1,&set,0,nullptr);
    vkCmdBindVertexBuffers(mainCmdBuffer,0,1,mesh[1].getVertexBuffer(),&offsets);
    vkCmdBindIndexBuffer(mainCmdBuffer,mesh[1].getIndexBuffer(),0,VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(mainCmdBuffer,mesh[1].getIndicesCount(),1,0,0,0);
}