#include "renderer.h"

#include "STB/stb_image_write.h"

#include <sstream>

void Renderer::setViewport(unsigned int width, unsigned int height)
{
	GLDEBUG(glViewport(0, 0, width, height));
	m_viewportWidth = width;
	m_viewportHeight = height;
}

void Renderer::clear() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	GLDEBUG(glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT));
}

void Renderer::clear(const FramebufferMS& framebuffer) const
{
	framebuffer.bind();
	GLDEBUG(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void Renderer::clear(const Framebuffer& framebuffer) const
{
	framebuffer.bind();
	GLDEBUG(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void Renderer::clearColor(glm::vec4 color)
{
	m_ClearColor = color;
	GLDEBUG(glClearColor(color.r, color.g, color.b, color.a));
}

void Renderer::clearColor(glm::vec3 color)
{
	m_ClearColor = glm::vec4(color, 1.0);
	GLDEBUG(glClearColor(color.r, color.g, color.b, 1.0f));
}

void Renderer::clearDepth() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	GLDEBUG(glClear(GL_DEPTH_BUFFER_BIT));
}

void Renderer::clearDepth(const FramebufferMS& framebuffer) const
{
	framebuffer.bind();
	GLDEBUG(glClear(GL_DEPTH_BUFFER_BIT));
}

void Renderer::clearDepth(const Framebuffer& framebuffer) const
{
	framebuffer.bind();
	GLDEBUG(glClear(GL_DEPTH_BUFFER_BIT));
}

void Renderer::drawTris (VertexArray& VAO, IndexBuffer& EBO, ShaderProgram& program, Camera& camera) const
{
	camera.framebuffer().bind();
	bool svp = false;
	if ((m_viewportHeight != camera.height()) || (m_viewportWidth != camera.width()))
	{
		GLDEBUG(glViewport(0, 0, camera.width(), camera.height()));
		svp = true;
	}
	VAO.linkIndexBuffer(EBO);
	VAO.bind();
	program.setUniform("u_cameraMatrix", camera.matrix());
	program.setUniform("u_cameraPos", camera.position());
	program.setUniform("u_offset", camera.offset());
	program.use();
	GLDEBUG(glDrawElements(GL_TRIANGLES, EBO.objectCount(), GL_UNSIGNED_INT, 0));
	if (svp)
	{
		GLDEBUG(glViewport(0, 0, m_viewportWidth, m_viewportHeight));
	}
}

void Renderer::drawTris(VertexArray& VAO, IndexBuffer& EBO, ShaderProgram& program, Camera& camera, std::vector<unsigned int> framebuffers) const
{
		bool svp = false;
		if ((m_viewportHeight != camera.framebuffer().height()) || (m_viewportWidth != camera.framebuffer().width()))
		{
			GLDEBUG(glViewport(0, 0, camera.width(), camera.height()));
			svp = true;
		}
		VAO.linkIndexBuffer(EBO);
		VAO.bind();
		program.setUniform("u_cameraMatrix", camera.matrix());
		program.setUniform("u_cameraPos", camera.position());
		float x, y;
		int rootOfSize;
		rootOfSize = static_cast<int>(glm::ceil(glm::sqrt(framebuffers.size())));
		for (auto i : framebuffers)
		{
			camera.framebuffer(i).bind();
			x = i % rootOfSize;
			x -= rootOfSize / 2.0F;
			y = i / rootOfSize;
			y -= rootOfSize / 2.0F;
			program.setUniform("u_offset", glm::vec2(x * (1.0f / (rootOfSize)) / camera.width(), y * (1.0f / (rootOfSize)) / camera.height()));
			program.use();
			GLDEBUG(glDrawElements(GL_TRIANGLES, EBO.objectCount(), GL_UNSIGNED_INT, 0));
		}
		if (svp)
		{
			GLDEBUG(glViewport(0, 0, m_viewportWidth, m_viewportHeight));
		}
	
}

void Renderer::drawLines(VertexArray& VAO, IndexBuffer& EBO, ShaderProgram& program, Camera& camera) const
{
	camera.framebuffer().bind();
	bool svp = false;
	if ((m_viewportHeight != camera.framebuffer().height()) || (m_viewportWidth != camera.framebuffer().width()))
	{
		GLDEBUG(glViewport(0, 0, camera.width(), camera.height()));
		svp = true;
	}
	clearDepth(camera.framebuffer());
	VAO.linkIndexBuffer(EBO);
	VAO.bind();
	program.setUniform("u_cameraMatrix", camera.matrix());
	program.setUniform("u_cameraPos", camera.position());
	program.setUniform("u_offset", camera.offset());
	program.use();
	GLDEBUG(glDrawElements(GL_LINES, EBO.objectCount(), GL_UNSIGNED_INT, 0));
	if (svp)
	{
		GLDEBUG(glViewport(0, 0, m_viewportWidth, m_viewportHeight));
	}
}

void Renderer::drawLines(VertexArray& VAO, IndexBuffer& EBO, ShaderProgram& program, Camera& camera, std::vector<unsigned int> framebuffers) const
{
	bool svp = false;
	if ((m_viewportHeight != camera.framebuffer().height()) || (m_viewportWidth != camera.framebuffer().width()))
	{
		GLDEBUG(glViewport(0, 0, camera.width(), camera.height()));
		svp = true;
	}
	clearDepth(camera.framebuffer());
	VAO.linkIndexBuffer(EBO);
	VAO.bind();
	program.setUniform("u_cameraMatrix", camera.matrix());
	program.setUniform("u_cameraPos", camera.position());
	float x, y;
	int rootOfSize;
	rootOfSize = static_cast<int>(glm::ceil(glm::sqrt(framebuffers.size())));
	for (auto i : framebuffers)
	{
		camera.framebuffer(i).bind();
		x = i % rootOfSize;
		x -= rootOfSize / 2.0F;
		y = i / rootOfSize;
		y -= rootOfSize / 2.0F;
		program.setUniform("u_offset", glm::vec2(x * (1.0f / (rootOfSize)) / camera.width(), y * (1.0f / (rootOfSize)) / camera.height()));
		program.use();
		GLDEBUG(glDrawElements(GL_LINES, EBO.objectCount(), GL_UNSIGNED_INT, 0));
	}
	if (svp)
	{
		GLDEBUG(glViewport(0, 0, m_viewportWidth, m_viewportHeight));
	}
}

void Renderer::drawShader(ShaderProgram& program)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	clearDepth();
	m_VAO.linkIndexBuffer(m_EBO);
	m_VAO.bind();
	program.use();
	GLDEBUG(glDrawElements(GL_TRIANGLES, m_EBO.objectCount(), GL_UNSIGNED_INT, 0));
}

void Renderer::drawShader(ShaderProgram& program, const Framebuffer& framebuffer)
{
	bool svp = false;
	if ((m_viewportHeight != framebuffer.height()) || (m_viewportWidth != framebuffer.width()))
	{
		GLDEBUG(glViewport(0, 0, framebuffer.width(), framebuffer.height()));
		svp = true;
	}
	framebuffer.bind();
	clearDepth(framebuffer);
	m_VAO.linkIndexBuffer(m_EBO);
	m_VAO.bind();
	program.use();
	GLDEBUG(glDrawElements(GL_TRIANGLES, m_EBO.objectCount(), GL_UNSIGNED_INT, 0));
	GLDEBUG(glViewport(0, 0, framebuffer.width(), framebuffer.height()));
	if (svp)
	{
		GLDEBUG(glViewport(0, 0, m_viewportWidth, m_viewportHeight));
	}
}

Texture Renderer::blurX(const Texture& tex, float roughness)
{
	int width = tex.attribs().WIDTH;
	int height = tex.attribs().HEIGHT;
	return blurX(tex, roughness, width, height);
}

Texture Renderer::blurX(const Texture& tex, float roughness, int width, int height)
{
	//create framebuffer to render texture to
	int max_dim = glm::max(width, height);
	float x_offset_multiplier = height / float(max_dim);

	Framebuffer framebuffer(width, height);
	Framebuffer tempfb(width, height);

	auto att = framebuffer.texture().attribs();
	att.MIPMAP = false;
	framebuffer.setTextureAttribs(att);
	tempfb.setTextureAttribs(att);

	//create and compile vertex shader
	Shader vertShader(GL_VERTEX_SHADER);
	vertShader.loadSource("postvert.glsl");


	//create and compile fragment shaders
	Shader blurfrag(GL_FRAGMENT_SHADER);
	blurfrag.loadSource("blurGfragX.glsl");
	Shader postfrag(GL_FRAGMENT_SHADER);
	postfrag.loadSource("postfrag.glsl");



	//create shader program and attach compiled shaders to program
	ShaderProgram blurprogram;
	blurprogram.addShader(vertShader);
	blurprogram.addShader(blurfrag);
	ShaderProgram postprogram;
	postprogram.addShader(vertShader);
	postprogram.addShader(postfrag);


	//link program binaries to OpenGL code
	blurprogram.link();
	postprogram.link();

	int kernel_size = glm::min(width, height);
	constexpr int gpu_kernel_size = 20;

	std::vector<float> kernel = gaussian(kernel_size, roughness, true);
	kernel[0] *= 0.5;
	glm::ivec2 resolution(width, height);


	blurprogram.setUniform("u_texture", tex);
	blurprogram.setUniform("u_frameBuffer", framebuffer.texture());
	blurprogram.setUniform("u_frameBufferWeight", 0.0f);
	blurprogram.setUniform("u_resolution", resolution);
	blurprogram.setUniform("u_gamma", 1.0f);

	postprogram.setUniform("u_resolution", resolution);
	postprogram.setUniform("u_gamma", 1.0f);
	postprogram.setUniform("u_frameBuffer", tempfb.texture());
 

	//float weight = kernel[0];
	float sum =0;
	int bound = ((kernel_size % gpu_kernel_size) ? (kernel_size / gpu_kernel_size) + 1 : (kernel_size / gpu_kernel_size));
	for (int i = 0; i < bound; i++)
	{
		std::vector<float> weights(gpu_kernel_size, 0);
		std::vector<float> offsets(gpu_kernel_size, 0);
		int j = 0;
		while(j < gpu_kernel_size)
		{
			if (i * gpu_kernel_size + j < kernel_size)
			{
				int index = i * gpu_kernel_size + j;
				offsets[j] = index * x_offset_multiplier / (kernel_size - 1.0);
				weights[j] = kernel[index];
			}
			else
			{
				break;
			}
			j++;
		}
		//float offset = i * x_offset_multiplier / (kernel_size - 1.0);
		//weight = kernel[i];
		//sum += weight;
		//blurprogram.setUniform("u_offset", offset);
		//blurprogram.setUniform("u_textureWeight", weight);
		blurprogram.setUniform("u_offsets", offsets);
		blurprogram.setUniform("u_textureWeights", weights);
		//std::cout << "calling 2 draws for x: " << i << std::endl;
		drawShader(blurprogram, tempfb);
		drawShader(postprogram, framebuffer);
		blurprogram.setUniform("u_frameBufferWeight", 1.0f);
	}
	std::cout << "blurX done" << std::endl;
	return framebuffer.texture();
}

Texture Renderer::blurY(const Texture& tex, float roughness)
{
	int width = tex.attribs().WIDTH;
	int height = tex.attribs().HEIGHT;
	return blurY(tex, roughness, width, height);
}

Texture Renderer::blurY(const Texture& tex, float roughness, int width, int height)
{
	//create framebuffer to render texture to
	int max_dim = glm::max(width, height);
	float y_offset_multiplier = width / float(max_dim);

	Framebuffer framebuffer(width, height);
	Framebuffer tempfb(width, height);

	auto att = framebuffer.texture().attribs();
	att.MIPMAP = false;
	framebuffer.setTextureAttribs(att);
	tempfb.setTextureAttribs(att);

	//create and compile vertex shader
	Shader vertShader(GL_VERTEX_SHADER);
	vertShader.loadSource("postvert.glsl");


	//create and compile fragment shaders
	Shader blurfrag(GL_FRAGMENT_SHADER);
	blurfrag.loadSource("blurGfragY.glsl");
	Shader postfrag(GL_FRAGMENT_SHADER);
	postfrag.loadSource("postfrag.glsl");



	//create shader program and attach compiled shaders to program
	ShaderProgram blurprogram;
	blurprogram.addShader(vertShader);
	blurprogram.addShader(blurfrag);
	ShaderProgram postprogram;
	postprogram.addShader(vertShader);
	postprogram.addShader(postfrag);


	//link program binaries to OpenGL code
	blurprogram.link();
	postprogram.link();

	int kernel_size = glm::min(width, height);
	constexpr int gpu_kernel_size = 20;

	std::vector<float> kernel = gaussian(kernel_size, roughness, true);
	kernel[0] *= 0.5;
	glm::ivec2 resolution(width, height);


	blurprogram.setUniform("u_texture", tex);
	blurprogram.setUniform("u_frameBuffer", framebuffer.texture());
	blurprogram.setUniform("u_frameBufferWeight", 0.0f);
	blurprogram.setUniform("u_resolution", resolution);
	blurprogram.setUniform("u_gamma", 1.0f);

	postprogram.setUniform("u_resolution", resolution);
	postprogram.setUniform("u_gamma", 1.0f);
	postprogram.setUniform("u_frameBuffer", tempfb.texture());
 

	//float weight = kernel[0];
	bool last_drawn_frame = 0;
	float sum =0;
	int bound = ((kernel_size % gpu_kernel_size) ? (kernel_size / gpu_kernel_size) + 1 : (kernel_size / gpu_kernel_size));
	for (int i = 0; i < bound; i++)
	{
		std::vector<float> weights(gpu_kernel_size, 0);
		std::vector<float> offsets(gpu_kernel_size, 0);
		int j = 0;
		while (j < gpu_kernel_size)
		{
			if (i * gpu_kernel_size + j < kernel_size)
			{
				int index = i * gpu_kernel_size + j;
				offsets[j] = index * y_offset_multiplier / (kernel_size - 1.0);
				weights[j] = kernel[index];
			}
			else
			{
				break;
			}
			j++;
		}
		//float offset = i * y_offset_multiplier / (kernel_size - 1.0);
		//weight = kernel[i];
		//sum += weight;
		//blurprogram.setUniform("u_offset", offset);
		//blurprogram.setUniform("u_textureWeight", weight);
		blurprogram.setUniform("u_offsets", offsets);
		blurprogram.setUniform("u_textureWeights", weights);
		//std::cout << "calling 2 draws for y: " << i << std::endl;
		drawShader(blurprogram, tempfb);
		drawShader(postprogram, framebuffer);
		blurprogram.setUniform("u_frameBufferWeight", 1.0f);
	}
	std::cout << "blurY done" << std::endl;
	return framebuffer.texture();
}


Texture Renderer::reduceEnvGGX(const Texture& env, int width, int height, float roughness, int index /*=0*/)
{
	std::string indstr = std::to_string(index);

	//create framebuffer to render texture to
	Framebuffer framebuffer(width, height);

	auto att = framebuffer.texture().attribs();
	att.EXTEND_S = GL_REPEAT;
	att.EXTEND_T = GL_MIRRORED_REPEAT;
	att.MIPMAP = false;
	framebuffer.setTextureAttribs(att);

	//create and compile vertex shader
	Shader vertShader(GL_VERTEX_SHADER);
	vertShader.loadSource("postvert.glsl");


	//create and compile fragment shaders
	Shader mollfrag(GL_FRAGMENT_SHADER);
	mollfrag.loadSource("mollweidefrag.glsl");

	Shader equifrag(GL_FRAGMENT_SHADER);
	equifrag.loadSource("molltoeqfrag.glsl");


	//create shader program and attach compiled shaders to program
	ShaderProgram mollprogram;
	mollprogram.addShader(vertShader);
	mollprogram.addShader(mollfrag);

	ShaderProgram eqprogram;
	eqprogram.addShader(vertShader);
	eqprogram.addShader(equifrag);


	//link program binaries to OpenGL code
	mollprogram.link();

	eqprogram.link();

	glm::ivec2 resolution(width, height);
	Texture tex = env;
	att = tex.attribs();
	att.EXTEND_S = GL_REPEAT;
	att.EXTEND_T = GL_MIRRORED_REPEAT;
	tex.setAttribs(att);
	tex = blurY(tex, roughness, width, height);

	mollprogram.setUniform("u_resolution", resolution);
	mollprogram.setUniform("u_gamma", 1.0f);


	eqprogram.setUniform("u_resolution", resolution);
	eqprogram.setUniform("u_gamma", 1.0f);

	//std::cout << "blur y is done" << std::endl;
	//saveImage("./Resources/Tests/" + indstr + "01.png", tex);
	mollprogram.setUniform("u_env", tex);
	drawShader(mollprogram, framebuffer);
	//saveImage("./Resources/Tests/" + indstr + "02.png", framebuffer1.texture());
	
	tex = blurX(framebuffer.texture(), roughness);
	//saveImage("./Resources/Tests/" + indstr + "03.png", tex);
	eqprogram.setUniform("u_env", tex);
	drawShader(eqprogram, framebuffer);
	//saveImage("./Resources/Tests/" + indstr + "04.png", framebuffer1.texture());
	std::cout << "done!!" << std::endl;
	return framebuffer.texture();
}

CUBEMAP_INITIALIZER Renderer::renderCubemap(const Texture& env1, int dimension, bool srgb/*=false*/)
{
	//create and compile vertex shader
	Shader vertexShader(GL_VERTEX_SHADER);
	vertexShader.loadSource("postvert.glsl");

	//create and compile fragment shader
	Shader fragmentShader(GL_FRAGMENT_SHADER);
	fragmentShader.loadSource("cubemapfrag.glsl");
	Shader postfragShader(GL_FRAGMENT_SHADER);
	postfragShader.loadSource("postfrag.glsl");

	//create shader program and attach compiled shaders to program
	ShaderProgram cubeprogram;
	cubeprogram.addShader(vertexShader);
	cubeprogram.addShader(fragmentShader);
	ShaderProgram postprogram;
	postprogram.addShader(vertexShader);
	postprogram.addShader(postfragShader);

	//link program binaries to OpenGL code												
	cubeprogram.link();
	postprogram.link();

	std::vector<cube_data> raw_data;

	cubeprogram.setUniform("u_gamma", 1.0f);
	srgb ? postprogram.setUniform("u_gamma", 2.2f) : postprogram.setUniform("u_gamma", 1.0f);

	int width, height, dim;
	dim = dimension;
	height = dim * 2;
	width = height * env1.width() / env1.height();
	float rouughness = 0;
	int cpp = 0;

	unsigned int counter = 0;
	std::cout << "starting reduction cycle..." << std::endl;
	const int max_levels = 8;

	for (int counter = 0; counter < max_levels; counter++)
	{
		float rouughness = float(counter)/(max_levels-1);
		std::cout << "	" << "calculating level " << counter << " " << std::endl;
		Texture env = reduceEnvGGX(env1, width, height, rouughness, counter);
		TEXTURE_ATTRIBS attr = env.attribs();
		attr.EXTEND_T = GL_MIRRORED_REPEAT;
		env.setAttribs(attr);

		//std::cout << ".";
		cube_data level_data;

		//create framebuffer to render cube map to
		Framebuffer faceframebuffer(dim, dim);
		Framebuffer postframebuffer(dim, dim);

		cubeprogram.setUniform("u_env", env);
		cubeprogram.setUniform("u_resolution", glm::vec2(faceframebuffer.width(), faceframebuffer.height()));
		postprogram.setUniform("u_frameBuffer", faceframebuffer.texture());
		postprogram.setUniform("u_resolution", glm::vec2(postframebuffer.width(), postframebuffer.height()));
		//std::cout << ".";
		for (int face = 0; face < 6; face++)
		{
			cubeprogram.setUniform("u_face", face);
			drawShader(cubeprogram, faceframebuffer);
			drawShader(postprogram, postframebuffer);
			//std::cout << ".";

			int w = postframebuffer.width();
			int h = postframebuffer.height();
			GLsizei c = postframebuffer.texture().cpp();
			//std::cout << c << std::endl;
			GLsizei stride = c * w;
			GLsizei bufferSize = stride * h;
			float* buffer = new float[bufferSize];
			postframebuffer.bind();
			postframebuffer.texture().bind();
			GLDEBUG(glPixelStorei(GL_PACK_ALIGNMENT, 1));
			//glReadBuffer(GL_FRONT);
			//GLDEBUG(glReadBuffer(GL_COLOR_ATTACHMENT0));
			GLDEBUG(glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, buffer));
			level_data[face] = buffer;
			//std::cout << ".";

		}
		raw_data.push_back(level_data);

		std::cout << /*std::endl <<*/ "	" << "level " << counter << " calculated with width " << dim <<" , height " << dim << "and roughness of " << rouughness << std::endl;
		dim = dim / 2;
		cpp = postframebuffer.texture().cpp();
		if (!dim)
			break;

	}

	std::cout << "calculations done. base width: " << dimension << " calculated " << raw_data.size() << " levels." << std::endl;

	CUBEMAP_INITIALIZER initializer;
	initializer.CPP = cpp;
	initializer.BASE_HEIGHT = dimension;
	initializer.BASE_WIDTH = dimension;
	initializer.RAW_DATA = raw_data;
	return initializer;
}

void Renderer::saveImage(std::string filepath, const Framebuffer& framebuffer)
{
	int width = framebuffer.width();
	int height = framebuffer.height();
	GLsizei nrChannels = 3;
	GLsizei stride = nrChannels * width;
	stride += (stride % 4) ? (4 - stride % 4) : 0;
	GLsizei bufferSize = stride * height;
	std::vector<uint8_t> buffer(bufferSize);
	framebuffer.bind();
	framebuffer.texture().bind();
	GLDEBUG(glPixelStorei(GL_PACK_ALIGNMENT, 4));
	//glReadBuffer(GL_FRONT);
	//GLDEBUG(glReadBuffer(GL_COLOR_ATTACHMENT0));
	GLDEBUG(glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer.data()));
	stbi_flip_vertically_on_write(true);
	stbi_write_png(filepath.c_str(), width, height, nrChannels, buffer.data(), stride);
	GLDEBUG(glPixelStorei(GL_PACK_ALIGNMENT, 1));
}

