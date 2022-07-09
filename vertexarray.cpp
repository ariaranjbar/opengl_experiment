#include "vertexarray.h"

void VertexArray::linkVertexBuffer(const VertexBuffer& vertexbuffer)
{
	bind();
	vertexbuffer.bind();
	const auto& layout = vertexbuffer.getLayout();
	unsigned int stride = vertexbuffer.getStride();
	unsigned int offset = 0;
	for (int i = 0; i < layout.size(); i++)
	{
		const auto& attribute = layout[i];
		GLDEBUG(glVertexAttribPointer(attribute.ATTRIB_ID, attribute.ELEMENT_COUNT, attribute.ELEMENT_TYPE, attribute.ELEMENT_NRML, stride, reinterpret_cast <void*> (offset)));
		offset += attribute.ELEMENT_SIZE * attribute.ELEMENT_COUNT;
	}
	vertexbuffer.unbind();
	unbind();
}

void VertexArray::enableAttribute(unsigned int attributeIndex) const
{
	bind();
	GLDEBUG(glEnableVertexAttribArray(attributeIndex));
}

void VertexArray::disableAttribute(unsigned int attributeIndex) const
{
	bind();
	GLDEBUG(glDisableVertexAttribArray(attributeIndex));
}

void VertexArray::linkIndexBuffer(const IndexBuffer& indexbuffer)
{
	bind();
	indexbuffer.bind();
	m_UsesIndexBuffer = true;
	unbind();
	indexbuffer.unbind();
}

void VertexArray::unlinkIndexBuffer()
{
	bind();
	GLDEBUG(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	m_UsesIndexBuffer = false;
	unbind();
}