#include "gl.h"
#include "draw.h"
#include <error.h>

namespace draw {

static Context* context(Context* context = nullptr) {

	static Context* ctxt = context;
	return ctxt;
}

void init(Context& context) {

	draw::context(&context)->setCurrent();

	if (glewInit() != GLEW_OK ||
		!glewIsSupported("GL_VERSION_2_0  GL_ARB_draw_instanced"))
		throw error(Code::UnsupportedOpenGLFeature);

	glDisable(GL_DITHER);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glDepthMask(GL_FALSE);
	glDisable(GL_BLEND);

	ASSERT(gl::checkError());
}

namespace gl {

void setCurrentContext() {

	auto ctxt = context();
	ASSERT(ctxt != nullptr);
	ctxt->setCurrent();
}

bool checkError() {

	return glGetError() == GL_NO_ERROR;
}

bool checkShader(GLuint shader, std::string& log) {

	GLint param = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &param);
	if (param != GL_TRUE) {
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &param);
		if (param > 0) {
			log.resize((size_t)param);
			glGetShaderInfoLog(shader, log.length(), &param, (GLchar*)log.data());
		} else {
			log = "unknown shader compilation error";
		}
		return false;
	}
	return true;
}

bool checkProgram(GLuint program, std::string& log) {

	GLint param = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &param);
	if (param != GL_TRUE) {
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &param);
		if (param > 0) {
			log.resize((size_t)param);
			glGetProgramInfoLog(program, log.length(), &param, (GLchar*)log.data());
		} else {
			log = "unknown program linking error";
		}
		return false;
	}
	return true;
}

} // namespace gl
} // namespace draw
