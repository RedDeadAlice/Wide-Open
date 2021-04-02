#include<Renderpass.h>
using namespace WideOpenBP;
Renderpass::Renderpass(){}
Renderpass& Renderpass::instance(){
    static Renderpass renderpass;
    return renderpass;
}
void Renderpass::init(){
    setupAttachments();
    setupSubpass();
    VkRenderPassCreateInfo createInfo{};
    createInfo.sType=VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.attachmentCount=2;
    createInfo.pAttachments=attachments;
    createInfo.subpassCount=1;
    createInfo.pSubpasses=&subpass;
    createInfo.dependencyCount=1;
    createInfo.pDependencies=&dependency;
    if(vkCreateRenderPass(DEVICE,&createInfo,ALLOCATOR,&renderpass)!=VK_SUCCESS){
        LOG.error("Failed to create renderpass");
    }
    createFramebufferImages();
    createFramebuffer();
}
void Renderpass::setupAttachments(){
    attachments[0]=VkAttachmentDescription{};
    attachments[0].loadOp=VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[0].storeOp=VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].samples=VK_SAMPLE_COUNT_1_BIT;
    attachments[0].format=VK_FORMAT_R8G8B8A8_UNORM; //MAY CAUSE WEIRD COLORING OUTPUT
    attachments[0].initialLayout=VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[0].finalLayout=VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachments[1]=VkAttachmentDescription{};
    attachments[1].loadOp=VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[1].storeOp=VK_ATTACHMENT_STORE_OP_STORE;
    attachments[1].stencilLoadOp=VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[1].stencilStoreOp=VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[1].samples=VK_SAMPLE_COUNT_1_BIT;
    attachments[1].format=VK_FORMAT_D16_UNORM;
    attachments[1].initialLayout=VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[1].finalLayout=VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    attachmentsRefs[0]=VkAttachmentReference{};
    attachmentsRefs[0].attachment=0;
    attachmentsRefs[0].layout=VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachmentsRefs[1]=VkAttachmentReference{};
    attachmentsRefs[1].attachment=1;
    attachmentsRefs[1].layout=VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
}
void Renderpass::setupSubpass(){
    subpass=VkSubpassDescription{};
    subpass.colorAttachmentCount=1;
    subpass.pColorAttachments=&attachmentsRefs[0];
    subpass.pDepthStencilAttachment=&attachmentsRefs[1];
    subpass.pipelineBindPoint=VK_PIPELINE_BIND_POINT_GRAPHICS;
    dependency=VkSubpassDependency{};
    dependency.srcSubpass=VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass=0;
    dependency.srcStageMask=VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
    dependency.dstStageMask=VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependency.srcAccessMask=VK_ACCESS_INDEX_READ_BIT;
    dependency.dstAccessMask=VK_ACCESS_MEMORY_WRITE_BIT;
}
void Renderpass::createFramebufferImages(){
    VkImageCreateInfo createInfo{};
    createInfo.sType=VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.arrayLayers=1;
    createInfo.extent.depth=1;
    createInfo.extent.height=RENDERER.getExtent().height;
    createInfo.extent.width=RENDERER.getExtent().width;
    createInfo.format=VK_FORMAT_R8G8B8A8_UNORM;
    createInfo.imageType=VK_IMAGE_TYPE_2D;
    createInfo.initialLayout=VK_IMAGE_LAYOUT_UNDEFINED;
    createInfo.mipLevels=1;
    uint32_t queue=RENDERER.getGraphicsQueueIndex();
    createInfo.pQueueFamilyIndices=&queue;
    createInfo.queueFamilyIndexCount=1;
    createInfo.samples=VK_SAMPLE_COUNT_1_BIT;
    createInfo.sharingMode=VK_SHARING_MODE_EXCLUSIVE;
    createInfo.tiling=VK_IMAGE_TILING_OPTIMAL;
    createInfo.usage=VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if(vkCreateImage(DEVICE,&createInfo,ALLOCATOR,&framebufferImages[0])!=VK_SUCCESS){
        LOG.error("Failed to create color img");
    }
    VkMemoryRequirements memReq;
    vkGetImageMemoryRequirements(DEVICE,framebufferImages[0],&memReq);
    fbImagesMem[0]=RENDERER.allocateMemory(memReq,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    if(vkBindImageMemory(DEVICE,framebufferImages[0],fbImagesMem[0],0)!=VK_SUCCESS){
        LOG.error("Failed to bind memory with color img");
    }
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType=VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.components.a=VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.b=VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.g=VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.r=VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.format=VK_FORMAT_R8G8B8A8_UNORM;
    viewInfo.image=framebufferImages[0];
    viewInfo.subresourceRange.aspectMask=VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseArrayLayer=0;
    viewInfo.subresourceRange.baseMipLevel=0;
    viewInfo.subresourceRange.layerCount=1;
    viewInfo.subresourceRange.levelCount=1;
    viewInfo.viewType=VK_IMAGE_VIEW_TYPE_2D;
    if(vkCreateImageView(DEVICE,&viewInfo,ALLOCATOR,&framebufferImageViews[0])!=VK_SUCCESS){
        LOG.error("Failed to create color img view");
    }
    createInfo=VkImageCreateInfo{};
    createInfo.arrayLayers=1;
    createInfo.extent.depth=1;
    createInfo.extent.height=RENDERER.getExtent().height;
    createInfo.extent.width=RENDERER.getExtent().width;
    createInfo.format=VK_FORMAT_D16_UNORM;
    createInfo.imageType=VK_IMAGE_TYPE_2D;
    createInfo.initialLayout=VK_IMAGE_LAYOUT_UNDEFINED;
    createInfo.mipLevels=1;
    createInfo.pQueueFamilyIndices=&queue;
    createInfo.queueFamilyIndexCount=1;
    createInfo.samples=VK_SAMPLE_COUNT_1_BIT;
    createInfo.sharingMode=VK_SHARING_MODE_EXCLUSIVE;
    createInfo.sType=VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.tiling=VK_IMAGE_TILING_OPTIMAL;
    createInfo.usage=VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    if(vkCreateImage(DEVICE,&createInfo,ALLOCATOR,&framebufferImages[1])!=VK_SUCCESS){
        LOG.error("Failed to create depth img");
    }
    vkGetImageMemoryRequirements(DEVICE,framebufferImages[1],&memReq);
    fbImagesMem[1]=RENDERER.allocateMemory(memReq,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    if(vkBindImageMemory(DEVICE,framebufferImages[1],fbImagesMem[1],0)!=VK_SUCCESS){
        LOG.error("Failed to bind memory to depth img");
    }
    viewInfo=VkImageViewCreateInfo{};
    viewInfo.sType=VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.components.a=VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.b=VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.g=VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.r=VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.format=VK_FORMAT_D16_UNORM;
    viewInfo.image=framebufferImages[1];
    viewInfo.subresourceRange.aspectMask=VK_IMAGE_ASPECT_DEPTH_BIT;
    viewInfo.subresourceRange.baseArrayLayer=0;
    viewInfo.subresourceRange.baseMipLevel=0;
    viewInfo.subresourceRange.layerCount=1;
    viewInfo.subresourceRange.levelCount=1;
    viewInfo.viewType=VK_IMAGE_VIEW_TYPE_2D;
    if(vkCreateImageView(DEVICE,&viewInfo,ALLOCATOR,&framebufferImageViews[1])!=VK_SUCCESS){
        LOG.error("Failed to create depth img view");
    }
}
void Renderpass::createFramebuffer(){
    VkFramebufferCreateInfo createInfo{};
    createInfo.sType=VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    createInfo.renderPass=renderpass;
    createInfo.attachmentCount=2;
    createInfo.pAttachments=framebufferImageViews;
    createInfo.layers=1;
    createInfo.height=WINDOW.getHeight();
    createInfo.width=WINDOW.getWidth();
    if(vkCreateFramebuffer(DEVICE,&createInfo,ALLOCATOR,&framebuffer)!=VK_SUCCESS){
        LOG.error("Failed to create framebuffer");
    }
}
void Renderpass::terminate(){
    vkDestroyRenderPass(DEVICE,renderpass,ALLOCATOR);
    vkDestroyFramebuffer(DEVICE,framebuffer,ALLOCATOR);
    for(int i=0;i<2;i++){
        vkDestroyImageView(DEVICE,framebufferImageViews[i],ALLOCATOR);
        vkDestroyImage(DEVICE,framebufferImages[i],ALLOCATOR);
        vkFreeMemory(DEVICE,fbImagesMem[i],ALLOCATOR);
    }
}