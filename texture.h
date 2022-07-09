#pragma once
#include "renderutils.h"
#include <string>
#include <array>



float* textur_data_copy(const float* src, uint16_t width, uint16_t height, uint8_t channels_per_pixel);

unsigned int dimension_at_level(unsigned int dimension, unsigned int level);

typedef std::array<float*, 6> cube_data;

struct TEXTURE_ATTRIBS
{
	bool MIPMAP = false;
	uint8_t CPP = 4;
	unsigned int BIND_TO = GL_TEXTURE_2D;
	unsigned int FILTER = GL_LINEAR;
	unsigned int EXTEND_S = GL_REPEAT;
	unsigned int EXTEND_T = GL_REPEAT;
	unsigned int GPU_FORMAT = GL_RGBA16F;
	uint16_t WIDTH = 0;
	uint16_t HEIGHT = 0;
};

struct CUBEMAP_INITIALIZER
{
	uint8_t CPP;
	uint16_t BASE_WIDTH;
	uint16_t BASE_HEIGHT;
	std::vector<cube_data> RAW_DATA;
};

class Texture
{
private:

	unsigned int m_ID;
	float* m_Data;
	std::string m_FileName;
	TEXTURE_ATTRIBS m_Attribs;
	

public:

	inline Texture() : m_ID(0), m_Data(nullptr), m_FileName(""), m_Attribs(TEXTURE_ATTRIBS()) {
	};
	inline Texture(std::string filename, bool sRGB = false, bool mipmap = false) :
		m_ID(0), m_Data(nullptr), m_FileName("")
	{
		load(filename, sRGB);
		mipMap(mipmap);
	};
	Texture(unsigned int width, unsigned int height, unsigned int channels_per_pixel = 4, unsigned int filter = GL_LINEAR, bool mipmap = false, const float* data = nullptr);
	Texture(const TEXTURE_ATTRIBS& attribs);
	Texture(const Texture& texture);
	Texture& operator=(const Texture& texture);

	void setAttribs(const TEXTURE_ATTRIBS& attribs);
	bool load(std::string filename, bool sRGB = false, bool keep_gpu_format = false);
	void reserve();
	void reserve(unsigned int width, unsigned int height);
	void reserve(unsigned int width, unsigned int height, unsigned int channels_per_pixel);
	void bind() const;
	void free();
	void mipMap(bool mm);
	void calculateMipMap();

	inline unsigned int ID()const { return m_ID; }
	inline unsigned int width()const { return m_Attribs.WIDTH; }
	inline unsigned int height()const { return m_Attribs.HEIGHT; }
	inline unsigned int cpp()const { return m_Attribs.CPP; }
	inline unsigned int diskFormat()const {
		switch (m_Attribs.CPP)
		{
		case 4:
			return GL_RGBA;
			break;
		case 3:
			return GL_RGB;
			break;
		case 2:
			return GL_RG;
			break;
		case 1:
			return GL_RED;
			break;
		default:
			std::cout << "unsupported channels per pixel: " << int(m_Attribs.CPP) << " in texture " << m_ID << std::endl;
			return 0;
			break;
		}
	}
	inline bool sRGB()const { return (m_Attribs.GPU_FORMAT == GL_SRGB8_ALPHA8 || m_Attribs.GPU_FORMAT == GL_SRGB8); }
	inline std::string filename()const { return m_FileName; }
	inline TEXTURE_ATTRIBS attribs()const { return m_Attribs; }
	inline ~Texture() {
		free();
		GLDEBUG(glDeleteTextures(1, &m_ID));
		//std::cout << "destroying non ms " << m_ID << std::endl;
	};
};

class MultiSampleTexture
{
private:
	unsigned int m_ID;
	int m_Width;
	int m_Height;
	int m_CPP; //bytes per pixel
	int m_SPP; //samples per pixel
public:
	MultiSampleTexture(unsigned int width, unsigned int height, unsigned int samples_per_pixel = 1, unsigned int channels_per_pixel = 4);
	MultiSampleTexture(const MultiSampleTexture& ms);
	MultiSampleTexture& operator=(const MultiSampleTexture& ms);
	void bind() const;
	inline unsigned int ID()const { return m_ID; }
	inline unsigned int width()const { return m_Width; }
	inline unsigned int height()const { return m_Height; }
	inline unsigned int cpp()const { return m_CPP; }
	inline unsigned int spp()const { return m_SPP; }
	inline ~MultiSampleTexture() {
		GLDEBUG(glDeleteTextures(1, &m_ID));
		//std::cout << "destroying ms " << m_ID << std::endl;
	};
};

class Cubemap
{
private:

	unsigned int m_ID;
	TEXTURE_ATTRIBS m_Attribs;
	std::vector<cube_data> m_Data;


public:

	Cubemap();

	Cubemap(CUBEMAP_INITIALIZER initializer);
	Cubemap(const Cubemap& cubemap);
	Cubemap& operator=(const Cubemap& cubemap);

	void load(std::vector<cube_data> raw_data, unsigned int base_width, unsigned int base_height, uint8_t cpp);
	void bind() const;
	void free();
	void mipMap(bool mm);

	inline unsigned int ID()const { return m_ID; }
	inline unsigned int width(unsigned int level = 0)const { return dimension_at_level(m_Attribs.WIDTH, level); }
	inline unsigned int height(unsigned int level = 0)const { return dimension_at_level(m_Attribs.HEIGHT, level); }
	inline unsigned int cpp()const { return m_Attribs.CPP; }
	inline bool sRGB()const { return (m_Attribs.GPU_FORMAT == GL_SRGB8_ALPHA8 || m_Attribs.GPU_FORMAT == GL_SRGB8); }
	inline unsigned int diskFormat()const {
		switch (m_Attribs.CPP)
		{
		case 4:
			return GL_RGBA;
			break;
		case 3:
			return GL_RGB;
			break;
		case 2:
			return GL_RG;
			break;
		case 1:
			return GL_RED;
			break;
		default:
			std::cout << "unsupported channels per pixel: " << int(m_Attribs.CPP) << " in cubemap " << m_ID << std::endl;
			return 0;
			break;
		}
	}
	inline ~Cubemap() {
		free();
		GLDEBUG(glDeleteTextures(1, &m_ID));
		//std::cout << "destroying non ms " << m_ID << std::endl;
	};
};