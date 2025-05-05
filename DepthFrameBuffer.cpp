#include <glad/glad.h>

#include "DepthFrameBuffer.h"

#include "helper/glutils.h"

#include <cassert>

namespace opengl {

	DepthFrameBuffer::DepthFrameBuffer(int width, int height) {
		this->width = width;
		this->height = height;

		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &originFBO);

		glGenFramebuffers(1, &fbo);

		{
			GLuint textureId;
			glGenTextures(1, &textureId);
			glBindTexture(GL_TEXTURE_2D, textureId);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

			tex = std::make_unique<Texture>();
			tex->generate(textureId, GL_TEXTURE_2D);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex->GetId(), 0);

		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

		glClear(GL_DEPTH_BUFFER_BIT);

		glBindFramebuffer(GL_FRAMEBUFFER, originFBO);
		GLUtils::checkForOpenGLError(__FILE__, __LINE__);
	}

} // namespace opengl