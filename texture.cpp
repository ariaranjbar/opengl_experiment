#include "texture.h"
#include "STB/stb_image.h"
#include <iostream>

float* textur_data_copy(const float* src, uint16_t width, uint16_t height, uint8_t cpp)
{
	//std::cout << "copying data:  width " << width << " *  height " << height << " * ccp " << int(cpp) << " = " << width * height * cpp << std::endl;
	int array_size = width * height  * cpp;
	float* data = new float[array_size];
	for (int i = 0; i < array_size; i++)
	{
		data[i] = src[i];
	}
	return data;
}

unsigned int dimension_at_level(unsigned int dimension, unsigned int level)
{
	unsigned int d = dimension;
	for (int i = 0; i < level; i++)
	{
		d = d / 2;
		if (!d)
			break;
	}
	return d;
}

Texture::Texture(unsigned int width, unsigned int height, unsigned int channels_per_pixel /*= 4*/,unsigned int filter /*=GL_LINEAR*/, bool mipmap /*=false*/, const float* data /*=nullptr*/):
	m_ID(0), m_Data(nullptr)
{
	m_FileName = "";

	m_Attribs.CPP = channels_per_pixel;

	m_Attribs.WIDTH = width;

	m_Attribs.HEIGHT = height;

	m_Attribs.FILTER = filter;

	m_Attribs.MIPMAP = mipmap;

	switch (m_Attribs.CPP)
	{
	case STBI_rgb_alpha:
		m_Attribs.GPU_FORMAT = GL_RGBA16F;
		break;
	case STBI_rgb:
		m_Attribs.GPU_FORMAT = GL_RGB16F;
		break;
	case STBI_grey_alpha:
		m_Attribs.GPU_FORMAT = GL_RG16F;
		break;
	case STBI_grey:
		m_Attribs.GPU_FORMAT = GL_R16F;
		break;
	default:
		std::cout << "unsupported channels per pixel: " << int(m_Attribs.CPP) << " 1" << std::endl;
		break;
	}
	reserve();
	if (data)
	{
		bind();
		m_Data = textur_data_copy(data, m_Attribs.WIDTH, m_Attribs.HEIGHT, m_Attribs.CPP);
		GLDEBUG(glTexImage2D(m_Attribs.BIND_TO, 0, m_Attribs.GPU_FORMAT, m_Attribs.WIDTH, m_Attribs.HEIGHT, 0, diskFormat(), GL_FLOAT, m_Data));
		mipMap(m_Attribs.MIPMAP); //load is a fresh start, this sets min/mag
		//std::cout << "making non ms " << m_ID << std::endl;
		std::cout << "filled texture " << m_ID << " with custom data" << std::endl;
	}
}

Texture::Texture(const TEXTURE_ATTRIBS& attribs):
	m_ID(0), m_Data(nullptr), m_Attribs(attribs)
{
	m_FileName = "";
	if (m_Attribs.WIDTH || m_Attribs.HEIGHT)
	{
		reserve();
	}
}

