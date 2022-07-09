#pragma once
#include "texture.h"

class FramebufferMS
{
private:
	unsigned int m_ID;
	MultiSampleTexture m_ColorTexture;
	unsigned int m_Width;
	unsigned int m_Height;
	unsigned int m_RendebufferID;
public:
	FramebufferMS(unsigned int width, unsigned int height, unsigned int samples = 1);
	FramebufferMS(const FramebufferMS& fb);
	FramebufferMS& operator=(const FramebufferMS& fb);
	void samples(unsigned int samples);
	inline void bind() const { GLDEBUG(glBindFramebuffer(GL_FRAMEBUFFER, m_ID)); }
	inline const MultiSampleTexture& texture() const { return m_ColorTexture; }
	inline unsigned int width() const { return m_Width; }
	inline unsigned int height() const { return m_Height; }
	inline unsigned int DSBufferID() const { return m_RendebufferID; }
	inline ~FramebufferMS() {
		GLDEBUG(glDeleteRenderbuffers(1, &m_RendebufferID));
		GLDEBUG(glDeleteFramebuffers(1, &m_ID));
	}
};

class Framebuffer
{
private:
	unsigned int m_ID;
	Texture m_ColorTexture;
	unsigned int m_Width;
	unsigned int m_Height;
	unsigned int m_RendebufferID;
public:
	Framebuffer(unsigned int width, unsigned int height);
	Framebuffer(const Framebuffer& fb);
	Framebuffer& operator=(const Framebuffer& fb);
	inline void bind() const { GLDEBUG(glBindFramebuffer(GL_FRAMEBUFFER, m_ID)); }
	inline const Texture& texture() const { return m_ColorTexture; }
	inline unsigned int width() const { return m_Width; }
	inline unsigned int height() const { return m_Height; }
	inline unsigned int DSBufferID() const { return m_RendebufferID; }
	inline void setTextureAttribs(TEXTURE_ATTRIBS attribs) { m_ColorTexture.setAttribs(attribs); }
	inline ~Framebuffer() {
		GLDEBUG(glDeleteRenderbuffers(1, &m_RendebufferID));
		GLDEBUG(glDeleteFramebuffers(1, &m_ID));
	}
};
