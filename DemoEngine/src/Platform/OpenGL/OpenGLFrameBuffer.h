#pragma once
#include "Renderer/Data/Framebuffer.h"

namespace DemoEngine
{
	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecification& spec); 
		virtual ~OpenGLFramebuffer();

		virtual void Bind() override;
		virtual void Unbind() override;
		virtual void Invalidate() override;
		virtual void Cleanup() override;
		virtual void Resize(uint32_t width, uint32_t height) override;

		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;
		virtual void ClearAttachment(uint32_t attachmentIndex, int value) override;

		virtual uint32_t GetColourAttachmentRendererID(uint32_t index = 0) const override {
			CORE_ASSERT("index exceeds number of colour attachments", index < m_ColourAttachments.size()); 
			return m_ColourAttachments[index];
		}

		virtual const FramebufferSpecification& GetSpecification() const override {
			return m_Specification;
		};

	private:
		uint32_t m_RendererID;
		FramebufferSpecification m_Specification;


		std::vector<FramebufferTextureSpecification> m_ColourAttachmentSpecifications; 
		FramebufferTextureSpecification m_DepthAttachmentSpecification;

		std::vector<uint32_t> m_ColourAttachments;
		uint32_t m_DepthAttachment;
	};
}