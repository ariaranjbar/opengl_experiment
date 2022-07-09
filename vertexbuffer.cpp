#include "vertexbuffer.h"

void VertexBuffer::addAttribute(LayoutAttribute attribute)
{
	m_Layout.push_back(attribute);
	m_Stride += attribute.ELEMENT_COUNT * attribute.ELEMENT_SIZE;
}
