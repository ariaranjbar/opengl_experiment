#include <iostream>
#include <chrono>

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "STB/stb_image.h"


#include"renderutils.h"
#include "shader.h"
#include "vertexarray.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"
#include "renderer.h"
#include "texture.h"
#include "primitives.h"
#include "camera.h"
#include "framebuffer.h"
#include "uiutils.h"

glm::mat4 lightRotationControl(glm::mat4 mat, GLFWwindow* window, int width, int height, float sensitivity);
glm::mat4 lightTranslateControl(glm::mat4 mat, GLFWwindow* window);
float* getWorldAxis();

double scrollY = 0.0;
double wheelscale = 3.0;
void screoll_callback(GLFWwindow* window, double x, double y)
{
	scrollY = y * wheelscale;
}

int main()
{


	stbi_ldr_to_hdr_gamma(1.0f);


	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);
	typedef std::chrono::high_resolution_clock Time;
	typedef std::chrono::milliseconds ms;
	typedef std::chrono::duration<float> fsec;
	auto t0 = Time::now();
	auto t1 = Time::now();
	fsec fs;
	fsec fsd;
	//setting some values for window size
	constexpr int WindowWidth = 800;
	constexpr int WindowHeight = 800;
	constexpr int viewportBorder = 0;
	constexpr int viewportWidth = WindowWidth - 2 * viewportBorder;
	constexpr int viewportHeight = WindowHeight - 2 * viewportBorder;

	//initializing glfw
	glfwInit();

	//giving glfw info about the OpenGL version we are using
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_VISIBLE , GLFW_FALSE);
	//creating a context window for glfw
	GLFWwindow* window = glfwCreateWindow(WindowWidth, WindowHeight, "OpenGL Experiment", NULL, NULL);
	{

		//error checking to ensure glfw context is made successfully
		if (window == NULL)
		{
			std::cout << "failed to create GLFW window!!" << std::endl;
			glfwTerminate();
			return -1;
		}

		//setting the created window context as current
		glfwMakeContextCurrent(window);

		//load glad so it configures OpenGL
		gladLoadGL();

		//specify the viewport for OpenGL in the current window
		glViewport(viewportBorder, viewportBorder, viewportWidth, viewportHeight);
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);

		//==================================ui initialization===========================================
		uiutils::UI ui_instance;
		//==============================================================================================

		//================================texture shader program========================================
		//create and compile vertex shader
		Shader vertexShader(GL_VERTEX_SHADER);
		vertexShader.loadSource("unwrapvert.glsl");

		//create and compile geometry shader
		Shader geometryShader(GL_GEOMETRY_SHADER);
		geometryShader.loadSource("geometry.glsl");

		//create and compile fragment shader
		Shader fragmentShader(GL_FRAGMENT_SHADER);
		fragmentShader.loadSource("fragment.glsl");

		//create shader program and attach compiled shaders to program
		ShaderProgram program;
		program.addShader(vertexShader);
		program.addShader(geometryShader);
		program.addShader(fragmentShader);

		//link program binaries to OpenGL code											 
		program.link();
		//==============================================================================================


		//=================================debug shader program=========================================
		//create and compile vertex shader
		Shader debugvertShader(GL_VERTEX_SHADER);
		debugvertShader.loadSource("debugvert.glsl");

		//create and compile geometry shader
		Shader debuggeomShader(GL_GEOMETRY_SHADER);
		debuggeomShader.loadSource("debuggeom.glsl");

		//create and compile fragment shader
		Shader debugfragShader(GL_FRAGMENT_SHADER);
		debugfragShader.loadSource("debugfrag.glsl");

		//create shader program and attach compiled shaders to program
		ShaderProgram debugprogram;
		debugprogram.addShader(debugvertShader);
		debugprogram.addShader(debuggeomShader);
		debugprogram.addShader(debugfragShader);

		//link program binaries to OpenGL code											 
		debugprogram.link();
		//==============================================================================================


		//================================normals shader program========================================
		//create and compile vertex shader
		Shader normals_vertShader(GL_VERTEX_SHADER);
		normals_vertShader.loadSource("normals_vert.glsl");

		//create and compile geometry shader
		Shader normals_geomShader(GL_GEOMETRY_SHADER);
		normals_geomShader.loadSource("normals_geom.glsl");

		//create and compile fragment shader
		Shader normals_fragShader(GL_FRAGMENT_SHADER);
		normals_fragShader.loadSource("normals_frag.glsl");

		//create shader program and attach compiled shaders to program
		ShaderProgram normals_program;
		normals_program.addShader(normals_vertShader);
		normals_program.addShader(normals_geomShader);
		normals_program.addShader(normals_fragShader);

		//link program binaries to OpenGL code											 
		normals_program.link();
		//==============================================================================================


		//=================================light shader program=========================================
		//create and compile vertex shader
		Shader lightvertexShader(GL_VERTEX_SHADER);
		lightvertexShader.loadSource("vertex.glsl");

		//create and compile fragment shader
		Shader lightfragmentShader(GL_FRAGMENT_SHADER);
		lightfragmentShader.loadSource("lightfrag.glsl");

		//create shader program and attach compiled shaders to program
		ShaderProgram lightprogram;
		lightprogram.addShader(lightvertexShader);
		lightprogram.addShader(lightfragmentShader);

		//link program binaries to OpenGL code												
		lightprogram.link();
		//==============================================================================================


		//==================================axis shader program=========================================
		//create and compile vertex shader
		Shader axisvertexShader(GL_VERTEX_SHADER);
		axisvertexShader.loadSource("axisvert.glsl");

		//create and compile fragment shader
		Shader axisfragmentShader(GL_FRAGMENT_SHADER);
		axisfragmentShader.loadSource("axisfrag.glsl");

		//create shader program and attach compiled shaders to program
		ShaderProgram axisprogram;
		axisprogram.addShader(axisvertexShader);
		axisprogram.addShader(axisfragmentShader);

		//link program binaries to OpenGL code												
		axisprogram.link();
		//==============================================================================================


		//==================================post shader program=========================================
		//create and compile vertex shader
		Shader postvertexShader(GL_VERTEX_SHADER);
		postvertexShader.loadSource("postvert.glsl");

		//create and compile fragment shader
		Shader postfragmentShader(GL_FRAGMENT_SHADER);
		postfragmentShader.loadSource("postfrag.glsl");

		//create shader program and attach compiled shaders to program
		ShaderProgram postprogram;
		postprogram.addShader(postvertexShader);
		postprogram.addShader(postfragmentShader);

		//link program binaries to OpenGL code												
		postprogram.link();
		//==============================================================================================


		//==================================AA shader program===========================================
		//create and compile vertex shader
		Shader AAvertexShader(GL_VERTEX_SHADER);
		AAvertexShader.loadSource("postvert.glsl");

		//create and compile fragment shader
		Shader AAfragmentShader(GL_FRAGMENT_SHADER);
		AAfragmentShader.loadSource("postfragAA.glsl");

		//create shader program and attach compiled shaders to program
		ShaderProgram AAprogram;
		AAprogram.addShader(AAvertexShader);
		AAprogram.addShader(AAfragmentShader);

		//link program binaries to OpenGL code												
		AAprogram.link();
		//==============================================================================================


		////==================================edge shader program=======================================
		////create and compile vertex shader
		//Shader edgevertexShader(GL_VERTEX_SHADER);
		//edgevertexShader.loadSource("edgevert.glsl");
		//
		////create and compile fragment shader
		//Shader edgefragmentShader(GL_FRAGMENT_SHADER);
		//edgefragmentShader.loadSource("edgefrag.glsl");
		//
		////create shader program and attach compiled shaders to program
		//ShaderProgram edgeprogram;
		//edgeprogram.addShader(edgevertexShader);
		//edgeprogram.addShader(edgefragmentShader);
		//
		////link program binaries to OpenGL code												
		//edgeprogram.link();
		////============================================================================================


		//==========================================MODEL===============================================
		wavefront::Object obj = wavefront::parse_obj_file(
			ui_instance.queryFile("model", uiutils::file_format::MODEL));
		primitives3d::Mesh loaded_model(obj);

		//primitives3d::Cylinder loaded_model(15, 30, 12);

		//primitives3d::Box loaded_model(30, 30, 30);

		//primitives3d::Plane loaded_model(30, 30);

		//create and bind vertex array object
		VertexArray VAO;

		//create buffer to contain vertex data for vertex array object
		VertexBuffer VBO;

		//put vertex data in buffer object
		VBO.data<GLfloat>(loaded_model.vertexCount() * (11), loaded_model.vertices(), GL_STATIC_DRAW);
		VBO.addAttribute(LayoutAttribute(0, GL_FLOAT, 3));
		VBO.addAttribute(LayoutAttribute(1, GL_FLOAT, 3));
		VBO.addAttribute(LayoutAttribute(2, GL_FLOAT, 3));
		VBO.addAttribute(LayoutAttribute(3, GL_FLOAT, 2));


		//create buffer to contain index data for element array object
		IndexBuffer EBO;


		//put index data in buffer object
		//EBO.data<GLuint>(sizeof(indices)/sizeof(GLuint), &indices, GL_STATIC_DRAW);
		EBO.data<GLuint>(loaded_model.indexCount(), loaded_model.indices(), GL_STATIC_DRAW);

		//linking the vertex data buffer object and the index buffer object to the vertex array
		VAO.linkVertexBuffer(VBO);
		VAO.enableAttribute(0);
		VAO.enableAttribute(1);
		VAO.enableAttribute(2);
		VAO.enableAttribute(3);
		VAO.linkIndexBuffer(EBO);
		//==============================================================================================


		//==========================================LIGHT===============================================
		primitives3d::Box box(3.0f, 3.0f, 3.0f, glm::vec3(0.0f, 0.0f, 0.0f));

		//create and bind vertex array object
		VertexArray VAObox;

		//create buffer to contain vertex data for vertex array object
		VertexBuffer VBObox;

		//put vertex data in buffer object
		VBObox.data<GLfloat>(box.vertexCount() * (11), box.vertices(), GL_STATIC_DRAW);
		VBObox.addAttribute(LayoutAttribute(0, GL_FLOAT, 3));
		VBObox.addAttribute(LayoutAttribute(1, GL_FLOAT, 3));
		VBObox.addAttribute(LayoutAttribute(2, GL_FLOAT, 3));
		VBObox.addAttribute(LayoutAttribute(3, GL_FLOAT, 2));

		
		//create buffer to contain index data for element array object
		IndexBuffer EBObox;


		//put index data in buffer object
		//EBO.data<GLuint>(sizeof(indices)/sizeof(GLuint), &indices, GL_STATIC_DRAW);
		EBObox.data<GLuint>(box.indexCount(), box.indices(), GL_STATIC_DRAW);

		//linking the vertex data buffer object and the index buffer object to the vertex array
		VAObox.linkVertexBuffer(VBObox);
		VAObox.enableAttribute(0);
		VAObox.enableAttribute(1);
		VAObox.enableAttribute(2);
		VAObox.enableAttribute(3);
		VAObox.linkIndexBuffer(EBObox);

		glm::vec3 lightColor = glm::vec3(1.2f, 1.1f, 1.0f) * 5.0f;
		glm::vec3 lightPos = box.position();
		//==============================================================================================


		//=========================================AXIS=================================================

		//create and bind vertex array object
		VertexArray VAOax;

		//create buffer to contain vertex data for vertex array object
		VertexBuffer VBOax;

		//put vertex data in buffer object
		float* axisverts = getWorldAxis();
		VBOax.data<GLfloat>(6 * 6, axisverts, GL_STATIC_DRAW);
		VBOax.addAttribute(LayoutAttribute(0, GL_FLOAT, 3));
		VBOax.addAttribute(LayoutAttribute(1, GL_FLOAT, 3));
		delete[] axisverts;

		unsigned int axisinds[]{ 0, 1, 2, 3, 4, 5 };

		//create buffer to contain index data for element array object
		IndexBuffer EBOax;


		//put index data in buffer object
		//EBO.data<GLuint>(sizeof(indices)/sizeof(GLuint), &indices, GL_STATIC_DRAW);
		EBOax.data<GLuint>(6, axisinds, GL_STATIC_DRAW);

		//linking the vertex data buffer object and the index buffer object to the vertex array
		VAOax.linkVertexBuffer(VBOax);
		VAOax.enableAttribute(0);
		VAOax.enableAttribute(1);
		VAOax.linkIndexBuffer(EBOax);
		//==============================================================================================


		//=======================================TEXTURES===============================================
		//importing a texture;
		Texture basecolor(ui_instance.queryFile("base color / albedo", uiutils::file_format::IMAGE), true);
		basecolor.mipMap(true);
		basecolor.free();
		Texture roughness(ui_instance.queryFile("roughness", uiutils::file_format::IMAGE));
		roughness.mipMap(true);
		roughness.free();
		Texture emissive(ui_instance.queryFile("emissive", uiutils::file_format::IMAGE), true);
		emissive.mipMap(true);
		emissive.free();
		Texture normal(ui_instance.queryFile("normal", uiutils::file_format::IMAGE));
		//Texture normal("Resources/noisenormal2.jpg");
		normal.mipMap(false);
		normal.free();
		Texture env(ui_instance.queryFile("environmet", uiutils::file_format::IMAGE), false);
		auto ker = gaussian(400, 0.999, true);
		float ker0 = ker[0];
		for(int i = 0; i < ker.size(); i++)
		{
			ker[i] = ker[i] / ker0;
		}
		Texture env3(400, 1, 1, 9729U, false, ker.data());
		env3.free();
		auto attribs = env3.attribs();
		attribs.EXTEND_S = GL_CLAMP_TO_EDGE;
		attribs.EXTEND_T = GL_CLAMP_TO_EDGE;
		env3.setAttribs(attribs);
		
		//==============================================================================================


		//========================================CAMERA================================================
		Camera camera(45.0f, 0.1, 10000.0f, viewportHeight, viewportWidth, glm::vec3(0.0, 15, 120));
		camera.lockTo(glm::vec3(0, 0, 0), true);
		camera.unlock();
		glm::mat3 camdirs;
		camdirs[0] = camera.right();
		camdirs[1] = glm::cross(camera.right(), camera.orientation());
		camdirs[2] = -camera.orientation();
		camera.addFramebuffer();
		camera.addFramebuffer();
		//camera.addFramebuffer();
		//camera.addFramebuffer();
		//camera.addFramebuffer();
		//camera.addFramebuffer();
		//camera.addFramebuffer();
		//camera.addFramebuffer();
		camera.calculateOffsets(256);
		//==============================================================================================


		//=======================================RENDERER===============================================
		//define a renderer to handle drawing
		Renderer renderer;
		renderer.setViewport(viewportWidth, viewportHeight);
		//==============================================================================================


		//=======================================CUBEMAPS===============================================
		//generating a cubemap
		Cubemap cubmap(renderer.renderCubemap(env, 256*3, false));
		Texture env1(env);
		auto att = env1.attribs();
		att.EXTEND_S = GL_REPEAT;
		att.EXTEND_T = GL_MIRRORED_REPEAT;
		env1.setAttribs(att);
		//Texture env2;
		//env2 = renderer.blurY(env1, 0.5);
		cubmap.free();
		//==============================================================================================


		//set the "clear color" to Navy Blue and clear frame buffer to it, swap buffer to cleared color
		glm::vec3 clearcolor(0.022f, 0.032f, 0.044f);
		glm::vec4 clearalpha(0.0f, 0.0f, 0.0f, 0.0f);
		glfwSwapBuffers(window);
		glfwShowWindow(window);


		//some constant values and uniforms

		glm::mat4 modelMatrix = glm::mat4(1.0);
		glm::ivec2 resolution(viewportWidth, viewportHeight);


		program.setUniform("u_baseColor", basecolor);
		program.setUniform("u_roughness", roughness);
		program.setUniform("u_emissive", emissive);
		program.setUniform("u_normal", normal);
		program.setUniform("u_resolution", resolution);
		program.setUniform("u_modelMatrix", modelMatrix);
		program.setUniform("u_lightPos", lightPos);
		program.setUniform("u_lightColor", lightColor);
		program.setUniform("u_cubemap", cubmap);
		program.setUniform("u_F1", 1);


		lightprogram.setUniform("u_modelMatrix", modelMatrix);
		lightprogram.setUniform("u_lightColor", lightColor);


		axisprogram.setUniform("u_modelMatrix", modelMatrix);
		axisprogram.setUniform("u_resolution", resolution);


		//postprogram.setUniform("u_frameBuffer", env);
		postprogram.setUniform("u_resolution", resolution);
		postprogram.setUniform("u_gamma", 2.2f);
		postprogram.setUniform("u_frameBuffer", camera.framebuffer(2).texture());


		AAprogram.setUniform("u_resolution", resolution);
		AAprogram.setUniform("u_gamma", 1.0f);
		AAprogram.setUniform("u_count", 1);
		AAprogram.setUniform("u_frameBuffer0", camera.framebuffer(0).texture());
		AAprogram.setUniform("u_frameBuffer1", camera.framebuffer(1).texture());


		normals_program.setUniform("u_modelMatrix", modelMatrix);

		debugprogram.setUniform("u_baseColor", basecolor);
		debugprogram.setUniform("u_roughness", roughness);
		debugprogram.setUniform("u_emissive", emissive);
		debugprogram.setUniform("u_modelMatrix", modelMatrix);
		debugprogram.setUniform("u_normal", normal);
		debugprogram.setUniform("u_mode", 1);
		debugprogram.setUniform("u_camdirs", camdirs);


		float speed = 10.0f;
		float sensitivity = 20.0f;
		glm::mat4 light_rotation_mat = modelMatrix;
		glm::mat4 light_translate_mat = modelMatrix;
		int debugmode = 1;
		bool tab = false;
		bool tab_is_pressed = false;
		bool hot_reload = false;
		bool c_is_pressed = false;
		bool culling = true;
		bool g_is_pressed = false;
		float g_inversion = 1;
		bool num_is_pressed = false;
		int count = 1;

		float f0 = 0.0f;

		int fac = 10;
		//Texture atex = renderer.reduceEnvMap(env, 200 *fac, 100*fac, 1);
		//postprogram.setUniform("u_frameBuffer", env);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		glfwSetScrollCallback(window, screoll_callback);

		//main loop
		camera.setActiveFramebuffer(0);
		while (!(glfwWindowShouldClose(window)))
		{
			if ((glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) && !hot_reload)
			{
				count = 1;
				hot_reload = true;
				f0 = 0.0f;
				program.clean();
				vertexShader.loadSource("unwrapvert.glsl");
				geometryShader.loadSource("geometry.glsl");
				fragmentShader.loadSource("fragment.glsl");
				program.addShader(vertexShader);
				program.addShader(geometryShader);
				program.addShader(fragmentShader);
				program.link();
				program.setUniform("u_baseColor", basecolor);
				program.setUniform("u_roughness", roughness);
				program.setUniform("u_emissive", emissive);
				program.setUniform("u_normal", normal);
				program.setUniform("u_cubemap", cubmap);
				program.setUniform("u_resolution", resolution);
				program.setUniform("u_modelMatrix", modelMatrix);
				program.setUniform("u_lightPos", lightPos);
				program.setUniform("u_lightColor", lightColor);
				program.setUniform("u_F1", 0);
				program.setUniform("u_F0", f0);
			}
			if ((glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE))
			{
				hot_reload = false;
			}
			if ((glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) && !c_is_pressed)
			{
				count = 1;
				c_is_pressed = true;
				culling = !culling;
			}
			if ((glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE))
			{
				c_is_pressed = false;
			}
			if (culling)
			{
				glEnable(GL_CULL_FACE);
			}
			else
			{
				glDisable(GL_CULL_FACE);
			}
			if ((glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) && !g_is_pressed)
			{
				count = 1;
				g_is_pressed = true;
				g_inversion = 1 - g_inversion;
			}
			if ((glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE))
			{
				g_is_pressed = false;
			}

			//polling events such as resize window and translate so the window is responsive
			glfwPollEvents();
			fsd = Time::now() - t1;
			t1 = Time::now();
			fs = t1 - t0;
			program.setUniform("u_time", fs.count());
			debugprogram.setUniform("u_time", fs.count());
			postprogram.setUniform("u_time", fs.count());

			program.setUniform("u_g_inversion", g_inversion);
			debugprogram.setUniform("u_g_inversion", g_inversion);


			//receiving camera movement from user input and passing to shader
			glm::mat4 a = camera.matrix();
			CameraInputControl(camera, window, speed, sensitivity, fsd.count());
			glm::mat4 b = camera.matrix();
			if (a != b)
			{
				count = 1;
			}

			camdirs[0] = camera.right();
			camdirs[1] = glm::cross(camera.right(), camera.orientation());
			camdirs[2] = -camera.orientation();
			debugprogram.setUniform("u_camdirs", camdirs);
			//swap the completed buffer with the current display
			glfwSwapBuffers(window);

			if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
			{
				count = 1;
				debugprogram.setUniform("u_mode", 1);
				f0 = 0.5f / 8.0f;
				program.setUniform("u_F0", f0);
				//postprogram.setUniform("u_frameBuffer", env);
				//env1 = renderer.blurY(env2, 0.1);
				//env1 = renderer.blurX(env1, 0.1);
			}
			if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
			{
				count = 1;
				debugprogram.setUniform("u_mode", 2);
				f0 = 1.5f / 8.0f;
				program.setUniform("u_F0", f0);
				//postprogram.setUniform("u_frameBuffer", atex);
				//env1 = renderer.blurY(env2, 0.2);
				//env1 = renderer.blurX(env1, 0.2);
			}
			if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
			{
				count = 1;
				debugprogram.setUniform("u_mode", 3);
				f0 = 2.5f / 8.0f;
				program.setUniform("u_F0", f0);
				//env1 = renderer.blurY(env2, 0.3);
				//env1 = renderer.blurX(env1, 0.3);
			}
			if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
			{
				count = 1;
				debugprogram.setUniform("u_mode", 4);
				f0 = 3.5f / 8.0f;
				program.setUniform("u_F0", f0);
				//env1 = renderer.blurY(env2, 0.4);
				//env1 = renderer.blurX(env1, 0.4);
			}
			if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
			{
				count = 1;
				debugprogram.setUniform("u_mode", 5);
				f0 = 4.5f / 8.0f;
				program.setUniform("u_F0", f0);
				//env1 = renderer.blurY(env2, 0.5);
				//env1 = renderer.blurX(env1, 0.5);
			}
			if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
			{
				count = 1;
				debugprogram.setUniform("u_mode", 6);
				f0 = 5.5f / 8.0f;
				program.setUniform("u_F0", f0);
				//env1 = renderer.blurY(env2, 0.6);
				//env1 = renderer.blurX(env1, 0.6);
			}
			if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
			{
				count = 1;
				debugprogram.setUniform("u_mode", 7);
				f0 = 6.5f / 8.0f;
				program.setUniform("u_F0", f0);
				//env1 = renderer.blurY(env2, 0.7);
				//env1 = renderer.blurX(env1, 0.7);
			}
			if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
			{
				count = 1;
				debugprogram.setUniform("u_mode", 8);
				f0 = 7.5f / 8.0f;
				program.setUniform("u_F0", f0);
				//env1 = renderer.blurY(env2, 0.8);
				//env1 = renderer.blurX(env1, 0.8);
			}
			if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
			{
				count = 1;
				debugprogram.setUniform("u_mode", 9);
				f0 = 1.0f;
				program.setUniform("u_F0", f0);
				//env1 = renderer.blurY(env2, 1.0f);
				//env1 = renderer.blurX(env1, 1.0f);
			}

			if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
			{
				count = 1;
				f0 += 0.0005f;
				f0 = glm::clamp(f0, 0.0f, 0.9999f);
				program.setUniform("u_F0", f0);
			}
			
			if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			{
				count = 1;
				f0 -= 0.0005f;
				f0 = glm::clamp(f0, 0.0f, 0.9999f);
				program.setUniform("u_F0", f0);
			}

			//draw triangle
			glm::mat4 lr = light_rotation_mat;
			glm::mat4 lt = light_translate_mat;
			light_rotation_mat = lightRotationControl(light_rotation_mat, window, WindowWidth, WindowHeight, sensitivity);
			light_translate_mat = lightTranslateControl(light_translate_mat, window);
			if ((lr != light_rotation_mat) || (lt != light_translate_mat))
			{
				count = 1;
			}
			lightPos = light_rotation_mat * light_translate_mat * glm::vec4(box.position(), 1.0f);
			program.setUniform("u_lightPos", lightPos);

			lightprogram.setUniform("u_modelMatrix", light_rotation_mat * light_translate_mat);

			renderer.clearColor(clearcolor);
			renderer.clear();
			renderer.clear(camera.framebuffer());
			renderer.clearColor(clearalpha);

			if ((glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) && (!tab_is_pressed))
			{
				count = 1;
				tab_is_pressed = true;
				tab = !tab;
			}
			if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE)
			{
				tab_is_pressed = false;
			}
			if (tab)
			{
				//program.setUniform("u_F1", 1);
				//renderer.drawTris(VAO, EBO, program, camera);
				renderer.drawTris(VAO, EBO, debugprogram, camera);
			}
			else
			{
				//program.setUniform("u_F1", 2);
				renderer.drawTris(VAO, EBO, program, camera);
			}
			//renderer.drawTris(VAO, EBO, normals_program, camera);
			//renderer.drawTris(VAObox, EBObox, lightprogram, camera);
			renderer.drawLines(VAOax, EBOax, axisprogram, camera);
			if (!(count-1))
			{
				renderer.clear(camera.framebuffer(1));
				camera.setActiveOffset(0);
			}
			if(count < camera.offsetSize())
			{
				AAprogram.setUniform("u_count", count);
				AAprogram.setUniform("u_frameBuffer1", camera.framebuffer((count % 2) + 1).texture());
				renderer.drawShader(AAprogram, camera.framebuffer(((count + 1) % 2) + 1));
				postprogram.setUniform("u_frameBuffer", camera.framebuffer(((count + 1) % 2) + 1).texture());
				renderer.drawShader(postprogram);
				camera.setActiveOffset((camera.activeOffset()+1)%camera.offsetSize());
				count++;
			}
			else
			{
				renderer.drawShader(postprogram);
			}
			//postprogram.setUniform("u_kernel", env3);
			//renderer.drawShader(postprogram);
		}
		//renderer.saveImage("Resources/Renders/render01.png");

	}
	//destroy the created context window
	glfwDestroyWindow(window);
	//terminate glfw
	glfwTerminate();


	return 0;
}

