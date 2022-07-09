#include "framebuffer.h"

FramebufferMS::FramebufferMS(unsigned int width, unsigned int height, unsigned int samples /*= 1*/): m_Width(width), m_Height(height), m_ColorTexture(width, height, samples)
{
	GLDEBUG(glGenFramebuffers(1, &m_ID));
	GLDEBUG(glBindFramebuffer(GL_FRAMEBUFFER, m_ID));
	//GLDEBUG(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_ColorTexture.ID(), 0));
	GLDEBUG(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_ColorTexture.ID(), 0));
	GLDEBUG(glGenRenderbuffers(1, &m_RendebufferID));
	GLDEBUG(glBindRenderbuffer(GL_RENDERBUFFER, m_RendebufferID));
	//GLDEBUG(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height));
	GLDEBUG(glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_ColorTexture.spp(), GL_DEPTH24_STENCIL8, width, height));
	GLDEBUG(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RendebufferID));
	GLDEBUG(glBindRenderbuffer(GL_RENDERBUFFER, 0));
	int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "error: ms framebuffer " << m_ID << " is not complete! <" << std::hex << GLDecode(status, "errlist.txt") << ">" << std::dec << std::endl;
	GLDEBUG(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

FramebufferMS::FramebufferMS(const FramebufferMS& fb) : m_ColorTexture(fb.m_ColorTexture), m_Width(fb.m_Width), m_Height(fb.m_Height)
{
	GLDEBUG(glGenFramebuffers(1, &m_ID));
	GLDEBUG(glBindFramebuffer(GL_FRAMEBUFFER, m_ID));
	//GLDEBUG(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_ColorTexture.ID(), 0));
	GLDEBUG(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_ColorTexture.ID(), 0));
	GLDEBUG(glGenRenderbuffers(1, &m_RendebufferID));
	GLDEBUG(glBindRenderbuffer(GL_RENDERBUFFER, m_RendebufferID));
	//GLDEBUG(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, fb.width(), fb.height()));
	GLDEBUG(glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_ColorTexture.spp(), GL_DEPTH24_STENCIL8, fb.width(), fb.height()));
	GLDEBUG(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RendebufferID));
	GLDEBUG(glBindRenderbuffer(GL_RENDERBUFFER, 0));
	int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "error: ms framebuffer " << m_ID << " is not complete! <" << std::hex << GLDecode(status, "errlist.txt") << ">" << std::dec << std::endl;
	GLDEBUG(glBindFramebuffer(GL_FRAMEBUFFER, 0));
};

FramebufferMS& FramebufferMS::operator=(const FramebufferMS& fb)
{
	if (m_RendebufferID)
	{
		GLDEBUG(glDeleteRenderbuffers(1, &m_RendebufferID));
	}
	if (m_ID)
	{
		GLDEBUG(glDeleteFramebuffers(1, &m_ID));
	}
	m_Width = fb.m_Width;
	m_Height = fb.m_Height;
	m_ColorTexture = fb.m_ColorTexture;
	GLDEBUG(glGenFramebuffers(1, &m_ID));
	GLDEBUG(glBindFramebuffer(GL_FRAMEBUFFER, m_ID));
	//GLDEBUG(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_ColorTexture.ID(), 0));
	GLDEBUG(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_ColorTexture.ID(), 0));
	GLDEBUG(glGenRenderbuffers(1, &m_RendebufferID));
	GLDEBUG(glBindRenderbuffer(GL_RENDERBUFFER, m_RendebufferID));
	//GLDEBUG(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, fb.width(), fb.height()));
	GLDEBUG(glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_ColorTexture.spp(), GL_DEPTH24_STENCIL8, fb.width(), fb.height()));
	GLDEBUG(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RendebufferID));
	GLDEBUG(glBindRenderbuffer(GL_RENDERBUFFER, 0));
	int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "error: ms framebuffer " << m_ID << " is not complete! <" << std::hex << GLDecode(status, "errlist.txt") << ">" << std::dec << std::endl;
	GLDEBUG(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	return (*this);
};

