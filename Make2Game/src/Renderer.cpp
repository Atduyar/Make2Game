#include "Renderer.h"
#include <iostream>

#include "GL/glew.h"

#include "IndexBuffer.h"
#include "Log.h"
#include "Shader.h"
#include "VertexArray.h"

void GLClearError() {
    while (glGetError() != GL_NO_ERROR);
}
bool GLLogCall(const char* func, int line) {
    bool errCheck = true;
    while (GLenum err = glGetError()) {
        std::cout << "[GL ERROR] (" << err << ")" << std::endl;
        errCheck = false;
    }
    return errCheck;
}

void GLAPIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei length,
    const GLchar* msg, const void* data)
{
    GLErrorInfo gl_error_info;

    switch (source) {
    case GL_DEBUG_SOURCE_API:
        gl_error_info.source = "API";
        break;

    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        gl_error_info.source = "WINDOW SYSTEM";
        break;

    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        gl_error_info.source = "SHADER COMPILER";
        break;

    case GL_DEBUG_SOURCE_THIRD_PARTY:
        gl_error_info.source = "THIRD PARTY";
        break;

    case GL_DEBUG_SOURCE_APPLICATION:
        gl_error_info.source = "APPLICATION";
        break;

    case GL_DEBUG_SOURCE_OTHER:
        gl_error_info.source = "UNKNOWN";
        break;

    default:
        gl_error_info.source = "UNKNOWN";
        break;
    }
    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        gl_error_info.type = "ERROR";
        break;

    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        gl_error_info.type = "DEPRECATED BEHAVIOR";
        break;

    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        gl_error_info.type = "UDEFINED BEHAVIOR";
        break;

    case GL_DEBUG_TYPE_PORTABILITY:
        gl_error_info.type = "PORTABILITY";
        break;

    case GL_DEBUG_TYPE_PERFORMANCE:
        gl_error_info.type = "PERFORMANCE";
        break;

    case GL_DEBUG_TYPE_OTHER:
        gl_error_info.type = "OTHER";
        break;

    case GL_DEBUG_TYPE_MARKER:
        gl_error_info.type = "MARKER";
        break;

    default:
        gl_error_info.type = "UNKNOWN";
        break;
    }
    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        gl_error_info.severity = "HIGH";
        break;

    case GL_DEBUG_SEVERITY_MEDIUM:
        gl_error_info.severity = "MEDIUM";
        break;

    case GL_DEBUG_SEVERITY_LOW:
        gl_error_info.severity = "LOW";
        break;

    case GL_DEBUG_SEVERITY_NOTIFICATION:
        gl_error_info.severity = "NOTIFICATION";
        break;

    default:
        gl_error_info.severity = "UNKNOWN";
        break;
    }

    gl_error_info.msg = msg;
    gl_error_info.time = ImGui::GetTime();
    Log::GetInstance().AddLogGlError(gl_error_info);
}
void GLErrorHandler() {
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(GLDebugMessageCallback, NULL);
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
    shader.Bind();
    va.Bind();
    ib.Bind();
    glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr);
}

void Renderer::Clear()
{
    glClear(GL_COLOR_BUFFER_BIT);
}