Texture::Texture(const Texture& texture) :
	m_ID(0), m_Data(nullptr), m_FileName(texture.m_FileName), m_Attribs(texture.m_Attribs)
{
	m_FileName = texture.m_FileName;

	m_Attribs = texture.m_Attribs;

	if (texture.m_ID)
	{
		GLDEBUG(glGenTextures(1, &m_ID));
		GLDEBUG(glActiveTexture(GL_TEXTURE0));
		GLDEBUG(glBindTexture(m_Attribs.BIND_TO, m_ID));
		GLDEBUG(glTexParameteri(m_Attribs.BIND_TO, GL_TEXTURE_WRAP_S, m_Attribs.EXTEND_S));
		GLDEBUG(glTexParameteri(m_Attribs.BIND_TO, GL_TEXTURE_WRAP_T, m_Attribs.EXTEND_T));
		if (texture.m_Data)
		{
			int array_size = m_Attribs.WIDTH * m_Attribs.HEIGHT * m_Attribs.CPP;
			m_Data = new float[array_size];
			for (int i = 0; i < array_size; i++)
			{
				m_Data[i] = texture.m_Data[i];
			}
			//std::cout << "data present, size " << array_size;
		}
		else
		{

			int stride = m_Attribs.CPP * m_Attribs.WIDTH;
			int bufferSize = stride * m_Attribs.HEIGHT;
			m_Data = new float[bufferSize];
			//std::cout << "data not present, allocated " << bufferSize;
			texture.bind();
			GLDEBUG(glPixelStorei(GL_PACK_ALIGNMENT, 1));
			GLDEBUG(glGetTexImage(m_Attribs.BIND_TO, 0, diskFormat(), GL_FLOAT, m_Data));
			GLDEBUG(glBindTexture(m_Attribs.BIND_TO, m_ID));
		}
		GLDEBUG(glTexImage2D(m_Attribs.BIND_TO, 0, m_Attribs.GPU_FORMAT, m_Attribs.WIDTH, m_Attribs.HEIGHT, 0, diskFormat(), GL_FLOAT, m_Data));
		if (!texture.m_Data)
		{
			delete[] m_Data;
			m_Data = nullptr;
		}

		GLDEBUG(glBindTexture(m_Attribs.BIND_TO, 0));
		//std::cout << "making non ms " << m_ID << std::endl;
		mipMap(m_Attribs.MIPMAP);
	}
}

Texture& Texture::operator=(const Texture& texture)
{
	if (this == &texture)
		return *this;
	if (m_ID)
	{
		//std::cout << "destroying non ms " << m_ID << std::endl;
		free();
		GLDEBUG(glDeleteTextures(1, &m_ID));
	}

	m_FileName = texture.m_FileName;

	m_Attribs = texture.m_Attribs;

	if (texture.m_ID)
	{
		GLDEBUG(glGenTextures(1, &m_ID));
		GLDEBUG(glActiveTexture(GL_TEXTURE0));
		GLDEBUG(glBindTexture(m_Attribs.BIND_TO, m_ID));
		GLDEBUG(glTexParameteri(m_Attribs.BIND_TO, GL_TEXTURE_WRAP_S, m_Attribs.EXTEND_S));
		GLDEBUG(glTexParameteri(m_Attribs.BIND_TO, GL_TEXTURE_WRAP_T, m_Attribs.EXTEND_T));
		if (texture.m_Data)
		{
			int array_size = m_Attribs.WIDTH * m_Attribs.HEIGHT * m_Attribs.CPP;
			m_Data = new float[array_size];
			for (int i = 0; i < array_size; i++)
			{
				m_Data[i] = texture.m_Data[i];
			}
			//std::cout << "data present, size " << array_size;
		}
		else
		{
			int stride = m_Attribs.CPP * m_Attribs.WIDTH;
			int bufferSize = stride * m_Attribs.HEIGHT;
			m_Data = new float[bufferSize];
			//std::cout << "data not present, allocated " << bufferSize;
			texture.bind();
			GLDEBUG(glPixelStorei(GL_PACK_ALIGNMENT, 1));
			GLDEBUG(glGetTexImage(m_Attribs.BIND_TO, 0, diskFormat(), GL_FLOAT, m_Data));
			GLDEBUG(glBindTexture(m_Attribs.BIND_TO, m_ID));
		}
		GLDEBUG(glTexImage2D(m_Attribs.BIND_TO, 0, m_Attribs.GPU_FORMAT, m_Attribs.WIDTH, m_Attribs.HEIGHT, 0, diskFormat(), GL_FLOAT, m_Data));
		if (!texture.m_Data)
		{
			//std::cout << "deleting data to match the original" << std::endl;
			delete[] m_Data;
			m_Data = nullptr;
		}

		GLDEBUG(glBindTexture(m_Attribs.BIND_TO, 0));
		//std::cout << "making non ms " << m_ID << std::endl;
		mipMap(m_Attribs.MIPMAP);
	}
	return (*this);
}

