#pragma once
#include <string>
#include <GL\glew.h>

namespace draw {
namespace gl {

void setCurrentContext();

bool checkError();
bool checkShader(GLuint shader, std::string& log);
bool checkProgram(GLuint program, std::string& log);

} // namespace gl
} // namespace draw