void Renderer::saveImage(std::string filepath, const Texture& texture)
{
	int width = texture.width();
	int height = texture.height();
	GLsizei nrChannels = 3;
	GLsizei stride = nrChannels * width;
	GLsizei bufferSize = stride * height;
	uint8_t* buffer = new uint8_t[bufferSize];
	texture.bind();
	GLDEBUG(glPixelStorei(GL_PACK_ALIGNMENT, 1));
	//glReadBuffer(GL_FRONT);
	//GLDEBUG(glReadBuffer(GL_COLOR_ATTACHMENT0));
	GLDEBUG(glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer));
	stbi_flip_vertically_on_write(true);
	stbi_write_png(filepath.c_str(), width, height, nrChannels, buffer, stride);
	GLDEBUG(glPixelStorei(GL_PACK_ALIGNMENT, 1));
}

void Renderer::saveImage(std::string filepath)
{
	int width = m_viewportWidth;
	int height = m_viewportHeight;
	GLsizei nrChannels = 3;
	GLsizei stride = nrChannels * width;
	stride += (stride % 4) ? (4 - stride % 4) : 0;
	GLsizei bufferSize = stride * height;
	std::vector<uint8_t> buffer(bufferSize);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	GLDEBUG(glPixelStorei(GL_PACK_ALIGNMENT, 4));
	//glReadBuffer(GL_FRONT);
	//GLDEBUG(glReadBuffer(GL_COLOR_ATTACHMENT0));
	GLDEBUG(glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer.data()));
	stbi_flip_vertically_on_write(true);
	stbi_write_png(filepath.c_str(), width, height, nrChannels, buffer.data(), stride);
	GLDEBUG(glPixelStorei(GL_PACK_ALIGNMENT, 1));
}

//!!!!add support for index-less drawing!!!!