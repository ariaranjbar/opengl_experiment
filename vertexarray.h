#pragma once
#include <vector>

#include "renderutils.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"


class VertexArray
{
private:
	unsigned int m_ID;
	bool m_UsesIndexBuffer;
public:
	inline VertexArray(): m_UsesIndexBuffer(false){ GLDEBUG(glGenVertexArrays(1, &m_ID)); };
	inline void bind() const { GLDEBUG(glBindVertexArray(m_ID)); };
	inline void unbind() const { GLDEBUG(glBindVertexArray(0)); };
	inline bool usesIndexBuffer() const { return m_UsesIndexBuffer; };
	void linkVertexBuffer(const VertexBuffer& vertexbuffer);
	void enableAttribute(unsigned int attributeIndex) const;
	void disableAttribute(unsigned int attributeIndex) const;
	void linkIndexBuffer(const IndexBuffer& indexbuffer);
	void unlinkIndexBuffer();
	inline ~VertexArray() { unbind(); GLDEBUG(glDeleteVertexArrays(1, &m_ID)); };
};