glm::mat4 lightRotationControl(glm::mat4 mat, GLFWwindow* window, int width, int height, float sensitivity)
{
	constexpr double PI = 3.141592653589793238462643383;
	static double posX, posY, initX, initY;
	static bool clicked = false;
	float snstvt = sensitivity * 5;
	glm::vec3 rigt = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	using namespace std;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		if (!clicked)
		{
			if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
			{
				glfwGetCursorPos(window, &initX, &initY);
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
				glfwSetCursorPos(window, width / 2.0, height / 2.0);
				clicked = true;
			}
		}
		else
		{
			glfwGetCursorPos(window, &posX, &posY);
			glfwSetCursorPos(window, width / 2.0, height / 2.0);
			posX = glm::clamp((((posX / double(width)) * 2.0) - 1.0) * snstvt, -5.0, 5.0);
			posY = glm::clamp((((posY / double(height)) * 2.0) - 1.0) * snstvt * height / width, -5.0, 5.0);
			mat = glm::translate(mat, float(-posY/5) *  up);
			mat = glm::rotate(mat, glm::radians(float(posX*PI/10)), up);
		}
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE && clicked)
	{
		glfwSetCursorPos(window, initX, initY);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		clicked = false;
	}
	return mat;
}

glm::mat4 lightTranslateControl(glm::mat4 mat, GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
	{
		auto loc = glm::vec3(0, 0, -1);
		mat = glm::translate(mat, loc * static_cast<float>(scrollY));
	}
	scrollY = 0.0;
	return mat;
}


float* getWorldAxis()
{
	glm::vec3 o = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 x = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 y = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 z = glm::vec3(0.0f, 0.0f, 1.0f);
	float* verts = new float[] {o.x, o.y, o.z, 0.75f, 0.25f, 0.25f,
								x.x, x.y, x.z, 1.0f, 0.5f, 0.5f,
								o.x, o.y, o.z, 0.25f, 0.75f, 0.25f,
								y.x, y.y, y.z, 0.5f, 1.0f, 0.5f,
								o.x, o.y, o.z, 0.25f, 0.25f, 0.75f,
								z.x, z.y, z.z, 0.5f, 0.5f, 1.0f};
	return verts;
}

