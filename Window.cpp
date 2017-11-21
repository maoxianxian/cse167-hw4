#define _USE_MATH_DEFINES
#include "window.h"
#include <math.h>
#include "Geometry.h"
#include "Transform.h"
#include "robot.h"
#include <vector>
#include "OBJObject.h"
const char* window_title = "GLFW Starter Project";
GLint shaderProgram;
GLint shader2;
GLint shader3;
GLuint VBO, VAO, VAO2,VBO2;
bool forward = true;
float velocity=1;
bool rider = false;
bool pause = false;
float totalenegy = 10.0f;
Cube *cbe;
// On some systems you need to change this to the absolute path
#define VERTEX_SHADER_PATH "../shader.vert"
#define FRAGMENT_SHADER_PATH "../shader.frag"
#define SHADER2_PATH "../shader2.vert"
#define SHADER3_PATH "../shader3.vert"
#define FRAG2_PATH "../shader2.frag"
#define FRAG3_PATH "../shader3.frag"
// Default camera parameters
glm::vec3 cam_pos(0.0f, 0.0f, 20.0f);		
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			

int Window::width;
int Window::height;
double prexpos, preypos, prerightx, prerighty;
glm::mat4 Window::P;
glm::mat4 Window::V;

int index = 1;
float count = 0;
Transform * root;
std::vector<Transform*> controlpoints;
int currentpoint;
float totallength;
std::vector<glm::vec3> linevertices;
std::vector<float> curvelength;
std::vector<glm::vec3> controlhandles;
Geometry * point;
Transform*car;
void Window::initialize_objects()
{
	root = new Transform(glm::mat4(1.0f));
	loadTexture();
	//  the shader program. Make sure you have the correct filepath up top
	shaderProgram = LoadShaders(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
	shader2 = LoadShaders(SHADER2_PATH, FRAG2_PATH);
	shader3 = LoadShaders(SHADER3_PATH, FRAG3_PATH);
	point = new Geometry("C:\\Users\\c7ye\\Desktop\\CSE167StarterCode2-master\\sphere.obj");
	car = new Transform(glm::mat4(1.0f));
	car->addChild(point);
	car->scale(5, 5, 5);
	for (int i = 0; i < 24; i++)
	{
		Transform * temp = new Transform(glm::mat4(1.0f));
		if (i % 3 == 1)
		{
			temp->color =glm::vec3(1,0,(float)i/255.0f);
		}
		else
		{
			temp->color = glm::vec3(0, 1, (float)i/255.0f);
		}
		temp->addChild(point);
		temp->translate((i) * 0.5, 0, 0);
		temp->scale(0.1, 0.1, 0.1);
		controlpoints.push_back(temp);
		root->addChild(temp);
	}
	for (int i = 0; i < 8; i++) {
		glm::vec4 p0 = controlpoints[i * 3+1]->toParent[3];
		glm::vec4 p1 = controlpoints[i * 3 + 2]->toParent[3];
		glm::vec4 p2 = controlpoints[(i * 3 + 3)%24]->toParent[3];
		glm::vec4 p3 = controlpoints[(i * 3 + 4) % 24]->toParent[3];
		float length=0;
		glm::vec3 pre;
		for (int j = 0; j < 200; j++)
		{
			float t = (float)j / 200.0f;
			glm::vec3 temp = pow(1 - t, 3)*p0 + 3 * pow(1 - t, 2)*t*p1 + 3 * (1 - t)*t*t*p2 + t*t*t*p3;
			if (j != 0)
			{
				length+=glm::length(pre - temp);
			}
			linevertices.push_back(temp);
			pre = temp;
		}
		curvelength.push_back(length);
		totallength += length;
	}
	//std::cout << curvelength[0] << " " << curvelength[1] << " " << curvelength[7]<<" "<<totallength << std::endl;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*linevertices.size(), &linevertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		3 * sizeof(GLfloat), // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	for (int i = 0; i < controlpoints.size(); i++)
	{
		if (i % 3 != 1)
		{
			controlhandles.push_back(controlpoints[i]->toParent[3]);
		}
	}

	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);
	glBindVertexArray(VAO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*controlhandles.size(), &controlhandles[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		3 * sizeof(GLfloat), // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	cbe= new Cube();
}

// Treat this as a destructor function. Delete dynamically allocated memory here.
void Window::clean_up()
{
	glDeleteProgram(shaderProgram);
	controlpoints.clear();
	linevertices.clear();
	controlhandles.clear();
}

unsigned char* Window::loadPPM(const char* filename, int& width, int& height)
{
	const int BUFSIZE = 128;
	FILE* fp;
	unsigned int read;
	unsigned char* rawData;
	char buf[3][BUFSIZE];
	char* retval_fgets;
	size_t retval_sscanf;
	if ((fp = fopen(filename, "rb")) == NULL)
	{
		std::cerr << "error reading ppm file, could not locate " << filename << std::endl;
		width = 0;
		height = 0;
		return NULL;
	}
	// Read magic number:
	retval_fgets = fgets(buf[0], BUFSIZE, fp);
	// Read width and height:
	do
	{
		retval_fgets = fgets(buf[0], BUFSIZE, fp);
	} while (buf[0][0] == '#');
	retval_sscanf = sscanf(buf[0], "%s %s", buf[1], buf[2]);
	width = atoi(buf[1]);
	height = atoi(buf[2]);

	// Read maxval:F
	do
	{
		retval_fgets = fgets(buf[0], BUFSIZE, fp);
	} while (buf[0][0] == '#');
	// Read image data:
	rawData = new unsigned char[width * height * 3];
	read = fread(rawData, width * height * 3, 1, fp);
	fclose(fp);
	if (read != 1)
	{
		std::cerr << "error parsing ppm file, incomplete data" << std::endl;
		delete[] rawData;
		width = 0;
		height = 0;
		return NULL;
	}
	return rawData;
}
GLuint Window::loadTexture()
{
	std::vector<std::string> faces
	{
		"../right.ppm",
			"../left.ppm",
			"../top.ppm",
			"../base.ppm",
			"../back.ppm",
			"../front.ppm"
	};
	GLuint texture;     // storage for one texture
	int twidth, theight;   // texture width/height [pixels]
	unsigned char* tdata;  // texture pixel data
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	for (int i = 0; i < 6; i++)
	{
		tdata = loadPPM(faces[i].c_str(), twidth, theight); //std::cout << tdata << std::endl;
		if (tdata == NULL) return 0;
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, tdata);
	}
	return texture;
}

GLFWwindow* Window::create_window(int width, int height)
{
	// Initialize GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return NULL;
	}

	// 4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ // Because Apple hates comforming to standards
	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window
	GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);

	// Check if the window could not be created
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		fprintf(stderr, "Either GLFW is not installed or your graphics card does not support modern OpenGL.\n");
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window
	glfwMakeContextCurrent(window);

	// Set swap interval to 1
	glfwSwapInterval(1);

	// Get the width and height of the framebuffer to properly resize the window
	glfwGetFramebufferSize(window, &width, &height);
	// Call the resize callback to make sure things get drawn immediately
	Window::resize_callback(window, width, height);

	return window;
}

