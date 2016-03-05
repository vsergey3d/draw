#pragma once

#include <gmock/gmock.h>
#include <GL/glew.h>

class GLMock {
public:
    MOCK_METHOD0(glew_Init, GLenum ());
    MOCK_METHOD1(glew_IsSupported, GLboolean (const char * name));
    MOCK_METHOD0(gl_GetError, GLenum ());
    MOCK_METHOD2(gl_GenBuffers, void  (GLsizei n, GLuint * buffers));
    MOCK_METHOD2(gl_DeleteBuffers, void  (GLsizei n, const GLuint * buffers));
    MOCK_METHOD2(gl_BindBuffer, void  (GLenum target, GLuint buffer));
    MOCK_METHOD4(gl_BufferData, void  (GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage));
    MOCK_METHOD2(gl_GenTextures, void (GLsizei n, GLuint * textures));
    MOCK_METHOD2(gl_BindTexture, void (GLenum target, GLuint texture));
    MOCK_METHOD3(gl_TexParameteri, void (GLenum target, GLenum pname, GLint param));
    MOCK_METHOD2(gl_PixelStorei, void (GLenum pname, GLint param));
    MOCK_METHOD9(gl_TexImage2D, void (GLenum target, GLint level, GLint internalformat,
            GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid * pixels));
    MOCK_METHOD1(gl_CompileShader, void  (GLuint shader));
    MOCK_METHOD4(gl_ShaderSource, void  (GLuint shader, GLsizei count,
            const GLchar ** strings, const GLint * lengths));
    MOCK_METHOD0(gl_CreateProgram, unsigned int  ());
    MOCK_METHOD1(gl_DeleteProgram, void  (GLuint program));
    MOCK_METHOD3(gl_GetProgramiv, void  (GLuint program, GLenum pname, GLint * param));
    MOCK_METHOD2(gl_GetUniformLocation, int  (GLuint program, const GLchar * name));
    MOCK_METHOD2(gl_GetAttribLocation, int  (GLuint program, const GLchar * name));
    MOCK_METHOD1(gl_LinkProgram, void  (GLuint program));
    MOCK_METHOD1(gl_CreateShader, unsigned int  (GLenum type));
    MOCK_METHOD2(gl_AttachShader, void  (GLuint program, GLuint shader));
    MOCK_METHOD1(gl_DeleteShader, void  (GLuint shader));
    MOCK_METHOD3(gl_GetShaderiv, void  (GLuint shader, GLenum pname, GLint * param));
    MOCK_METHOD2(gl_DetachShader, void  (GLuint program, GLuint shader));
    MOCK_METHOD1(gl_Disable, void (GLenum cap));
    MOCK_METHOD5(gl_DrawElementsInstanced, void  (GLenum arg0, GLsizei arg1, GLenum arg2,
            const GLvoid * arg3, GLsizei arg4));
    MOCK_METHOD2(gl_DeleteTextures, void (GLsizei n, const GLuint * textures));
    MOCK_METHOD4(gl_BufferSubData, void  (GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid * data));
    MOCK_METHOD1(gl_DepthMask, void (GLboolean flag));
    MOCK_METHOD1(gl_Enable, void (GLenum cap));
    MOCK_METHOD1(gl_BlendEquation, void  (GLenum mode));
    MOCK_METHOD2(gl_BlendFunc, void (GLenum sfactor, GLenum dfactor));
    MOCK_METHOD1(gl_UseProgram, void  (GLuint program));
    MOCK_METHOD3(gl_Uniform2fv, void  (GLint location, GLsizei count, const GLfloat * value));
    MOCK_METHOD1(gl_ActiveTexture, void  (GLenum texture));
    MOCK_METHOD2(gl_Uniform1i, void  (GLint location, GLint v0));
    MOCK_METHOD1(gl_EnableVertexAttribArray, void  (GLuint arg0));
    MOCK_METHOD6(gl_VertexAttribPointer, void  (GLuint index, GLint size, GLenum type,
            GLboolean normalized, GLsizei stride, const GLvoid * pointer));
    MOCK_METHOD2(gl_VertexAttribDivisor, void  (GLuint index, GLuint divisor));
    MOCK_METHOD4(gl_Viewport, void (GLint x, GLint y, GLsizei width, GLsizei height));
    MOCK_METHOD1(gl_Clear, void (GLbitfield mask));
    MOCK_METHOD4(gl_ClearColor, void (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha));
    MOCK_METHOD1(gl_ClearDepth, void (GLclampd depth));
};

