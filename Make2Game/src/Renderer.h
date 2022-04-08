#pragma once

#include <GL/glew.h>

#include "IndexBuffer.h"
#include "Shader.h"
#include "VertexArray.h"


#define ASSERT(x)   if(!(x)) __debugbreak();

#define OLDGLCall(x)   GLClearError();\
                    x;\
                    ASSERT(GLLogCall(__func__, __LINE__))

#define DD          GLLogCall(__func__, __LINE__);\
                    GLClearError();


void GLClearError();
bool GLLogCall(const char* func, int line);
void GLErrorHandler();

struct GLErrorInfo
{
public:
    std::string source;
    std::string type;
    std::string severity;
    std::string msg;
    double time;
};

class Renderer
{
public:
    void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
    void Clear();
};