void Texture::setAttribs(const TEXTURE_ATTRIBS& attribs)
{
	if (!m_ID)
	{
		//std::cout << "texture has no id" << std::endl;
		m_Attribs = attribs;
		return;
	}
	else
	{
		if ((m_Attribs.HEIGHT != attribs.HEIGHT) || (m_Attribs.WIDTH != attribs.WIDTH))
		{
			//std::cout << "height, width were altered" << std::endl;
			m_Attribs = attribs;
			if (m_Attribs.HEIGHT || m_Attribs.WIDTH)
			{
				//std::cout << "reserving for new dimensions" << std::endl;
				reserve();
			}
			else
			{
				//std::cout << "new dimensions are 0, deleting texture" << std::endl;
				free();
				GLDEBUG(glDeleteTextures(1, &m_ID));
			}

			return;
		}
		else
		{
			//std::cout << "key qualities unmodified" << std::endl;
			GLDEBUG(glActiveTexture(GL_TEXTURE0));
			if (m_Attribs.GPU_FORMAT != attribs.GPU_FORMAT)
			{
				//std::cout << "GPU format changed from " << GLDecode(m_Attribs.GPU_FORMAT, "glgpuformatlist.txt") << " to "
				//	<< GLDecode(attribs.GPU_FORMAT, "glgpuformatlist.txt") << " , need to re-upload texture" << std::endl;
				bind();
				m_Attribs.GPU_FORMAT = attribs.GPU_FORMAT;
				if (m_Attribs.CPP != attribs.CPP)
				{
					//std::cout << "CPP changed from " << static_cast<int>(m_Attribs.CPP) << " to " << static_cast<int>(attribs.CPP) << " , need to query texture data" << std::endl;
					free();
					m_Attribs.CPP = attribs.CPP;
				}
				if (!m_Data)
				{
					//std::cout << "data has been deleted, querying texture data" << std::endl;
					int stride = m_Attribs.CPP * m_Attribs.WIDTH;
					int bufferSize = stride * m_Attribs.HEIGHT;
					m_Data = new float[bufferSize];
					GLDEBUG(glPixelStorei(GL_PACK_ALIGNMENT, 1));
					GLDEBUG(glGetTexImage(m_Attribs.BIND_TO, 0, diskFormat(), GL_FLOAT, m_Data));
				}

				//std::cout << "data validated, uploading to GPU" << std::endl;
				int stride = m_Attribs.CPP * m_Attribs.WIDTH;
				int bufferSize = stride * m_Attribs.HEIGHT;
				GLDEBUG(glTexImage2D(m_Attribs.BIND_TO, 0, m_Attribs.GPU_FORMAT, m_Attribs.WIDTH, m_Attribs.HEIGHT, 0, diskFormat(), GL_FLOAT, m_Data));
				GLDEBUG(glBindTexture(m_Attribs.BIND_TO, 0));
			}
			//std::cout << "updating parameters" << std::endl;
			m_Attribs = attribs;
			GLDEBUG(glBindTexture(m_Attribs.BIND_TO, m_ID));
			GLDEBUG(glTexParameteri(m_Attribs.BIND_TO, GL_TEXTURE_WRAP_S, m_Attribs.EXTEND_S));
			GLDEBUG(glTexParameteri(m_Attribs.BIND_TO, GL_TEXTURE_WRAP_T, m_Attribs.EXTEND_T));
			mipMap(attribs.MIPMAP);
		}
	}
}

