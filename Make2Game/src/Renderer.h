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


class SubTexture;
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

struct Vertex
{
    glm::vec2 positoin;
    glm::vec2 imageCoord;
};

class Renderer
{
public:
    void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
    void Clear();
};


struct BatchRenderer
{
public:
    BatchRenderer(); // init
    ~BatchRenderer();// shutdown

    void BeginBatch();
    void EndBatch();
    void Draw();
    void DrawQuad(const glm::vec2& positoin, const glm::vec2& size, SubTexture subTexture);

    void DrawStat();
};