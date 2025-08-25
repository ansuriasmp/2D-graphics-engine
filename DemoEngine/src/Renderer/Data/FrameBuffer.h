#pragma once
#include "Core/Core.h"

namespace DemoEngine
{


	enum class FramebufferTextureFormat
	{
		None = 0,
		//Color
		RGBA8,
		RED_INTEGER,

		//Depth / Stencil
		DEPTH24STENCIL8,

		//Default
		Depth = DEPTH24STENCIL8

	};

	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(FramebufferTextureFormat format)
			: TextureFormat(format) {
		}

		FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
		//TODO: filtering/wrap
	
	};
	
	struct FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> startingAttachments)
			: Attachments(startingAttachments) {
		}	
		std::vector<FramebufferTextureSpecification> Attachments;
	};

	struct FramebufferSpecification
	{
		uint32_t Width, Height;
		uint32_t Samples = 1;
		bool SwapChainTarget = false;
		FramebufferAttachmentSpecification Attachments;
	};

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;
		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Invalidate() = 0;
		virtual void Cleanup() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;


		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;

		virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;

		virtual uint32_t GetColourAttachmentRendererID(uint32_t index = 0) const = 0;

		virtual const FramebufferSpecification& GetSpecification() const = 0;

		static Ref<Framebuffer> Create(const FramebufferSpecification& spec);
	};
}