bool Texture::load(std::string filename, bool sRGB /*=false*/, bool keep_gpu_format /*=false*/)
{

	stbi_set_flip_vertically_on_load(1);
	int w, h, c;
	float* out;
	out = stbi_loadf(filename.c_str(), &w, &h, &c, 0);
	if (!out)
	{
		std::cout << "failed to load texture file [" << filename << "]" << std::endl;
		return false;
	}

	m_FileName = filename;
	if (m_ID)
	{
		//std::cout << "destroying non ms " << m_ID << std::endl;
		free();
		GLDEBUG(glDeleteTextures(1, &m_ID));
	}

	m_Data = out;
	m_Attribs.WIDTH = w;
	m_Attribs.HEIGHT = h;
	m_Attribs.CPP = c;
	GLDEBUG(glGenTextures(1, &m_ID));
	GLDEBUG(glActiveTexture(GL_TEXTURE0));
	GLDEBUG(glBindTexture(m_Attribs.BIND_TO, m_ID));
	GLDEBUG(glTexParameteri(m_Attribs.BIND_TO, GL_TEXTURE_WRAP_S, m_Attribs.EXTEND_S));
	GLDEBUG(glTexParameteri(m_Attribs.BIND_TO, GL_TEXTURE_WRAP_T, m_Attribs.EXTEND_T));

	if (sRGB)
	{
		m_Attribs.GPU_FORMAT = (diskFormat() > 3) ? GL_SRGB_ALPHA:GL_SRGB8;
	}
	else
	{
		if (!keep_gpu_format)
		{
			switch (m_Attribs.CPP)
			{
			case 4:
				m_Attribs.GPU_FORMAT = GL_RGBA16F;
				break;
			case 3:
				m_Attribs.GPU_FORMAT = GL_RGB16F;
				break;
			case 2:
				m_Attribs.GPU_FORMAT = GL_RG16F;
				break;
			case 1:
				m_Attribs.GPU_FORMAT = GL_R16F;
				break;
			default:
				break;
			}
		}
	}
	GLDEBUG(glTexImage2D(GL_TEXTURE_2D, 0, m_Attribs.GPU_FORMAT, m_Attribs.WIDTH, m_Attribs.HEIGHT, 0, diskFormat(), GL_FLOAT, m_Data));
	mipMap(m_Attribs.MIPMAP); //load is a fresh start, this sets min/mag

	GLDEBUG(glGenerateMipmap(m_Attribs.BIND_TO));
	GLDEBUG(glBindTexture(m_Attribs.BIND_TO, 0));
	mipMap(m_Attribs.MIPMAP);
	//std::cout << "making non ms " << m_ID << std::endl;
	std::cout << "loaded texture " << m_ID << " from [" << filename << "] Width: " << m_Attribs.WIDTH << ", Height: " << m_Attribs.HEIGHT << std::endl;
	return true;
}

void Texture::reserve()
{
	if (m_ID)
	{
		//std::cout << "destroying non ms " << m_ID << std::endl;
		free();
		GLDEBUG(glDeleteTextures(1, &m_ID));
	}
	m_FileName = "";

	GLDEBUG(glGenTextures(1, &m_ID));
	GLDEBUG(glActiveTexture(GL_TEXTURE0));
	GLDEBUG(glBindTexture(m_Attribs.BIND_TO, m_ID));
	GLDEBUG(glTexParameteri(m_Attribs.BIND_TO, GL_TEXTURE_WRAP_S, m_Attribs.EXTEND_S));
	GLDEBUG(glTexParameteri(m_Attribs.BIND_TO, GL_TEXTURE_WRAP_T, m_Attribs.EXTEND_T));

	GLDEBUG(glTexImage2D(m_Attribs.BIND_TO, 0, m_Attribs.GPU_FORMAT, m_Attribs.WIDTH, m_Attribs.HEIGHT, 0, diskFormat(), GL_FLOAT, NULL));

	mipMap(m_Attribs.MIPMAP);

	GLDEBUG(glBindTexture(m_Attribs.BIND_TO, 0));
	//std::cout << "making non ms " << m_ID << std::endl;
}

void Texture::reserve(unsigned int width, unsigned int height)
{
	m_Attribs.WIDTH = width;
	m_Attribs.HEIGHT = height;
	reserve();
}

void Texture::reserve(unsigned int width, unsigned int height, unsigned int channels_per_pixel)
{
	m_Attribs.CPP = channels_per_pixel;
	m_Attribs.WIDTH = width;
	m_Attribs.HEIGHT = height;
	reserve();
}

void Texture::bind() const
{
	//std::cout << "binding non ms" << std::endl;
	GLDEBUG(glBindTexture(m_Attribs.BIND_TO, m_ID));
}

