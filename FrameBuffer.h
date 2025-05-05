#pragma once
#include "helper/Texture.h"

#include <GLFW/glfw3.h>

#include <array>
#include <optional>
#include <memory>

namespace opengl {

	class FrameBuffer {
	public:
		FrameBuffer(int w, int h, int internalFormat, unsigned int format, unsigned int type);
		FrameBuffer(int w, int h);

		FrameBuffer(const FrameBuffer &rhs) = delete;
		FrameBuffer &operator=(const FrameBuffer &rhs) = delete;

		FrameBuffer(FrameBuffer &&rhs) noexcept;
		FrameBuffer &operator=(FrameBuffer &&rhs) noexcept;

		~FrameBuffer();

		void Resize(int w, int h);

		void StartFill();
		void EndFill();

		int GetWidth();
		int GetHeigth();

		std::optional<std::array<unsigned char, 4>> GetPixel(int x, int y) const noexcept;

		// void Bitblt(int w, int h);

		GLuint GetFBO();
		Texture &GetTexture();

	protected:
		int width, height;
		std::unique_ptr<Texture> tex;
		GLint originFBO;
		GLuint fbo;
		unsigned int rbo;
		FrameBuffer() = default;

		void SwitchToSelf();
		virtual void SwitchToOrigin();

		void BackupCurrentFrameBuffer() noexcept;
	};

} // namespace opengl