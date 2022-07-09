#pragma once
#include <vector>
#include "renderutils.h"
#include "bufferobject.h"

class LayoutAttribute
{
public:
	unsigned int ATTRIB_ID;
	unsigned int ELEMENT_TYPE;		//type of element data members. GL define expected 
	unsigned int ELEMENT_SIZE;		//size of element data.
	unsigned int ELEMENT_COUNT;		//how many data members per vertex in element
	bool ELEMENT_NRML;				//is element normalized


	LayoutAttribute(unsigned int id, unsigned int type, unsigned int count, bool normalize = false) : ATTRIB_ID(id), ELEMENT_TYPE(type), ELEMENT_SIZE(0), ELEMENT_COUNT(count), ELEMENT_NRML(normalize)
	{
		switch (ELEMENT_TYPE)
		{
		case GL_FLOAT:
			ELEMENT_SIZE = 4;
			break;
		case GL_UNSIGNED_INT:
			ELEMENT_SIZE = 4;
			break;
		case GL_UNSIGNED_BYTE:
			ELEMENT_SIZE = 1;
			break;
		default:
			ASSERT(false);
			break;
		}
	};
};


class VertexBuffer :
	public BufferObject
{
private:
	std::vector<LayoutAttribute> m_Layout;
	unsigned int m_Stride;
public:
	inline VertexBuffer() : BufferObject(GL_ARRAY_BUFFER), m_Stride(0) {};
	void addAttribute(LayoutAttribute attribute);
	inline unsigned int getStride() const { return m_Stride; };
	inline const std::vector<LayoutAttribute>& getLayout() const { return m_Layout; };
};

