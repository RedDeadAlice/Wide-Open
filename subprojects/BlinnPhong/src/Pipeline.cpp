#include<Pipeline.h>
using namespace WideOpenBP;
using namespace glm;
using namespace Common;
Pipeline::Pipeline(){}
Pipeline& Pipeline::instance(){
    static Pipeline pipeline;
    return pipeline;
}
void Pipeline::init(){
    createShaderModules();
    createLayout();
    VkPipelineShaderStageCreateInfo stagesCreateInfo[2];
    stagesCreateInfo[0].sType=VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stagesCreateInfo[0].module=modules[0];
    stagesCreateInfo[0].pName="main";
    stagesCreateInfo[0].stage=VK_SHADER_STAGE_VERTEX_BIT;
    stagesCreateInfo[0].flags=0;
    stagesCreateInfo[0].pSpecializationInfo=nullptr;
    stagesCreateInfo[0].pNext=nullptr;
    stagesCreateInfo[1].sType=VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stagesCreateInfo[1].module=modules[1];
    stagesCreateInfo[1].pName="main";
    stagesCreateInfo[1].stage=VK_SHADER_STAGE_FRAGMENT_BIT;
    stagesCreateInfo[1].flags=0;
    stagesCreateInfo[1].pSpecializationInfo=nullptr;
    stagesCreateInfo[1].pNext=nullptr;
    VkVertexInputBindingDescription bindingDesc{};
    bindingDesc.binding=0;
    bindingDesc.inputRate=VK_VERTEX_INPUT_RATE_VERTEX;
    bindingDesc.stride=sizeof(vec3)*2;
    VkVertexInputAttributeDescription attribDescs[2];
    attribDescs[0].binding=0;
    attribDescs[0].format=VK_FORMAT_R32G32B32_SFLOAT;
    attribDescs[0].location=0;
    attribDescs[0].offset=0;
    attribDescs[1].binding=0;
    attribDescs[1].format=VK_FORMAT_R32G32B32_SFLOAT;
    attribDescs[1].location=1;
    attribDescs[1].offset=sizeof(vec3);
    VkPipelineVertexInputStateCreateInfo inputInfo{};
    inputInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    inputInfo.pVertexAttributeDescriptions=attribDescs;
    inputInfo.pVertexBindingDescriptions=&bindingDesc;
    inputInfo.vertexAttributeDescriptionCount=2;
    inputInfo.vertexBindingDescriptionCount=1;
    VkPipelineInputAssemblyStateCreateInfo assembInfo{};
    assembInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    assembInfo.primitiveRestartEnable=VK_FALSE;
    assembInfo.topology=VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    VkViewport viewport{};
    viewport.height=RENDERER.getExtent().height;
    viewport.width=RENDERER.getExtent().width;
    viewport.x=0;
    viewport.y=0;
    viewport.minDepth=0;
    viewport.maxDepth=1;
    VkRect2D scissor;
    scissor.extent=RENDERER.getExtent();
    scissor.offset={0,0};
    VkPipelineViewportStateCreateInfo viewportInfo{};
    viewportInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportInfo.pScissors=&scissor;
    viewportInfo.pViewports=&viewport;
    viewportInfo.scissorCount=1;
    viewportInfo.viewportCount=1;
    VkPipelineRasterizationStateCreateInfo rasterInfo{};
    rasterInfo.cullMode=VK_CULL_MODE_BACK_BIT;
    rasterInfo.depthBiasEnable=VK_FALSE;
    rasterInfo.depthClampEnable=VK_FALSE;
    rasterInfo.frontFace=VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterInfo.lineWidth=1.0f;
    rasterInfo.polygonMode=VK_POLYGON_MODE_FILL;
    rasterInfo.rasterizerDiscardEnable=VK_FALSE;
    rasterInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    VkPipelineMultisampleStateCreateInfo samplesInfo{};
    samplesInfo.rasterizationSamples=VK_SAMPLE_COUNT_1_BIT;
    samplesInfo.sampleShadingEnable=VK_FALSE;
    samplesInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    VkPipelineDepthStencilStateCreateInfo depthInfo{};
    depthInfo.depthBoundsTestEnable=VK_FALSE;
    depthInfo.depthCompareOp=VK_COMPARE_OP_LESS_OR_EQUAL;
    depthInfo.depthTestEnable=VK_TRUE;
    depthInfo.depthWriteEnable=VK_TRUE;
    depthInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    VkPipelineColorBlendAttachmentState attachementState{};
    attachementState.blendEnable=VK_FALSE; //DOUBLE CHECK IF DRAWING UNPROPERLY
    VkPipelineColorBlendStateCreateInfo blendInfo{};
    blendInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    blendInfo.logicOpEnable=VK_FALSE;
    blendInfo.attachmentCount=1;
    blendInfo.pAttachments=&attachementState;
    VkGraphicsPipelineCreateInfo createInfo{};
    createInfo.layout=layout;
    createInfo.pColorBlendState=&blendInfo;
    createInfo.pDepthStencilState=&depthInfo;
    createInfo.pDynamicState=nullptr;
    createInfo.pInputAssemblyState=&assembInfo;
    createInfo.pMultisampleState=&samplesInfo;
    createInfo.pRasterizationState=&rasterInfo;
    createInfo.pStages=stagesCreateInfo;
    createInfo.pVertexInputState=&inputInfo;
    createInfo.pViewportState=&viewportInfo;
    createInfo.renderPass=RENDERPASS.getRenderPass();
    createInfo.stageCount=2;
    createInfo.sType=VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    createInfo.subpass=0;
    if(vkCreateGraphicsPipelines(DEVICE,VK_NULL_HANDLE,1,&createInfo,ALLOCATOR,&pipeline)!=VK_SUCCESS){
        LOG.error("Failed to create pipeline");
    }
}
void Pipeline::createShaderModules(){
    uint32_t vertexShaderSize,fragmentShaderSize;
    char* vertexShaderCode,*fragmentShaderCode;
    vertexShaderCode=readBinFile("./Assets/vertexShader.spv",vertexShaderSize);
    fragmentShaderCode=readBinFile("./Assets/fragmentShader.spv",fragmentShaderSize);
    VkShaderModuleCreateInfo vertexInfo{};
    vertexInfo.sType=VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    vertexInfo.codeSize=vertexShaderSize;
    vertexInfo.pCode=(uint32_t*)vertexShaderCode;
    if(vkCreateShaderModule(DEVICE,&vertexInfo,ALLOCATOR,&modules[0])!=VK_SUCCESS){
        LOG.error("Failed to create vertex shader module");
    }
    VkShaderModuleCreateInfo fragmentInfo{};
    fragmentInfo.sType=VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    fragmentInfo.codeSize=fragmentShaderSize;
    fragmentInfo.pCode=(uint32_t*)fragmentShaderCode;
    if(vkCreateShaderModule(DEVICE,&fragmentInfo,ALLOCATOR,&modules[1])!=VK_SUCCESS){
        LOG.error("Failed to create fragment shader module");
    }
}
void Pipeline::createLayout(){
    VkPipelineLayoutCreateInfo createInfo{};
    createInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    createInfo.setLayoutCount=1;
    VkDescriptorSetLayout layout=LAYOUT.getLayout();
    createInfo.pSetLayouts=&layout;
    if(vkCreatePipelineLayout(DEVICE,&createInfo,ALLOCATOR,&(this->layout))!=VK_SUCCESS){
        LOG.error("Failed to create pipeline layout");
    }
}
void Pipeline::terminate(){
    vkDestroyPipeline(DEVICE,pipeline,ALLOCATOR);
    vkDestroyShaderModule(DEVICE,modules[0],ALLOCATOR);
    vkDestroyShaderModule(DEVICE,modules[1],ALLOCATOR);
    vkDestroyPipelineLayout(DEVICE,layout,ALLOCATOR);
}