#pragma once

#include "FrameBuffer.h"

namespace opengl {

class DepthFrameBuffer : public FrameBuffer {
public:
    DepthFrameBuffer(int width = 1024, int height = 1024);
};
} // namespace opengl