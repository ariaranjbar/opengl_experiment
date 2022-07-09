#pragma once
#include "renderutils.h"
#include "bufferobject.h"
class IndexBuffer :
    public BufferObject
{
public:
    inline IndexBuffer() : BufferObject(GL_ELEMENT_ARRAY_BUFFER){};
};