void FramebufferMS::samples(unsigned int samples)
{
	m_ColorTexture = MultiSampleTexture(m_Width, m_Height, samples);
	GLDEBUG(glBindFramebuffer(GL_FRAMEBUFFER, m_ID));
	//GLDEBUG(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_ColorTexture.ID(), 0));
	GLDEBUG(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_ColorTexture.ID(), 0));
	if (m_RendebufferID) 
	{
		GLDEBUG(glDeleteRenderbuffers(1, &m_RendebufferID));
	}
	GLDEBUG(glGenRenderbuffers(1, &m_RendebufferID));
	GLDEBUG(glBindRenderbuffer(GL_RENDERBUFFER, m_RendebufferID));
	//GLDEBUG(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height));
	GLDEBUG(glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_ColorTexture.spp(), GL_DEPTH24_STENCIL8, m_Width, m_Height));
	GLDEBUG(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RendebufferID));
	GLDEBUG(glBindRenderbuffer(GL_RENDERBUFFER, 0));
	int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "error: ms framebuffer is not complete! <" << std::hex << GLDecode(status, "errlist.txt") << ">" << std::dec << std::endl;
	GLDEBUG(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}



Framebuffer::Framebuffer(unsigned int width, unsigned int height) : m_Width(width), m_Height(height), m_ColorTexture(width, height)
{
	GLDEBUG(glGenFramebuffers(1, &m_ID));
	GLDEBUG(glBindFramebuffer(GL_FRAMEBUFFER, m_ID));
	//GLDEBUG(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_ColorTexture.ID(), 0));
	GLDEBUG(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorTexture.ID(), 0));
	GLDEBUG(glGenRenderbuffers(1, &m_RendebufferID));
	GLDEBUG(glBindRenderbuffer(GL_RENDERBUFFER, m_RendebufferID));
	//GLDEBUG(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height));
	GLDEBUG(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height));
	GLDEBUG(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RendebufferID));
	GLDEBUG(glBindRenderbuffer(GL_RENDERBUFFER, 0));
	int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "error: non ms framebuffer " << m_ID << " is not complete! <" << std::hex << GLDecode(status, "errlist.txt") << ">" << std::dec << std::endl;
	GLDEBUG(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

Framebuffer::Framebuffer(const Framebuffer& fb) : m_ColorTexture(fb.m_ColorTexture), m_Width(fb.m_Width), m_Height(fb.m_Height)
{
	GLDEBUG(glGenFramebuffers(1, &m_ID));
	GLDEBUG(glBindFramebuffer(GL_FRAMEBUFFER, m_ID));
	//GLDEBUG(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_ColorTexture.ID(), 0));
	GLDEBUG(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorTexture.ID(), 0));
	GLDEBUG(glGenRenderbuffers(1, &m_RendebufferID));
	GLDEBUG(glBindRenderbuffer(GL_RENDERBUFFER, m_RendebufferID));
	//GLDEBUG(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, fb.width(), fb.height()));
	GLDEBUG(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, fb.width(), fb.height()));
	GLDEBUG(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RendebufferID));
	GLDEBUG(glBindRenderbuffer(GL_RENDERBUFFER, 0));
	int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "error: non ms framebuffer " << m_ID << " is not complete! <" << std::hex << GLDecode(status, "errlist.txt") << ">" << std::dec << std::endl;
	GLDEBUG(glBindFramebuffer(GL_FRAMEBUFFER, 0));
};

Framebuffer& Framebuffer::operator=(const Framebuffer& fb)
{
	if (m_RendebufferID)
	{
		GLDEBUG(glDeleteRenderbuffers(1, &m_RendebufferID));
	}
	if (m_ID)
	{
		GLDEBUG(glDeleteFramebuffers(1, &m_ID));
	}
	m_Width = fb.m_Width;
	m_Height = fb.m_Height;
	m_ColorTexture = fb.m_ColorTexture;
	GLDEBUG(glGenFramebuffers(1, &m_ID));
	GLDEBUG(glBindFramebuffer(GL_FRAMEBUFFER, m_ID));
	//GLDEBUG(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_ColorTexture.ID(), 0));
	GLDEBUG(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorTexture.ID(), 0));
	GLDEBUG(glGenRenderbuffers(1, &m_RendebufferID));
	GLDEBUG(glBindRenderbuffer(GL_RENDERBUFFER, m_RendebufferID));
	//GLDEBUG(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, fb.width(), fb.height()));
	GLDEBUG(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, fb.width(), fb.height()));
	GLDEBUG(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RendebufferID));
	GLDEBUG(glBindRenderbuffer(GL_RENDERBUFFER, 0));
	int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "error: non ms framebuffer " << m_ID << " is not complete! <" << std::hex << GLDecode(status, "errlist.txt") << ">" << std::dec << std::endl;
	GLDEBUG(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	return (*this);
};