void Texture::free()
{
	if (m_Data) stbi_image_free(m_Data);
	m_Data = nullptr;
}

void Texture::mipMap(bool mm)
{
	//std::cout << "setting mipmaps for texture " << m_ID << "... " << std::endl;;
	m_Attribs.MIPMAP = mm;
	GLDEBUG(glActiveTexture(GL_TEXTURE0));
	bind();
	if (m_Attribs.MIPMAP)
	{
		if (m_Attribs.FILTER == GL_LINEAR)
		{
			GLDEBUG(glTexParameteri(m_Attribs.BIND_TO, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
			GLDEBUG(glTexParameteri(m_Attribs.BIND_TO, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		}
		else if (m_Attribs.FILTER == GL_NEAREST)
		{
			GLDEBUG(glTexParameteri(m_Attribs.BIND_TO, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST));
			GLDEBUG(glTexParameteri(m_Attribs.BIND_TO, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		}
		calculateMipMap();
		//std::cout << "	done." << std::endl;
	}
	else
	{
		GLDEBUG(glTexParameteri(m_Attribs.BIND_TO, GL_TEXTURE_MIN_FILTER, m_Attribs.FILTER));
		GLDEBUG(glTexParameteri(m_Attribs.BIND_TO, GL_TEXTURE_MAG_FILTER, m_Attribs.FILTER));
		//std::cout << "	didn't need'em." << std::endl;
	}
}

void Texture::calculateMipMap()
{
	//std::cout << "		calculating...";
	GLDEBUG(glActiveTexture(GL_TEXTURE0));
	bind();
	GLDEBUG(glBindTexture(m_Attribs.BIND_TO, m_ID));
	GLDEBUG(glGenerateMipmap(m_Attribs.BIND_TO));
	GLDEBUG(glBindTexture(m_Attribs.BIND_TO, 0));
	//std::cout << "done." << std::endl;
}

MultiSampleTexture::MultiSampleTexture(unsigned int width, unsigned int height, unsigned int samples_per_pixel /*= 1*/, unsigned int channels_per_pixel /*= 4*/) :
	m_ID(0), m_Width(width), m_Height(height), m_CPP(channels_per_pixel), m_SPP(samples_per_pixel)
{
	GLDEBUG(glGenTextures(1, &m_ID));
	GLDEBUG(glActiveTexture(GL_TEXTURE0));
	GLDEBUG(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_ID));
	int internalformat;
	switch (m_CPP)
	{
	case STBI_rgb_alpha:
		internalformat = GL_RGBA;
		break;
	case STBI_rgb:
		internalformat = GL_RGB;
		break;
	case STBI_grey:
		internalformat = GL_RED;
		break;
	default:
		std::cout << "unsupported bit depth: "<< m_CPP;
		break;
	}
	GLDEBUG(glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples_per_pixel, internalformat, width, height, true));
	GLDEBUG(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0));
	//std::cout << "making ms " << m_ID << std::endl;
};

MultiSampleTexture::MultiSampleTexture(const MultiSampleTexture& ms): m_Width(ms.m_Width), m_Height(ms.m_Height), m_CPP(ms.m_CPP), m_SPP(ms.m_SPP)
{
	GLDEBUG(glGenTextures(1, &m_ID));
	GLDEBUG(glActiveTexture(GL_TEXTURE0));
	GLDEBUG(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_ID));
	int internalformat;
	switch (m_CPP)
	{
	case STBI_rgb_alpha:
		internalformat = GL_RGBA;
		break;
	case STBI_rgb:
		internalformat = GL_RGB;
		break;
	case STBI_grey:
		internalformat = GL_RED;
		break;
	default:
		std::cout << "unsupported bit depth: " << m_CPP;
		break;
	}
	GLDEBUG(glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_SPP, internalformat, m_Width, m_Height, true));
	GLDEBUG(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0));
	//std::cout << "making ms " << m_ID << std::endl;
}

