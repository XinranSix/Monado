// #include "hzpch.h"
// #include "OpenGLUniformBuffer.h"

// #include <glad/glad.h>

// namespace Hazel
// {
// 	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size, uint32_t binding)
// 	{
// 		// OpenGL里的uniform也是一种buffer, 只不过类型为GL_UNIFORM_BUFFER
// 		glCreateBuffers(1, &m_BufferID);
// 		glNamedBufferData(m_BufferID, size, nullptr, GL_DYNAMIC_DRAW); // TODO: investigate usage hint
// 		glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_BufferID);
// 	}

// 	OpenGLUniformBuffer::~OpenGLUniformBuffer()
// 	{
// 		glDeleteBuffers(1, &m_BufferID);
// 	}

// 	void OpenGLUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
// 	{
// 		glNamedBufferSubData(m_BufferID, offset, size, data);
// 	}
// }
