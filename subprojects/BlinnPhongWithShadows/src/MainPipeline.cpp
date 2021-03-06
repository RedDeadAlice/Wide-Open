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
#include<MainPipeline.h>
using namespace WideOpenBPWS;
using namespace glm;
MainPipeline::MainPipeline(){}
MainPipeline& MainPipeline::instance(){
    static MainPipeline pipeline;
    return pipeline;
}
void MainPipeline::createShaderModules(){
    shaderModulesCount=2;
    shaderModules=new VkShaderModule[2];
    shaderModules[0]=createShaderModule("./Assets/Wide-OpenBPWS/mainShaderVS.spv");
    shaderModules[1]=createShaderModule("./Assets/Wide-OpenBPWS/mainShaderFS.spv");
}
void MainPipeline::createLayout(){
    VkDescriptorSetLayout descriptorSetLayout=dsl->getDSL();
    VkPipelineLayoutCreateInfo createInfo{};
    createInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    createInfo.setLayoutCount=1;
    createInfo.pSetLayouts=&descriptorSetLayout;
    if(vkCreatePipelineLayout(renderer->getDevice(),&createInfo,ALLOCATOR,&layout)!=VK_SUCCESS){
        LOG.error("Failed to create main pipeline layout");
    }
}
void MainPipeline::createShaderStages(){
    shaderStagesCount=2;
    shaderStageCreateInfo=new VkPipelineShaderStageCreateInfo[2];
    shaderStageCreateInfo[0]=VkPipelineShaderStageCreateInfo{};
    shaderStageCreateInfo[0].sType=VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfo[0].stage=VK_SHADER_STAGE_VERTEX_BIT;
    shaderStageCreateInfo[0].module=shaderModules[0];
    shaderStageCreateInfo[0].pName="main";
    shaderStageCreateInfo[1]=VkPipelineShaderStageCreateInfo{};
    shaderStageCreateInfo[1].sType=VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfo[1].stage=VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStageCreateInfo[1].module=shaderModules[1];
    shaderStageCreateInfo[1].pName="main";
}
void MainPipeline::createVertexInputState(){
    attribDescs.resize(2);
    bindingDesc.binding=0;
    bindingDesc.stride=sizeof(vec3)*2;
    bindingDesc.inputRate=VK_VERTEX_INPUT_RATE_VERTEX;
    attribDescs[0].location=0;
    attribDescs[0].binding=0;
    attribDescs[0].offset=0;
    attribDescs[0].format=VK_FORMAT_R32G32B32_SFLOAT;
    attribDescs[1].location=1;
    attribDescs[1].binding=0;
    attribDescs[1].offset=sizeof(vec3);
    attribDescs[1].format=VK_FORMAT_R32G32B32_SFLOAT;
    vertexInputStateCreateInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputStateCreateInfo.vertexAttributeDescriptionCount=attribDescs.size();
    vertexInputStateCreateInfo.vertexBindingDescriptionCount=1;
    vertexInputStateCreateInfo.pVertexAttributeDescriptions=attribDescs.data();
    vertexInputStateCreateInfo.pVertexBindingDescriptions=&bindingDesc;
}
void MainPipeline::createInputAssemblyState(){
    inputAssemblyStateCreateInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyStateCreateInfo.topology=VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyStateCreateInfo.primitiveRestartEnable=VK_FALSE;
}
void MainPipeline::createViewportState(){
    viewport.x=0;
    viewport.y=0;
    viewport.width=640;
    viewport.height=480;
    viewport.minDepth=0.0f;
    viewport.maxDepth=1.0f;
    scissors.extent={640,480};
    scissors.offset={0,0};
    viewportStateCreateInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.viewportCount=1;
    viewportStateCreateInfo.scissorCount=1;
    viewportStateCreateInfo.pScissors=&scissors;
    viewportStateCreateInfo.pViewports=&viewport;
}
void MainPipeline::createRasterState(){
    rasterStateCreateInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterStateCreateInfo.depthBiasEnable=VK_FALSE;
    rasterStateCreateInfo.rasterizerDiscardEnable=VK_FALSE;
    rasterStateCreateInfo.polygonMode=VK_POLYGON_MODE_FILL;
    rasterStateCreateInfo.lineWidth=1.0f;
    rasterStateCreateInfo.cullMode=VK_CULL_MODE_BACK_BIT;
    rasterStateCreateInfo.frontFace=VK_FRONT_FACE_COUNTER_CLOCKWISE;
}
void MainPipeline::createMultisampleState(){
    multisampleStateCreateInfo.rasterizationSamples=VK_SAMPLE_COUNT_1_BIT;
    multisampleStateCreateInfo.sampleShadingEnable=VK_FALSE;
    multisampleStateCreateInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
}
void MainPipeline::createDepthStencilState(){
    depthStencilStateCreateInfo.depthBoundsTestEnable=VK_FALSE;
    depthStencilStateCreateInfo.depthCompareOp=VK_COMPARE_OP_LESS_OR_EQUAL;
    depthStencilStateCreateInfo.depthTestEnable=VK_TRUE;
    depthStencilStateCreateInfo.depthWriteEnable=VK_TRUE;
    depthStencilStateCreateInfo.stencilTestEnable=VK_FALSE;
    depthStencilStateCreateInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
}
void MainPipeline::createDynamicState(){
    dynamicStateCreateInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCreateInfo.dynamicStateCount=0;
}
void MainPipeline::createColorBlendState(){
    attachmentState.colorWriteMask=VK_COLOR_COMPONENT_R_BIT|VK_COLOR_COMPONENT_G_BIT|VK_COLOR_COMPONENT_B_BIT|VK_COLOR_COMPONENT_A_BIT;
    attachmentState.blendEnable=VK_FALSE;
    colorBlendStateCreateInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendStateCreateInfo.logicOpEnable=VK_FALSE;
    colorBlendStateCreateInfo.attachmentCount=1;
    colorBlendStateCreateInfo.pAttachments=&attachmentState;
}