inline GLMock& glMocked() {
    
    static ::testing::NiceMock<GLMock> glMock;
    return glMock; 
}

#undef glGenBuffers
#define glGenBuffers glMocked().gl_GenBuffers
#undef glDeleteBuffers
#define glDeleteBuffers glMocked().gl_DeleteBuffers
#undef glBindBuffer
#define glBindBuffer glMocked().gl_BindBuffer
#undef glBufferData
#define glBufferData glMocked().gl_BufferData
#undef glGenTextures
#define glGenTextures glMocked().gl_GenTextures
#undef glBindTexture
#define glBindTexture glMocked().gl_BindTexture
#undef glTexParameteri
#define glTexParameteri glMocked().gl_TexParameteri
#undef glPixelStorei
#define glPixelStorei glMocked().gl_PixelStorei
#undef glTexImage2D
#define glTexImage2D glMocked().gl_TexImage2D
#undef glCreateShader
#define glCreateShader glMocked().gl_CreateShader
#undef glShaderSource
#define glShaderSource glMocked().gl_ShaderSource
#undef glCompileShader
#define glCompileShader glMocked().gl_CompileShader
#undef glGetShaderiv
#define glGetShaderiv glMocked().gl_GetShaderiv
#undef glCreateProgram
#define glCreateProgram glMocked().gl_CreateProgram
#undef glAttachShader
#define glAttachShader glMocked().gl_AttachShader
#undef glLinkProgram
#define glLinkProgram glMocked().gl_LinkProgram
#undef glGetProgramiv
#define glGetProgramiv glMocked().gl_GetProgramiv
#undef glGetUniformLocation
#define glGetUniformLocation glMocked().gl_GetUniformLocation
#undef glGetAttribLocation
#define glGetAttribLocation glMocked().gl_GetAttribLocation
#undef glDeleteShader
#define glDeleteShader glMocked().gl_DeleteShader
#undef glDeleteProgram
#define glDeleteProgram glMocked().gl_DeleteProgram
#undef glGetError
#define glGetError glMocked().gl_GetError
#undef glewInit
#define glewInit glMocked().glew_Init
#undef glewIsSupported
#define glewIsSupported glMocked().glew_IsSupported
#undef glDisable
#define glDisable glMocked().gl_Disable
#undef glDeleteTextures
#define glDeleteTextures glMocked().gl_DeleteTextures
#undef glDisable
#define glDisable glMocked().gl_Disable
#undef glDrawElementsInstanced
#define glDrawElementsInstanced glMocked().gl_DrawElementsInstanced
#undef glBufferSubData
#define glBufferSubData glMocked().gl_BufferSubData
#undef glDepthMask
#define glDepthMask glMocked().gl_DepthMask
#undef glEnable
#define glEnable glMocked().gl_Enable
#undef glBlendEquation
#define glBlendEquation glMocked().gl_BlendEquation
#undef glBlendFunc
#define glBlendFunc glMocked().gl_BlendFunc
#undef glUseProgram
#define glUseProgram glMocked().gl_UseProgram
#undef glUniform2fv
#define glUniform2fv glMocked().gl_Uniform2fv
#undef glActiveTexture
#define glActiveTexture glMocked().gl_ActiveTexture
#undef glUniform1i
#define glUniform1i glMocked().gl_Uniform1i
#undef glEnableVertexAttribArray
#define glEnableVertexAttribArray glMocked().gl_EnableVertexAttribArray
#undef glVertexAttribPointer
#define glVertexAttribPointer glMocked().gl_VertexAttribPointer
#undef glVertexAttribDivisor
#define glVertexAttribDivisor glMocked().gl_VertexAttribDivisor
#undef glViewport
#define glViewport glMocked().gl_Viewport
#undef glClear
#define glClear glMocked().gl_Clear
#undef glClearColor
#define glClearColor glMocked().gl_ClearColor
#undef glClearDepth
#define glClearDepth glMocked().gl_ClearDepth