MultiSampleTexture& MultiSampleTexture::operator=(const MultiSampleTexture& ms)
{
	if (m_ID)
	{
		//std::cout << "destroying ms " << m_ID << std::endl;
		GLDEBUG(glDeleteTextures(1, &m_ID));
	}
	GLDEBUG(glGenTextures(1, &m_ID));
	GLDEBUG(glActiveTexture(GL_TEXTURE0));
	GLDEBUG(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_ID));
	m_CPP = ms.m_CPP;
	m_SPP = ms.m_SPP;
	m_Width = ms.m_Width;
	m_Height = ms.m_Height;
	int internalformat;
	switch (m_CPP)
	{
	case STBI_rgb_alpha:
		internalformat = GL_RGBA;
		break;
	case STBI_rgb:
		internalformat = GL_RGB;
		break;
	case STBI_grey:
		internalformat = GL_RED;
		break;
	default:
		std::cout << "unsupported bit depth: " << m_CPP;
		break;
	}
	GLDEBUG(glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_SPP, internalformat, m_Width, m_Height, true));
	GLDEBUG(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0));
	//std::cout << "making ms " << m_ID << std::endl;
	return (*this);
}

void MultiSampleTexture::MultiSampleTexture::bind() const
{
	//std::cout << "binding ms" << std::endl;
	GLDEBUG(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_ID));
}



Cubemap::Cubemap() :
	m_ID(0)
{
	/*************************************/
	m_Attribs = TEXTURE_ATTRIBS{ true, 3, GL_TEXTURE_CUBE_MAP, GL_LINEAR, GL_CLAMP_TO_EDGE, 0, GL_RGB16F, 0 ,0 };
	cube_data data;
	for (int i = 0; i < 6; i++)
	{
		data[i] = nullptr;
	}
	m_Data.push_back(data);
	/*************************************/
};

Cubemap::Cubemap(CUBEMAP_INITIALIZER initializer) :
	m_ID(0)
{
	m_Attribs = TEXTURE_ATTRIBS{ true, initializer.CPP, GL_TEXTURE_CUBE_MAP, GL_LINEAR, GL_CLAMP_TO_EDGE, 0, GL_RGB16F, initializer.BASE_WIDTH, initializer.BASE_HEIGHT };
	cube_data data;

	load(initializer.RAW_DATA, initializer.BASE_WIDTH, initializer.BASE_HEIGHT, initializer.CPP);
}

Cubemap::Cubemap(const Cubemap& cubemap) :
	m_ID(0)
{
	/*************************************/
	m_Attribs = cubemap.m_Attribs;

	if (m_Attribs.WIDTH || m_Attribs.HEIGHT)
	{
		GLDEBUG(glGenTextures(1, &m_ID));
		GLDEBUG(glActiveTexture(GL_TEXTURE0));
		bind();
		for (int level = 0; level < cubemap.m_Data.size(); level++) 
		{
			cube_data data;
			for (int i = 0; i < 6; i++)
			{
				if (cubemap.m_Data[level][i])
				{
					data[i] = textur_data_copy(cubemap.m_Data[level][i], width(level), height(level), m_Attribs.CPP);
				}
				else
				{
					cubemap.bind();
					int stride = m_Attribs.CPP * m_Attribs.WIDTH;
					int bufferSize = stride * m_Attribs.HEIGHT;
					data[i] = new float[bufferSize];
					//std::cout << "data not present, allocated " << bufferSize;
					GLDEBUG(glPixelStorei(GL_PACK_ALIGNMENT, 1));
					GLDEBUG(glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, level, diskFormat(), GL_FLOAT, data[i]));
					bind();
				}
				m_Data.push_back(data);
				GLDEBUG(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, level, m_Attribs.GPU_FORMAT, width(level), height(level), 0, diskFormat(), GL_FLOAT, m_Data[level][i]));
			}
		}
		mipMap(m_Attribs.MIPMAP);
		GLDEBUG(glTexParameteri(m_Attribs.BIND_TO, GL_TEXTURE_WRAP_S, m_Attribs.EXTEND_S));
		GLDEBUG(glTexParameteri(m_Attribs.BIND_TO, GL_TEXTURE_WRAP_T, m_Attribs.EXTEND_S));
		GLDEBUG(glTexParameteri(m_Attribs.BIND_TO, GL_TEXTURE_WRAP_R, m_Attribs.EXTEND_S));
		GLDEBUG(glBindTexture(m_Attribs.BIND_TO, 0));
	}
	else
	{
		for (int level = 0; level < cubemap.m_Data.size(); level++)
		{
			cube_data data;
			for (int i = 0; i < 6; i++)
			{
				data[i] = nullptr;
			}
			m_Data.push_back(data);
		}
	}
	/*************************************/
	//std::cout << "making cubemap " << m_ID << std::endl;
}

