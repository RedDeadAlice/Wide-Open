#pragma once
#include<Definations.i>
#include<Renderer.h>
#include<Mesh.h>
namespace WideOpenBP{
    class Renderpass{
    public:
        struct Framebuffer{
            VkFramebuffer framebuffer;
            VkImageView ImageView[2];
            VkImage depthImg;
            VkDeviceMemory depthImgMem;
            VkCommandBuffer cmdBuffer;
        };
    private:
        Renderpass();
        VkRenderPass renderpass;
        VkAttachmentDescription attachments[2];
        VkAttachmentReference attachmentsRefs[2];
        VkSubpassDescription subpass;
        VkSubpassDependency dependency;
        Framebuffer* framebuffers;
        uint32_t framebuffersCount;
        void setupAttachments();
        void setupSubpass();
        void createFramebuffer();
        void createFramebufferImages();
        void createCommandBuffer();
    public:
        static Renderpass& instance();
        void init();
        void record();//DEPRACATED
        /*Records commands to command buffers*/
        void record(VkPipeline pipeline,VkPipelineLayout layout,Mesh& mesh);
        VkRenderPass getRenderPass();
        Framebuffer* getFramebuffers();
        void terminate();
    };
}