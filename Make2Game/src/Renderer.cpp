#include "Renderer.h"
#include <iostream>
#include <sstream>

#include "GL/glew.h"

#include "IndexBuffer.h"
#include "Log.h"
#include "Shader.h"
#include "Texture.h"
#include "TextureAtlases.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"

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

void GLAPIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* msg, const void* data)
{
    GLErrorInfo gl_error_info;
    bool prterr = false;

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
        prterr = true;
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

    if(prterr)
    {
        std::cout << "HIGH "<< gl_error_info.type << " ---" << msg << std::endl;
        std::cout.flush();
    }
    gl_error_info.msg = msg;
    gl_error_info.time = ImGui::GetTime();
    Log::GetInstance().AddLogGlError(gl_error_info);
}
void GLErrorHandler() {
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(GLDebugMessageCallback, NULL);
}


// Start Batch Renderer

static const size_t MaxQuadCount = 10000;
static const size_t MaxVertexCount = MaxQuadCount * 4;
static const size_t MaxIndexCount = MaxQuadCount * 6;

struct BatchRendererData
{
    VertexArray*  VA;
    VertexBuffer* VB;
    IndexBuffer*  IB;

    Vertex* QuadBuffer;
    Vertex* QuadBufferPtr;

    uint32_t IndexCount = 0;

    uint32_t TextureSlotIndex = 0;

    uint32_t QuadCount = 0;
    uint32_t DrawCall = 0;
};

static BatchRendererData s_Data;

BatchRenderer::BatchRenderer()
{
    s_Data.QuadBuffer = new Vertex[MaxVertexCount];

    s_Data.VA = new VertexArray();
    s_Data.VB = new VertexBuffer(nullptr, MaxVertexCount * sizeof(Vertex));

    VertexBufferLayout layout;
    layout.Push<float>(2);
    layout.Push<float>(2);
    s_Data.VA->AddBuffer(*s_Data.VB, layout);

    uint32_t indices[MaxIndexCount];
    uint32_t offset = 0;

    for (size_t i = 0; i < MaxIndexCount; i += 6)
    {
        indices[i + 0] = 0 + offset;
        indices[i + 1] = 1 + offset;
        indices[i + 2] = 2 + offset;

        indices[i + 3] = 2 + offset;
        indices[i + 4] = 1 + offset;
        indices[i + 5] = 3 + offset;

        offset += 4;
    }

    s_Data.IB = new IndexBuffer(indices, MaxIndexCount);
    
}

BatchRenderer::~BatchRenderer()
{
    delete s_Data.VA;
    delete s_Data.VB;
    delete s_Data.IB;

    delete[] s_Data.QuadBuffer;
}

void BatchRenderer::BeginBatch()
{
    s_Data.QuadBufferPtr = s_Data.QuadBuffer;
}

void BatchRenderer::EndBatch()
{
    GLsizeiptr size = (uint8_t*)s_Data.QuadBufferPtr - (uint8_t*)s_Data.QuadBuffer;
    s_Data.VB->Bind();

    //??????
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, s_Data.QuadBuffer);
	//??????
}

void BatchRenderer::Draw()
{
    s_Data.VA->Bind();
    s_Data.IB->Bind();

    glDrawElements(GL_TRIANGLES, s_Data.IndexCount, GL_UNSIGNED_INT, nullptr);

    s_Data.IndexCount = 0;
    s_Data.DrawCall++;
}

void BatchRenderer::DrawQuad(const glm::vec2& positoin, const glm::vec2& size, SubTexture subTexture)
{
    if(s_Data.IndexCount >= MaxIndexCount)
    {
        EndBatch();
        Draw();
        BeginBatch();
    }
    /*
		float x = 16.0f/160.0f*0;
		float y = 16.0f/160.0f*9;
		float x = 0.0f;
		float y = 0.0f;
		float w = 16.0f/160.0f;
		float h = 16.0f/160.0f;

	    float positions[]{
	         50.0f,  00.0f,  x+w,    y, //sag alt
	    	 00.0f,  00.0f,    x,    y, //sol alt
	         50.0f,  50.0f,  x+w,  y+h, //sag üst
	         00.0f,  50.0f,    x,  y+h //sol üst
	    };

	    uint32_t indices[] = {
			0, 1, 2,
			1, 2, 3
	    };
     */
    s_Data.TextureSlotIndex = 0.0f;

    s_Data.QuadBufferPtr->positoin =    { positoin.x + size.x   , positoin.y };
    s_Data.QuadBufferPtr->imageCoord =  subTexture.imageCoord[0];
    s_Data.QuadBufferPtr++;

    s_Data.QuadBufferPtr->positoin =    { positoin.x            , positoin.y };
    s_Data.QuadBufferPtr->imageCoord =  subTexture.imageCoord[1];
    s_Data.QuadBufferPtr++;

    s_Data.QuadBufferPtr->positoin =    { positoin.x + size.x   , positoin.y + size.y };
    s_Data.QuadBufferPtr->imageCoord =  subTexture.imageCoord[2];
    s_Data.QuadBufferPtr++;

    s_Data.QuadBufferPtr->positoin =    { positoin.x            , positoin.y + size.y };
    s_Data.QuadBufferPtr->imageCoord =  subTexture.imageCoord[3];
    s_Data.QuadBufferPtr++;

    s_Data.IndexCount += 6;

    s_Data.QuadCount++;
}

void BatchRenderer::DrawStat()
{
    ImGui::Text("Quad Count = %d", s_Data.QuadCount);
    ImGui::Text("Draw Call = %d", s_Data.DrawCall);
    s_Data.QuadCount = 0;
    s_Data.DrawCall = 0;
}

// End Batch Renderer




// Start Renderer
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
// End Renderer
