#include <glad/glad.h>

#include "helper/glutils.h"

#include "FrameBuffer.h"


#include <stdexcept>
#include <assert.h>
#include <vector>
#include <algorithm>

using namespace std;

namespace opengl {

	void OPENGL_CHECK_ERROR() {
		GLUtils::checkForOpenGLError(__FILE__, __LINE__);
	}

	FrameBuffer::FrameBuffer(int w, int h, int internalFormat, unsigned int format, unsigned int type)
		: width(w), height(h), tex(std::make_unique<Texture>()), originFBO(-1) {
		tex->generate(nullptr, w, h);

		BackupCurrentFrameBuffer();

		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex->GetId(), 0);

		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);

		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);

		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

		assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

		glBindFramebuffer(GL_FRAMEBUFFER, originFBO);
		OPENGL_CHECK_ERROR();
	}

	FrameBuffer::FrameBuffer(int w, int h) : FrameBuffer(w, h, GL_RGBA16F, GL_RGBA, GL_FLOAT) {}

	FrameBuffer::FrameBuffer(FrameBuffer &&rhs) noexcept {
		this->operator=(std::move(rhs));
	}

	FrameBuffer &FrameBuffer::operator=(FrameBuffer &&rhs) noexcept {
		if (this != &rhs) {
			this->~FrameBuffer();
			this->width = rhs.width;
			this->height = rhs.height;
			this->tex = std::move(rhs.tex);
			this->originFBO = rhs.originFBO;
			this->fbo = rhs.fbo;
			this->rbo = rhs.rbo;

			rhs.width = -1;
			rhs.height = -1;
			rhs.tex = nullptr;
			rhs.originFBO = -1;
			rhs.fbo = 0;
			rhs.rbo = 0;
		}
		return *this;
	}

	FrameBuffer::~FrameBuffer() {
		glDeleteFramebuffers(1, &fbo);
		OPENGL_CHECK_ERROR();
		glDeleteRenderbuffers(1, &rbo);
		OPENGL_CHECK_ERROR();
	}

	void FrameBuffer::Resize(int w, int h) {
		this->~FrameBuffer();
		this->FrameBuffer::FrameBuffer(w, h);
		width = w;
		height = h;
	}

	void FrameBuffer::StartFill() {
		BackupCurrentFrameBuffer();

		SwitchToSelf();
		glViewport(0, 0, width, height);
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void FrameBuffer::EndFill() {
		SwitchToOrigin();
	}

	void FrameBuffer::SwitchToOrigin() {
		glBindFramebuffer(GL_FRAMEBUFFER, originFBO);
		OPENGL_CHECK_ERROR();
	}

	void FrameBuffer::SwitchToSelf() {
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		OPENGL_CHECK_ERROR();
	}

	int FrameBuffer::GetWidth() {
		return width;
	}

	int FrameBuffer::GetHeigth() {
		return height;
	}

	std::optional<std::array<unsigned char, 4>> FrameBuffer::GetPixel(int x, int y) const noexcept {
		if (x < 0 || x >= width || y < 0 || y >= height) {
			return {};
		}

		FrameBuffer *mutThis = const_cast<FrameBuffer *>(this);
		mutThis->BackupCurrentFrameBuffer();
		mutThis->SwitchToSelf();

		std::array<unsigned char, 4> ret;
		glReadPixels(x, height - y - 1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, ret.data());
		OPENGL_CHECK_ERROR();

		int value = *reinterpret_cast<const int *>(ret.data());
		assert(value != 0xCCCCCCCC);

		mutThis->SwitchToOrigin();

		return ret;
	}

	// void TFrameBuffer::Bitblt(int w, int h)
	//{
	//	width = w; height = h;
	//	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
	//	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	//	glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	// }

	GLuint FrameBuffer::GetFBO() {
		return fbo;
	}

	Texture &FrameBuffer::GetTexture() {
		return *tex;
	}

	void FrameBuffer::BackupCurrentFrameBuffer() noexcept {
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &originFBO);
		OPENGL_CHECK_ERROR();
	}

} // namespace opengl