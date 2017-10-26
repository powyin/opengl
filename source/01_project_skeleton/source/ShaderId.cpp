#include "ShaderId.h"



	Pow::Pow()
	{
		_position = -1;
		_uv = -1;
		_texture = -1;
		_MVP = -1;
	}
	Pow::~Pow()
	{
	}

	/// 初始化函数
    bool   Pow:: initialize()
	{
		const char* vs =
		{
			"#version 150 \n"
			"uniform   mat4 _MVP; \n"
			"in vec2 _position; \n"
			"in vec2 _uv;"
			"out vec2 _outUV;"
			"void main()"
			"{"
			"   vec4    pos =   vec4(_position,0,1);"
			"   _outUV      =   _uv;"
			"   gl_Position =   _MVP * pos;"
			"}"
		};
		const char* ps =
		{
			"#version 150 \n"
			"uniform sampler2D tex; \n"
			"in   vec2  _outUV; \n"
			"out vec4 finalColor;"
			"void main()"
			"{"
			"   vec4    tColor0  =   texture(tex,_outUV);\n"
			"   finalColor    =   tColor0;\n"
			"}"
		};

		bool    res = createProgram(vs, ps);
		if (res)
		{
			_position = glGetAttribLocation(_programId, "_position");
			_uv = glGetAttribLocation(_programId, "_uv");
			_texture = glGetUniformLocation(_programId, "_texture");
			_MVP = glGetUniformLocation(_programId, "_MVP");

		}
		return  res;
	}

	/**
	*   使用程序
	*/
    void    Pow::begin()
	{
		glUseProgram(_programId);
		glEnableVertexAttribArray(_position);
		glEnableVertexAttribArray(_uv);

	}
	/**
	*   使用完成
	*/
	 void    Pow::end()
	{
		glDisableVertexAttribArray(_position);
		glDisableVertexAttribArray(_uv);
		glUseProgram(0);
	}









