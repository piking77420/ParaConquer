#pragma once

#include "vulkan_header.h"

BEGIN_PCCORE
    enum class AttachementIndex
    {
        Color00,
        Color01,
        Color02,
        Color03,
        Color04,
        Color05,
        Color06,
        Color07,
        Color08,
        Color09,
        Depth,
        Stencil,

        Max
    };


    struct Attachment
    {
        AttachementIndex attachementIndex {};
        VkFormat format;
        bool clearOnLoad = false;
        bool write = false;
        VkImageLayout imageLayoutRef = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        VkImageLayout imageLayoutDes = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    };

    class VulkanRenderPass
    {
    public:
        
        void Init(const std::vector<Attachment>& _attachments);
    
        void Destroy();
    
        VkRenderPass renderPass = VK_NULL_HANDLE;
    private:
        void ParseAttachementDescriptor(const Attachment& _in, VkAttachmentDescription* _out);
        
        void ParseAttachementReference(const Attachment& _in, std::vector<VkAttachmentReference>* _outColor,
            std::vector<VkAttachmentReference>* _outDepth, const size_t _index);
        
        void ParseAttachementType(const std::vector<Attachment>& _attachments,uint32_t* nbrOfColorPass, uint32_t* nbrOfDepth ,uint32_t* ResolveAttachments);

    };

END_PCCORE