Cubemap& Cubemap::operator=(const Cubemap& cubemap)
{
	/*************************************/
	if (this == &cubemap)
		return *this;
	if (m_ID)
	{
		free();
		GLDEBUG(glDeleteTextures(1, &m_ID));
	}
	
	m_Attribs = cubemap.m_Attribs;

	if (m_Attribs.WIDTH || m_Attribs.HEIGHT)
	{
		GLDEBUG(glGenTextures(1, &m_ID));
		GLDEBUG(glActiveTexture(GL_TEXTURE0));
		bind();
		for (int level = 0; level < cubemap.m_Data.size(); level++)
		{
			cube_data data;
			for (int i = 0; i < 6; i++)
			{
				if (cubemap.m_Data[level][i])
				{
					data[i] = textur_data_copy(cubemap.m_Data[level][i], width(level), height(level), m_Attribs.CPP);
				}
				else
				{
					cubemap.bind();
					int stride = m_Attribs.CPP * m_Attribs.WIDTH;
					int bufferSize = stride * m_Attribs.HEIGHT;
					data[i] = new float[bufferSize];
					//std::cout << "data not present, allocated " << bufferSize;
					GLDEBUG(glPixelStorei(GL_PACK_ALIGNMENT, 1));
					GLDEBUG(glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, level, diskFormat(), GL_FLOAT, data[i]));
					bind();
				}
				m_Data.push_back(data);
				GLDEBUG(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, level, m_Attribs.GPU_FORMAT, width(level), height(level), 0, diskFormat(), GL_FLOAT, m_Data[level][i]));
			}
		}
		mipMap(m_Attribs.MIPMAP);
		GLDEBUG(glTexParameteri(m_Attribs.BIND_TO, GL_TEXTURE_WRAP_S, m_Attribs.EXTEND_S));
		GLDEBUG(glTexParameteri(m_Attribs.BIND_TO, GL_TEXTURE_WRAP_T, m_Attribs.EXTEND_S));
		GLDEBUG(glTexParameteri(m_Attribs.BIND_TO, GL_TEXTURE_WRAP_R, m_Attribs.EXTEND_S));
		GLDEBUG(glBindTexture(m_Attribs.BIND_TO, 0));
	}
	else
	{
		for (int level = 0; level < cubemap.m_Data.size(); level++)
		{
			cube_data data;
			for (int i = 0; i < 6; i++)
			{
				data[i] = nullptr;
			}
			m_Data.push_back(data);
		}
	}

	return *this;
	/*************************************/
}


