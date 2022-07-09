#pragma once
#include "renderutils.h"

class BufferObject
{
public:
	inline BufferObject(uint16_t type) : m_Type(type), m_Count(0) { GLDEBUG(glGenBuffers(1, &m_ID)); };
	void bind() const;
	void unbind() const;
	template<typename T>
	void data(unsigned int count, const void* data, uint16_t hint)
	{
		bind();
		m_Size = count * sizeof(T);
		m_Count = count;
		GLDEBUG(glBufferData(m_Type, m_Size, data, hint));  //add bound buffer tracking to a renderer class and reset the binding to the previously bound object instead of unbind
		unbind();
	};
	inline const size_t bufferSize() const { return m_Size; }
	inline const size_t objectCount() const { return m_Count; }
	inline unsigned int type() { return m_Type; };
	inline ~BufferObject() { unbind(); GLDEBUG(glDeleteBuffers(1, &m_ID)); };
protected:
	unsigned int m_ID;
	unsigned int m_Count;
	unsigned int m_Size;
	unsigned int m_Type;
};