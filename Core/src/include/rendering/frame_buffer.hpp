#pragma once

#include "rendering_typedef.h"

BEGIN_PCCORE

class FrameBuffer
{
public:

    FrameBuffer();
    

private:
    FrameBufferHandle m_FrameBufferHandle;
};

END_PCCORE