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
#include<MeshBPWS.h>
using namespace WideOpenBPWS;
VkSampler MeshBPWS::sampler=VK_NULL_HANDLE;
MeshBPWS::MeshBPWS(const char* path,UniformBufferObject uniformBufferObject):Mesh(path,uniformBufferObject,&RendererBPWS::instance()){
    vertices=readVertices(scene->mMeshes[0]);
    createVertexBuffer();
    applyUBO();
    VkDescriptorSetLayout secondDSL=MainDSL::instance().getDSL();
    setDescriptorSetsCount(2);
    renderer->allocateDescriptorSet(&descriptorSets[1],&secondDSL);
    renderer->importer.FreeScene();
    if(sampler==VK_NULL_HANDLE){
        VkSamplerCreateInfo samplerCreateInfo{};
        samplerCreateInfo.sType=VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerCreateInfo.magFilter=VK_FILTER_LINEAR;
        samplerCreateInfo.minFilter=VK_FILTER_LINEAR;
        samplerCreateInfo.mipmapMode=VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerCreateInfo.addressModeU=VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        samplerCreateInfo.addressModeV=VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        samplerCreateInfo.addressModeW=VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        samplerCreateInfo.anisotropyEnable=VK_FALSE;
        samplerCreateInfo.compareEnable=VK_FALSE;
        samplerCreateInfo.borderColor=VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;    
        if(vkCreateSampler(renderer->getDevice(),&samplerCreateInfo,ALLOCATOR,&sampler)!=VK_SUCCESS){
            LOG.error("Failed to create sampler");
        }
    }
    LOG.log("Created a mesh successfully");
}
Vertex* MeshBPWS::readVertices(aiMesh* mesh){
    Vertex* vertices = new Vertex[verticesCount];
    for(uint32_t i=0;i<verticesCount;++i){
        vertices[i].pos={mesh->mVertices[i].x,mesh->mVertices[i].y,mesh->mVertices[i].z};
        vertices[i].normal={mesh->mNormals[i].x,mesh->mNormals[i].y,mesh->mNormals[i].z};
    }
    return vertices;
}
void MeshBPWS::applyUBO(){
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer=uniformBuffer;
    bufferInfo.offset=0;
    bufferInfo.range=VK_WHOLE_SIZE;
    VkWriteDescriptorSet writeInfo{};
    writeInfo.sType=VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeInfo.descriptorCount=1;
    writeInfo.descriptorType=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writeInfo.dstBinding=0;
    writeInfo.dstSet=descriptorSets[0];
    writeInfo.pBufferInfo=&bufferInfo;
    vkUpdateDescriptorSets(renderer->getDevice(),1,&writeInfo,0,nullptr);
}
void MeshBPWS::applyUBO(VkImageView texture){
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer=uniformBuffer;
    bufferInfo.offset=0;
    bufferInfo.range=VK_WHOLE_SIZE;
    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout=VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL;
    imageInfo.sampler=sampler;
    imageInfo.imageView=texture;
    VkWriteDescriptorSet writeInfo[2];
    writeInfo[0]=VkWriteDescriptorSet{};
    writeInfo[0].sType=VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeInfo[0].descriptorCount=1;
    writeInfo[0].descriptorType=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writeInfo[0].dstBinding=0;
    writeInfo[0].dstSet=descriptorSets[1];
    writeInfo[0].pBufferInfo=&bufferInfo;
    writeInfo[1]=VkWriteDescriptorSet{};
    writeInfo[1].sType=VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeInfo[1].descriptorCount=1;
    writeInfo[1].descriptorType=VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writeInfo[1].dstBinding=1;
    writeInfo[1].dstSet=descriptorSets[1];
    writeInfo[1].pImageInfo=&imageInfo;
    vkUpdateDescriptorSets(renderer->getDevice(),2,writeInfo,0,nullptr);
}
VkDescriptorSet* MeshBPWS::getSecondDescriptorSet(){
    return &descriptorSets[1];
}
void MeshBPWS::destroySharedSampler(){
    vkDestroySampler(RendererBPWS::instance().getDevice(),sampler,ALLOCATOR);
}