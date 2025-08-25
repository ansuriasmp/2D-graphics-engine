#include "DemoEngine_PCH.h" 
#include "OpenGLFramebuffer.h"

#include <glad/glad.h>
#include "OpenGLFramebufferUtils.h"

namespace DemoEngine
{
	static const uint32_t s_MaxFramebufferSize = 8192;

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
		: m_Specification(spec)
	{

		for (auto spec : m_Specification.Attachments.Attachments)
		{
			if (!DemoEngine::FramebufferUtils::OpenGLUtils::IsDepthFormat(spec.TextureFormat))
				m_ColourAttachmentSpecifications.emplace_back(spec);
			else
				m_DepthAttachmentSpecification = spec;
		}

		Invalidate();
	}
	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		Cleanup();
	}

	void OpenGLFramebuffer::Cleanup()
	{
		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures(m_ColourAttachments.size(), m_ColourAttachments.data());
		glDeleteTextures(1, &m_DepthAttachment);

		m_ColourAttachments.clear();
		m_DepthAttachment = 0;

	}

	void OpenGLFramebuffer::Invalidate()
	{
		if (m_RendererID)
		{
			//If it is set, we need to delete everything to 'clean' up 
			// //before making a new framebuffer
			Cleanup();
		}

		glCreateFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		bool multiSample = m_Specification.Samples > 1;

		//Attachments
		if (m_ColourAttachmentSpecifications.size())
		{
			m_ColourAttachments.resize(m_ColourAttachmentSpecifications.size());
			DemoEngine::FramebufferUtils::OpenGLUtils::CreateTextures(multiSample, m_ColourAttachments.data(), m_ColourAttachments.size());
			
			for (size_t i = 0; i < m_ColourAttachments.size(); i++)
			{
				DemoEngine::FramebufferUtils::OpenGLUtils::BindTexture(multiSample, m_ColourAttachments[i]);
				switch (m_ColourAttachmentSpecifications[i].TextureFormat)
				{
				case FramebufferTextureFormat::RGBA8:
					DemoEngine::FramebufferUtils::OpenGLUtils::AttachColourTexture(m_ColourAttachments[i], m_Specification.Samples, GL_RGBA8, GL_RGBA, m_Specification.Width, m_Specification.Height, i);
					break;
				case FramebufferTextureFormat::RED_INTEGER:
					DemoEngine::FramebufferUtils::OpenGLUtils::AttachColourTexture(m_ColourAttachments[i], m_Specification.Samples, GL_R32I, GL_RED_INTEGER , m_Specification.Width, m_Specification.Height, i);
					break;
				}
			}
		}

		if (m_DepthAttachmentSpecification.TextureFormat != FramebufferTextureFormat::None)
		{
			DemoEngine::FramebufferUtils::OpenGLUtils::CreateTextures(multiSample, &m_DepthAttachment, 1); 
			DemoEngine::FramebufferUtils::OpenGLUtils::BindTexture(multiSample, m_DepthAttachment);
			
			switch (m_DepthAttachmentSpecification.TextureFormat)
			{
			case FramebufferTextureFormat::DEPTH24STENCIL8:
				DemoEngine::FramebufferUtils::OpenGLUtils::AttachDepthTexture(m_DepthAttachment, m_Specification.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Specification.Width, m_Specification.Height);
				break;
			}
		}

		if (m_ColourAttachments.size() > 1)
		{
			CORE_ASSERT("MAX Colour attachments supported is 4", m_ColourAttachments.size() <= 4);
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,GL_COLOR_ATTACHMENT3 }; 
			
			glDrawBuffers(m_ColourAttachments.size(), buffers);
		}
		else if (m_ColourAttachments.empty())
		{
			//only depth pass
			glDrawBuffer(GL_NONE);
		}

		
		CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}

	void OpenGLFramebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Specification.Width, m_Specification.Height);
	}
	void OpenGLFramebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > s_MaxFramebufferSize || height > s_MaxFramebufferSize)
		{
			LOG_WARN("Attempted to resize Framebuffer to {0}, {1}", width, height);
			return;
		}

		m_Specification.Width = width;
		m_Specification.Height = height;

		Invalidate();
	}

	int OpenGLFramebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
	{
		CORE_ASSERT("attachement Index exceeds number of colour attachments", attachmentIndex < m_ColourAttachments.size());
		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		int pixelData;

		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
		return pixelData;
	}

	void OpenGLFramebuffer::ClearAttachment(uint32_t attachmentIndex, int value)
	{
		CORE_ASSERT("attachement Index exceeds number of colour attachments", attachmentIndex < m_ColourAttachments.size());
		
		auto& spec = m_ColourAttachmentSpecifications[attachmentIndex];
		glClearTexImage(m_ColourAttachments[attachmentIndex], 0,
			DemoEngine::FramebufferUtils::OpenGLUtils::TextureFormatToGL(spec.TextureFormat),
			GL_INT,
			&value);
	}
}