#pragma once
#include <string>
#include "glad/glad.h"
#include "glad/glad.h"
#include <iostream>
#include <vector>
#include <signal.h>

#define ASSERT(x) if (!(x)) __debugbreak();
#ifdef _DEBUG

#define GLDEBUG(x) GLClearError(); x; ASSERT(GLReport(true, #x, __LINE__ , __FILE__))

#else 

#define GLDEBUG(x) x

#endif // DEBUG


unsigned int GLParseError(const std::string& hexstring);

bool readfile(const std::string& filename, std::string& content);

void GLClearError();

bool GLReport(bool infile, const char* funcname, unsigned int linenum, const char* filename);

std::string GLDecode(int code, std::string filename);

std::vector<float> gaussian(int count, float roughness, bool normalize = false);

std::vector<float> ggx(int count, float roughness, bool normalize = false);