void Cubemap::load(std::vector<cube_data> raw_data, unsigned int base_width, unsigned int base_height, uint8_t cpp)
{
	/*************************************/
	
	if (m_ID)
	{
		//std::cout << "destroying cubemap " << m_ID << std::endl;
		free();
		GLDEBUG(glDeleteTextures(1, &m_ID));
	}

	m_Attribs.WIDTH = base_width;
	m_Attribs.HEIGHT = base_height;
	m_Attribs.CPP = cpp;

	if (sRGB())
	{
		m_Attribs.GPU_FORMAT = (m_Attribs.CPP > 3) ? GL_SRGB_ALPHA : GL_SRGB8;
	}
	else
	{
		switch (m_Attribs.CPP)
		{
		case 4:
			m_Attribs.GPU_FORMAT = GL_RGBA16F;
			break;
		case 3:
			m_Attribs.GPU_FORMAT = GL_RGB16F;
			break;
		case 2:
			m_Attribs.GPU_FORMAT = GL_RG16F;
			break;
		case 1:
			m_Attribs.GPU_FORMAT = GL_R16F;
			break;
		default:
			break;
		}
	}

	GLDEBUG(glGenTextures(1, &m_ID));
	bind();
	GLDEBUG(glTexParameteri(m_Attribs.BIND_TO, GL_TEXTURE_WRAP_S, m_Attribs.EXTEND_S));
	GLDEBUG(glTexParameteri(m_Attribs.BIND_TO, GL_TEXTURE_WRAP_T, m_Attribs.EXTEND_S));
	GLDEBUG(glTexParameteri(m_Attribs.BIND_TO, GL_TEXTURE_WRAP_R, m_Attribs.EXTEND_S));

	for (int level = 0; level < raw_data.size(); level++)
	{
		cube_data data;
		for (int i = 0; i < 6; i++)
		{
			//std::cout << "copying data of face " << i << " with dimensions " << width(level) << " x " << height(level) << std::endl;
			data[i] = textur_data_copy(raw_data[level][i], width(level), height(level), cpp);
			//std::cout << "done." << std::endl;
			GLDEBUG(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, level, m_Attribs.GPU_FORMAT, width(level), height(level), 0, diskFormat(), GL_FLOAT, data[i]));
		}
			m_Data.push_back(data);
	}
	mipMap(m_Attribs.MIPMAP);
	GLDEBUG(glBindTexture(m_Attribs.BIND_TO, 0));

	std::cout << "constructed cubemap " << m_ID << " with " << m_Data.size() - 1 << " mipmap levels from raw data. base Width: " << m_Attribs.WIDTH << ",  base Height: " << m_Attribs.HEIGHT << std::endl;

	/*************************************/
}

void Cubemap::bind() const
{
	/*************************************/
	//std::cout << "binding cubemap" << std::endl;
	GLDEBUG(glBindTexture(m_Attribs.BIND_TO, m_ID));
	/*************************************/
}

void Cubemap::free()
{
	/*************************************/
	for (int level = 0; level < m_Data.size(); level++)
	{
		for (int i = 0; i < 6; i++)
		{
			if (m_Data[level][i]) stbi_image_free(m_Data[level][i]);
			m_Data[level][i] = nullptr;
		}
		m_Data = std::vector<cube_data>();
	}
	
	/*************************************/
}

void Cubemap::mipMap(bool mm)
{
	//std::cout << "setting mipmaps for cubemap " << m_ID << "..." << std::endl;
	/*************************************/
	m_Attribs.MIPMAP = mm;
	GLDEBUG(glActiveTexture(GL_TEXTURE0));
	bind();
	if (m_Attribs.MIPMAP)
	{
		if (m_Attribs.FILTER == GL_LINEAR)
		{
			GLDEBUG(glTexParameteri(m_Attribs.BIND_TO, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
			GLDEBUG(glTexParameteri(m_Attribs.BIND_TO, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		}
		else if (m_Attribs.FILTER == GL_NEAREST)
		{
			GLDEBUG(glTexParameteri(m_Attribs.BIND_TO, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST));
			GLDEBUG(glTexParameteri(m_Attribs.BIND_TO, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		}
		GLDEBUG(glTexParameteri(m_Attribs.BIND_TO, GL_TEXTURE_MAX_LEVEL, m_Data.size() - 1));
		//std::cout << "	done." << std::endl;
	}
	else
	{
		GLDEBUG(glTexParameteri(m_Attribs.BIND_TO, GL_TEXTURE_MIN_FILTER, m_Attribs.FILTER));
		GLDEBUG(glTexParameteri(m_Attribs.BIND_TO, GL_TEXTURE_MAG_FILTER, m_Attribs.FILTER));
		//std::cout << "	didn't need'em." << std::endl;
	}
	/*************************************/
}