void Window::resize_callback(GLFWwindow* window, int width, int height)
{
#ifdef __APPLE__
	glfwGetFramebufferSize(window, &width, &height); // In case your Mac has a retina display
#endif
	Window::width = width;
	Window::height = height;
	// Set the viewport size. This is the only matrix that OpenGL maintains for us in modern OpenGL!
	glViewport(0, 0, width, height);

	if (height > 0)
	{
		P = glm::perspective(45.0f, (float)width / (float)height, 1.0f, 1000.0f);
		V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	}
}

void Window::idle_callback()
{
	// Call the update function the cube
}
void Window::mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		glfwGetCursorPos(window, &prexpos, &preypos);
		unsigned char pix[4];
		glReadPixels(prexpos, height-preypos,1,1,GL_RGBA,GL_UNSIGNED_BYTE,&pix);
		if (pix[1] == 0 || pix[0] == 0 && pix[2] < 24)
		{
			currentpoint = (int)(pix[2]);
		}
		else
		{
			currentpoint = -1;
		}
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		glfwGetCursorPos(window, &prerightx, &prerighty);
	}
}
void Window::drawCurves()
{
	glUniform3fv(glGetUniformLocation(shader2, "id"), 1, &glm::vec3(0,0,0)[0]);
	glm::mat4 modelview = V;
	GLuint uProjection = glGetUniformLocation(shader2, "projection");
	GLuint uModelview = glGetUniformLocation(shader2, "modelview");
	//std::cout <<P[0][0]<<std::endl;

	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &P[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
	glBindVertexArray(VAO);
	glDrawArrays(GL_LINE_STRIP, 0, 1600);
	glBindVertexArray(0);
	
	glUniform3fv(glGetUniformLocation(shader2, "id"), 1, &glm::vec3(0, 0, 1)[0]);
	glBindVertexArray(VAO2);
	//std::cout << controlhandles.size() << std::endl;
	glDrawArrays(GL_LINES, 0, 16);
	glBindVertexArray(0);


}
void Window::display_callback(GLFWwindow* window)
{
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use the shader of programID
	glUseProgram(shaderProgram);
	cbe->draw(shaderProgram);
	glUseProgram(shader2);
	drawCurves();
	root->draw(shader2,glm::mat4(1.0f));
	glUseProgram(shader3);
	glUniformMatrix4fv(glGetUniformLocation(shader3, "view"), 1, GL_FALSE,&V[0][0]);
	glUniform3fv(glGetUniformLocation(shader3, "cameraPos"), 1, &cam_pos[0]);
	int i = (int)((int)count / 200);
	glm::vec4 p0 = controlpoints[i*3+1]->toParent[3];
	glm::vec4 p1 = controlpoints[i*3+2]->toParent[3];
	glm::vec4 p2 = controlpoints[(i*3+3) % 24]->toParent[3];
	glm::vec4 p3 = controlpoints[(i*3+4) % 24]->toParent[3];
	float t = float(count) / 200.0f - i;
	glm::vec3 temp = pow(1 - t, 3)*p0 + 3 * pow(1 - t, 2)*t*p1 + 3 * (1 - t)*t*t*p2 + t*t*t*p3;
	car->toParent = glm::translate(glm::mat4(1.0f), temp);
	if (!rider) {
		car->draw(shader3, glm::mat4(1.0f));
	}
	if (totalenegy * 2 - 2 * car->toParent[3][1] > 0.3) {
		velocity = sqrt(totalenegy * 2 - 2 * car->toParent[3][1])*0.1f*(totallength/curvelength[i]);
	}
	else
	{
		velocity = 0.2;
	}
	/*if (totalenegy * 2 - 2 * car->toParent[3][1] > 0) {
		if (forward)
		{
			velocity = sqrt(totalenegy * 2 - 2 * car->toParent[3][1])*0.1f*(totallength/curvelength[i]);
		}
		else
		{
			velocity = -sqrt(totalenegy * 2 - 2 * car->toParent[3][1])*0.1f*(totallength / curvelength[i]);
		}
	}
	else
	{
		forward = !forward;
		if (forward)
		{
			velocity = sqrt(-(totalenegy * 2 - 2 * car->toParent[3][1]))*0.1f*(totallength / curvelength[i]);
		}
		else
		{
			velocity = -sqrt(-(totalenegy * 2 - 2 * car->toParent[3][1]))*0.1f*(totallength / curvelength[i]);
		}
	}*/
	if (pause)
	{
		velocity = 0;
	}
	// Render the cube	
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if (state == GLFW_PRESS)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		if (currentpoint>=0&&(xpos != prexpos || ypos != preypos))
		{
			if (currentpoint % 3 == 1)
			{
				controlpoints[currentpoint]->translate(0.03*(xpos - prexpos), 0.03*(preypos - ypos), 0);

				controlpoints[(currentpoint + 1) % 24]->translate(0.03f*(xpos - prexpos), 0.03f*(preypos - ypos), 0);
				controlpoints[(currentpoint + 23) % 24]->translate(0.03f*(xpos - prexpos), 0.03f*(preypos - ypos), 0);
			}
			else if (currentpoint % 3 == 2)
			{
				glm::vec3 mid = controlpoints[(currentpoint + 23) % 24]->toParent[3];
				glm::vec3 pre = controlpoints[(currentpoint + 22) % 24]->toParent[3];
				glm::vec3 cur = controlpoints[(currentpoint)]->toParent[3];
				controlpoints[currentpoint]->translate(0.03*(xpos - prexpos), 0.03*(preypos - ypos), 0);

				float midtocurr = glm::length(mid - cur);
				float midtopre = glm::length(mid - pre);
				cur = controlpoints[(currentpoint)]->toParent[3];
				root->removeChild(controlpoints[(currentpoint + 22) % 24]);
				controlpoints[(currentpoint + 22) % 24] = new Transform(glm::translate(glm::mat4(1.0f),midtopre*glm::normalize(mid - cur) + mid));
				controlpoints[(currentpoint + 22) % 24]->addChild(point);
				controlpoints[(currentpoint + 22) % 24]->scale(0.1,0.1,0.1);
				controlpoints[(currentpoint + 22) % 24]->color= glm::vec3(0, 1, (float)((currentpoint + 22) % 24)/ 255.0f);

				root->addChild(controlpoints[(currentpoint + 22) % 24]);
				//float xdis = -midtopre*(0.03*(xpos - prexpos) / midtocurr);
				//float ydis = -midtopre*(0.03*(preypos-ypos) / midtocurr);
				//controlpoints[(currentpoint + 22) % 24]->translate(xdis, ydis, 0);

			}
			else if (currentpoint % 3 == 0)
			{

				glm::vec3 mid = controlpoints[(currentpoint + 1) % 24]->toParent[3];
				glm::vec3 nex = controlpoints[(currentpoint + 2) % 24]->toParent[3];
				glm::vec3 cur = controlpoints[(currentpoint)]->toParent[3];
				controlpoints[currentpoint]->translate(0.03*(xpos - prexpos), 0.03*(preypos - ypos), 0);

				float midtocurr = glm::length(mid - cur);
				float midtopre = glm::length(mid - nex);
				
				cur = controlpoints[(currentpoint)]->toParent[3];
				root->removeChild(controlpoints[(currentpoint + 2) % 24]);
				controlpoints[(currentpoint + 2) % 24] = new Transform(glm::translate(glm::mat4(1.0f), midtopre*glm::normalize(mid - cur) + mid));
				controlpoints[(currentpoint + 2) % 24]->addChild(point);
				controlpoints[(currentpoint + 2) % 24]->scale(0.1, 0.1, 0.1);
				controlpoints[(currentpoint + 2) % 24]->color = glm::vec3(0, 1, (float)((currentpoint + 2) % 24) / 255.0f);

				root->addChild(controlpoints[(currentpoint + 2) % 24]);
				//float xdis = -midtopre*(0.03*(xpos - prexpos) / midtocurr);
				//float ydis = -midtopre*(0.03*(preypos - ypos) / midtocurr);
				//controlpoints[(currentpoint + 2) % 24]->translate(xdis, ydis, 0);
			}

			linevertices.clear();
			curvelength.clear();
			totallength = 0;
			for (int i = 0; i < 8; i++) {
				glm::vec4 p0 = controlpoints[i * 3 + 1]->toParent[3];
				glm::vec4 p1 = controlpoints[i * 3 + 2]->toParent[3];
				glm::vec4 p2 = controlpoints[(i * 3 + 3) % 24]->toParent[3];
				glm::vec4 p3 = controlpoints[(i * 3 + 4) % 24]->toParent[3];
				float length = 0;
				glm::vec3 pre;
				for (int j = 0; j < 200; j++)
				{
					float t = (float)j / 200.0f;
					glm::vec3 temp = pow(1 - t, 3)*p0 + 3 * pow(1 - t, 2)*t*p1 + 3 * (1 - t)*t*t*p2 + t*t*t*p3;
					if (j != 0)
					{
						length += glm::length(pre - temp);
					}
					linevertices.push_back(temp);
					pre = temp;
				}
				curvelength.push_back(length);
				totallength += length;
			}
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*linevertices.size(), &linevertices[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
				3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
				GL_FLOAT, // What type these components are
				GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
				3 * sizeof(GLfloat), // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
				(GLvoid*)0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);

			controlhandles.clear();
			for (int i = 0; i < controlpoints.size(); i++)
			{
				if (i % 3 != 1)
				{
					controlhandles.push_back(controlpoints[i]->toParent[3]);
				}
			}
			glBindVertexArray(VAO2);
			glBindBuffer(GL_ARRAY_BUFFER, VBO2);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*controlhandles.size(), &controlhandles[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
				3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
				GL_FLOAT, // What type these components are
				GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
				3 * sizeof(GLfloat), // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
				(GLvoid*)0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
		prexpos = xpos;
		preypos = ypos;
	}
	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
	// Swap buffers
	glfwSwapBuffers(window);
	count+=velocity;
	if (count > 1600)
	{
		count -= 1600;
	}
	if (count < 0)
	{
		count += 1600;
	}
	if (rider)
	{
		cam_pos = pow(1 - t, 3)*p0 + 3 * pow(1 - t, 2)*t*p1 + 3 * (1 - t)*t*t*p2 + t*t*t*p3;
		cam_look_at = glm::vec3(-3 * (1 - t)*(1 - t)*p0 + (9 * t*t - 12 * t + 3)*p1 + (-9 * t*t + 6 * t)*p2 + 3 * t*t*p3) + cam_pos;
		cam_up = glm::cross(glm::vec3(-3 * (1 - t)*(1 - t)*p0 + (9 * t*t - 12 * t + 3)*p1 + (-9 * t*t + 6 * t)*p2 + 3 * t*t*p3), glm::vec3(0, 0, -1));
		V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	}
	//std::cout<<cam_
}
glm::vec3 Window::trackmap(double x, double y)
{
	glm::vec3 result( (2.0f * x - width) / width, (height- 2.0f*y)/height, 0.0f);
	float length = glm::length(result);
	if (length > 1.0f)
	{
		length = 1.0;
	}
	result[2] = sqrt(1.001 - length*length);
	return glm::normalize(result);
}
void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Check for a key press
	if (action == GLFW_PRESS)
	{
		// Check if escape was pressed
		if (key == GLFW_KEY_ESCAPE)
		{
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		if (key == GLFW_KEY_P)
		{
			pause = !pause;
		}
		if (key == GLFW_KEY_C)
		{
			if (rider)
			{
				cam_pos=glm::vec3(0.0f, 0.0f, 20.0f);
				cam_look_at=glm::vec3(0.0f, 0.0f, 0.0f);
				cam_up=glm::vec3(0.0f, 1.0f, 0.0f);
				V = glm::lookAt(cam_pos, cam_look_at, cam_up);
			}
			else
			{
				int i = (int)((int)count / 200);
				glm::vec4 p0 = controlpoints[i * 3 + 1]->toParent[3];
				glm::vec4 p1 = controlpoints[i * 3 + 2]->toParent[3];
				glm::vec4 p2 = controlpoints[(i * 3 + 3) % 24]->toParent[3];
				glm::vec4 p3 = controlpoints[(i * 3 + 4) % 24]->toParent[3];
				float t = float(count) / 200.0f - i;
				cam_pos = pow(1 - t, 3)*p0 + 3 * pow(1 - t, 2)*t*p1 + 3 * (1 - t)*t*t*p2 + t*t*t*p3;
				cam_look_at = glm::vec3(-3 * (1 - t)*(1 - t)*p0 + (9 * t*t - 12 * t + 3)*p1 + (-9 * t*t + 6 * t)*p2 + 3 * t*t*p3)+cam_pos;
				cam_up = glm::cross(cam_look_at, glm::vec3(0, 0, -1));
				V = glm::lookAt(cam_pos, cam_look_at, cam_up);
			}
			rider = !rider;
		}
	}
	if (action == GLFW_REPEAT)
	{
	}
}
