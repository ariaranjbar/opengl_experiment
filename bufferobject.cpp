#include "bufferobject.h"

void BufferObject::bind() const
{
	GLDEBUG(glBindBuffer(m_Type, m_ID));
}

void BufferObject::unbind() const
{
	GLDEBUG(glBindBuffer(m_Type, 0));
}