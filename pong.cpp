#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <array>
#include <vector>

namespace Game {
	enum Object {
		leftPong,	// 0
		rightPong,	// 12
		ball,		// 24
		max_object,
	};

	enum class Movement {
		left_up,
		left_down,
		right_up,
		right_down,
		max_movement,
	};
}

constexpr std::array g_vertices{
	// Left Pong, indices 0-11
	-0.91f, -0.2f, 0.0f, // bottomLeft
	-0.89f, -0.2f, 0.0f, // BottomRight
	-0.91f, 0.2f, 0.0f, // topLeft
	-0.89f, 0.2f, 0.0f, // topRight

	// Right Pong, indices 12-23
	0.91f, -0.2f, 0.0f, // bottomLeft
	0.89f, -0.2f, 0.0f, // BottomRight
	0.91f, 0.2f, 0.0f, // topLeft
	0.89f, 0.2f, 0.0f, // topRight

	// Ball, indices 24-35
	-0.01f, -0.02f, 0.0f, // bottomleft
	0.01f, -0.02f, 0.0f, // bottomRight
	-0.01f, 0.02f, 0.0f, // topLeft
	0.01f, 0.02f, 0.0f, // topRight
};


void inputHandling(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

Game::Movement leftMovement(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		return Game::Movement::left_up;

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		return Game::Movement::left_down;

	return Game::Movement::max_movement;
}

Game::Movement rightMovement(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		return Game::Movement::right_up;

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		return Game::Movement::right_down;

	return Game::Movement::max_movement;
}

void framebufferCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

int main() {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(720, 540, "Pong!", NULL, NULL);
	glfwMakeContextCurrent(window);

	if (window == NULL) {
		std::cout << "ERROR: no glfw load\n\b";
		glfwTerminate();
		return -1;
	}

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "ERROR: No glad load\n";
		return -1;
	}

	glViewport(0, 0, 720, 540);
	glfwSetFramebufferSizeCallback(window, framebufferCallback);

	glfwSwapInterval(1); // for limiting fps, making the game playable

	unsigned int vertexShader{ glCreateShader(GL_VERTEX_SHADER) };
	const char* vertexSource{
		"# version 330 core\n"
		"layout(location = 0) in vec3 inputVertex;\n"
		"void main() {\n"
		"gl_Position = vec4(inputVertex.x, inputVertex.y, inputVertex.z, 1.0f);\n"
		"}\0"
	};
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	// create the fragment shader that colours the pixels blue
	unsigned int fragmentShader_blue{ glCreateShader(GL_FRAGMENT_SHADER) };
	const char* fragmentSource_blue{
		"# version 330 core\n"
		"out vec4 fragmentBlue;\n"
		"void main() {\n"
		"fragmentBlue = vec4(0.2f, 0.6f, 0.8f, 1.0f);\n"
		"}\0"
	};
	glShaderSource(fragmentShader_blue, 1, &fragmentSource_blue, NULL);
	glCompileShader(fragmentShader_blue);

	// program with the blue fragment shader
	unsigned int shaderProgram_1{ glCreateProgram() };
	glAttachShader(shaderProgram_1, vertexShader);
	glAttachShader(shaderProgram_1, fragmentShader_blue);

	glLinkProgram(shaderProgram_1);

	glDeleteShader(fragmentShader_blue);

	// fragment shader that colours the pixels yellow
	const char* fragmentSource_yellow{
		"# version 330 core\n"
		"out vec4 fragmentYellow;\n"
		"void main() {\n"
		"fragmentYellow = vec4(1.0f, 1.0f, 0.0f, 1.0f);\n"
		"}\0"
	};

	unsigned int fragmentShader_yellow{ glCreateShader(GL_FRAGMENT_SHADER) };
	glShaderSource(fragmentShader_yellow, 1, &fragmentSource_yellow, NULL);
	glCompileShader(fragmentShader_yellow);

	// shader program with the yellow fragment shader
	// both programs use the same vertex shader
	unsigned int shaderProgram_2{ glCreateProgram() };
	glAttachShader(shaderProgram_2, vertexShader);
	glAttachShader(shaderProgram_2, fragmentShader_yellow);
	glLinkProgram(shaderProgram_2);

	glDeleteShader(fragmentShader_yellow);

	const char* fragmentSource_white{
		"# version 330 core\n"
		"out vec4 fragmentYellow;\n"
		"void main() {\n"
		"fragmentYellow = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
		"}\0"
	};
	unsigned int fragmentShader_white{ glCreateShader(GL_FRAGMENT_SHADER) };
	glShaderSource(fragmentShader_white, 1, &fragmentSource_white, NULL);
	glCompileShader(fragmentShader_white);

	unsigned int shaderProgram_3{ glCreateProgram() };
	glAttachShader(shaderProgram_3, vertexShader);
	glAttachShader(shaderProgram_3, fragmentShader_white);

	glLinkProgram(shaderProgram_3);

	glDeleteShader(fragmentShader_white);

	// delete vertex shader only after its been used on both programs
	glDeleteShader(vertexShader);

	std::array vertices{ g_vertices };

	// the indices are the same for the left and right pong, and ball, so ill use em for both VAOs
	constexpr std::array<unsigned int, 6> indices{
		0, 1, 2,
		1, 2, 3,
	};

	// arrays work, less code to write
	unsigned int VAO[3]{};
	unsigned int VBO[3]{};

	// EBO will be used by both pongs
	unsigned int EBO{};
	glGenBuffers(1, &EBO);

	glGenVertexArrays(3, VAO);
	glGenBuffers(3, VBO);

	// Game::Object * 12 returns the first element of each object: either of leftPong, rightPong or ball
	// the VAO and VBO stuff could be reduces with a for loop but keeping it like this since don't want to + the comments
	// left pong
	glBindVertexArray(VAO[Game::leftPong]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[Game::leftPong]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) / 3, &vertices[(Game::leftPong * 12)], GL_DYNAMIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, 0, 3 * sizeof(float), static_cast<void*>(0));
	glEnableVertexAttribArray(0);

	// the reason unbinding the VBO before VAO is okay but unbinding the EBO before VAO isnt okay is that
	// glVertexAttribPointer already binds/remembers the VBO, so even if you unbind the VBO, through the vertexAttrib its ok
	// this doesnt happen for the EBO, so when you unbind it, the VAO remembers it. when we try drawing, it tries to accesses a 0 pointer and crashes
	glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind VBO[0]
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind EBO; do not do this, im guessing the VAO will register this over the EBO one earlier and thus crash out

	glBindVertexArray(0); // unbind VAO[0]
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// right pong
	glBindVertexArray(VAO[Game::rightPong]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[Game::rightPong]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) / 3, &vertices[(Game::rightPong * 12)], GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, 0, 3 * sizeof(float), (void*)0); // this
	glEnableVertexAttribArray(0); // and this need to be done for every vao/vbo. enabling it for one doesnt enable it for the other

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // same here, do not unbind
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//ball
	glBindVertexArray(VAO[Game::ball]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[Game::ball]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) / 3, &vertices[(Game::rightPong * 12)], GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, 0, 3 * sizeof(float), static_cast<void*>(0));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	/*unsigned int VBO_2{};
	glGenBuffers(1, &VBO_2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_2); // change binded VBO to the second set of vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_2), &vertices_2, GL_STATIC_DRAW);
	*/

	/* // for EBO / indexed drawing
	unsigned int EBO{}; // indices
	glGenBuffers(1, &EBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW); // send EBO data to the GPU
	*/

	// we typically dont wanna unbind VAOs and VBOs. for VAOs its alright, since it prevents stuff like other calls being binded to the vao, although its rare
	// the VBO unbind is alright since glVertexAttribPointer already registered the VBO to the vertex attributes
	//glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind VBO
	//glBindVertexArray(0); // unbind VAO
	// glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind EBO

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	float epsilon{ 0.01f };	

	float direction_axisX{ 1.0f };
	float direction_axisY{ -1.0f };
	Game::Movement leftInput{};
	Game::Movement rightInput{};
	int leftScore{ 0 };
	int rightScore{ 0 };
	float speedBall{ 0.01f };
	float speedPong{ 0.0125f };
	while (!glfwWindowShouldClose(window)) {
		inputHandling(window);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// left pong input
		leftInput = leftMovement(window);
		if (leftInput != Game::Movement::max_movement) {
			// vertices[(Game::leftPong * 12) + 7] = top
			if (leftInput == Game::Movement::left_up && vertices[(Game::leftPong * 12) + 7] <= 1.0f)
				for (std::size_t index{ (Game::leftPong * 12) + 1 }; index < (Game::leftPong * 12) + 11; index += 3)
					vertices[index] += speedPong;
			// vertices[(Game::leftPong * 12) + 1] = bottom
			if (leftInput == Game::Movement::left_down && vertices[(Game::leftPong * 12) + 1] >= -1.0f)
				for (std::size_t index{ 1 }; index < 11; index += 3)
					vertices[index] -= speedPong;
			
			glBindBuffer(GL_ARRAY_BUFFER, VBO[Game::leftPong]);

			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) / 3, &vertices[(Game::leftPong * 12)], GL_DYNAMIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		// right pong input
		rightInput = rightMovement(window);
		if (rightInput != Game::Movement::max_movement) {
			// vertices[(Game::rightPong * 12) + 7] = top
			if (rightInput == Game::Movement::right_up && vertices[(Game::rightPong * 12) + 7] <= 1.0f)
				for (std::size_t index{ (Game::rightPong * 12) + 1 }; index < (Game::rightPong * 12) + 11; index += 3)
					vertices[index] += speedPong;
			// vertices[(Game::rightPong * 12) + 1] = bottom
			if (rightInput == Game::Movement::right_down && vertices[(Game::rightPong * 12) + 1] >= -1.0f)
				for (std::size_t index{ (Game::rightPong * 12) + 1 }; index < (Game::rightPong * 12) + 11; index += 3)
					vertices[index] -= speedPong;
		
			glBindBuffer(GL_ARRAY_BUFFER, VBO[Game::rightPong]);

			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) / 3, &vertices[(Game::rightPong * 12)], GL_DYNAMIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		
		// left pong
		// use the blue shader program
		glUseProgram(shaderProgram_1);

		glBindVertexArray(VAO[Game::leftPong]);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);

		// right pong
		// use the yellow shader program
		glUseProgram(shaderProgram_2);

		glBindVertexArray(VAO[Game::rightPong]);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);

		bool leftPong_collision_axisX{ ( (vertices[(Game::leftPong * 12) + 3] - vertices[(Game::ball * 12)]) > epsilon * -1 ) && vertices[(Game::ball * 12)] >= vertices[(Game::leftPong * 12)] }; // left-side of ball is in the same X axis as the left pong
		bool leftPong_collision_axisY{ vertices[(Game::ball * 12) + 1] <= vertices[(Game::leftPong * 12) + 10] && vertices[(Game::ball * 12) + 1] >= vertices[(Game::leftPong * 12) + 4]};

		// no clue why vertices[(Game::ball * 12)] <= vertices[(Game::rightPong * 12) + 3] instead of vertices[(Game::ball * 12) + 3] <= vertices[(Game::rightPong * 12) + 3] but it works
		bool rightPong_collision_axisX{ ( (vertices[(Game::rightPong * 12)] - vertices[(Game::ball * 12) + 3]) < epsilon ) && (vertices[(Game::ball * 12)] <= vertices[(Game::rightPong * 12) + 3]) }; // left-side of ball is in the same X axis as the left pong
		bool rightPong_collision_axisY{ vertices[(Game::ball * 12) + 4] <= vertices[(Game::rightPong * 12) + 7] && vertices[(Game::ball * 12) + 1] >= vertices[(Game::rightPong * 12) + 1] };

		glBindBuffer(GL_ARRAY_BUFFER, VBO[Game::ball]);
		if (	(leftPong_collision_axisX && leftPong_collision_axisY) ||
				(rightPong_collision_axisX && rightPong_collision_axisY)	)
			direction_axisX *= -1.0f;

		if (vertices[(Game::ball * 12) + 1] <= -1.0f || vertices[(Game::ball * 12) + 7] >= 1.0f)
			direction_axisY *= -1.0f;

		for (std::size_t index{ (Game::ball * 12) }; index < (Game::ball * 12) + 10; index += 3)
			vertices[index] += speedBall * direction_axisX;

		for (std::size_t index{ (Game::ball * 12) + 1 }; index < (Game::ball * 12) + 11; index += 3)
			vertices[index] += speedBall * direction_axisY;

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) / 3, &vertices[(Game::ball * 12)], GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);


		// ball
		glUseProgram(shaderProgram_3);

		glBindVertexArray(VAO[Game::ball]);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);

		// Adds score to the right side if it scores one against the left
		if (vertices[(Game::ball * 12) + 3] <= -1.0f)
			++rightScore;

		// Adds score to the left side if it scores one against the right
		if (vertices[(Game::ball * 12)] >= 1.0f)
			++leftScore;

		// if the ball goes past the left or right pong, set everything to its original position
		if (vertices[(Game::ball * 12) + 3] <= -1.0f || vertices[(Game::ball * 12)] >= 1.0f) {
			for (std::size_t i{ 0 }; i < 36; ++i)
				vertices[i] = g_vertices[i];

			// Set speed back to its original value
			speedBall = 0.01f;
			speedPong = 0.02f;
			epsilon = 0.01f;

			std::cout << "Score left side: " << leftScore << "\t\tScore right side: " << rightScore << "\n\n";


			// loop to bind the VBO of each object and pass in the original vertex data
			for (std::size_t i{ 0 }; i < 3; ++i) {
				glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) / 3, &vertices[(i * 12)], GL_DYNAMIC_DRAW);
			}

			glBindBuffer(GL_ARRAY_BUFFER, 0);

			direction_axisX = 1.0f;
			direction_axisY = -1.0f;
		}

		speedBall += 0.00001f; // slightly increment the balls speed every frame
		speedPong += 0.00001f;
		epsilon += 0.00001f;  // increment epsilon alongside the ball, otherwise it will clip through the pongs


		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	// optionally delete VAO, VBO and shader program
	glDeleteProgram(shaderProgram_1);
	glDeleteProgram(shaderProgram_2);
	glDeleteProgram(shaderProgram_3);

	glDeleteBuffers(1, &EBO); // delete EBO
	glDeleteBuffers(3, VBO); // delete the VBOs
	glDeleteVertexArrays(3, VAO); // delete the VAOs

	glfwTerminate();
	return 0;
}