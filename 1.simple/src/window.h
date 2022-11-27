#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <QOpenGLWindow>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>

class Window : public QOpenGLWindow
{
	Q_OBJECT

public:
	Window();
	~Window();

protected:
	void initializeGL() override;
	void resizeGL(int w, int h) override;
	void paintGL() override;

private:
	void initializeGeometry();
	void initializeShaders();
	void initializeTextures();

private:
	QOpenGLFunctions_3_3_Core* m_funcs{ nullptr };
	QOpenGLBuffer m_vbo{ QOpenGLBuffer::VertexBuffer };
	QOpenGLBuffer m_ibo{ QOpenGLBuffer::IndexBuffer };
	QOpenGLVertexArrayObject m_vao;
	std::unique_ptr<QOpenGLShaderProgram> m_program;
	GLuint m_texture{ 0 };
};

#endif //_WINDOW_H_