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
#include<DescriptorSetLayoutCM.h>
using namespace WideOpenCM;
using namespace Common;
DescriptorSetLayoutCM::DescriptorSetLayoutCM(){}
DescriptorSetLayoutCM& DescriptorSetLayoutCM::instance(){
    static DescriptorSetLayoutCM dsl;
    return dsl;
}
DescriptorSetLayoutCM::~DescriptorSetLayoutCM(){}
void DescriptorSetLayoutCM::init(Renderer* renderer){
    this->renderer=renderer;
    VkDescriptorSetLayoutBinding bindings[2];
    bindings[0]=VkDescriptorSetLayoutBinding{};
    bindings[1]=VkDescriptorSetLayoutBinding{};
    bindings[0].binding=0;
    bindings[0].descriptorCount=1;
    bindings[0].descriptorType=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindings[0].stageFlags=VK_SHADER_STAGE_ALL;
    /*cube map binding for DSL*/
    bindings[1].binding=1;
    bindings[1].descriptorCount=1;
    bindings[1].descriptorType=VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[1].stageFlags=VK_SHADER_STAGE_ALL;
    VkDescriptorSetLayoutCreateInfo createInfo{};
    createInfo.bindingCount=2;
    createInfo.pBindings=bindings;
    createInfo.sType=VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    if(vkCreateDescriptorSetLayout(renderer->getDevice(),&createInfo,ALLOCATOR,&descriptorSetLayout)!=VK_SUCCESS){
        LOG.error("Failed to create dsl");
    }
}