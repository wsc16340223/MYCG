#include <learnopengl/shader.h>
#include<iostream>
#include<vector>
#include<cmath>
#include<GLFW/glfw3.h>


using namespace std;

struct Point {
	double x = 0.0f, y = 0.0f;
	Point(double x, double y) {
		this->x = x;
		this->y = y;
	}
};

vector<Point> points, bezierCurve, animationCurve;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

GLuint pointVAO = 0, pointVBO = 0;

GLuint bezierVAO = 0, bezierVBO = 0;

GLuint animationVAO = 0, animationVBO = 0;

bool drawCurve = false;

float pointsVertices[900000] = { 0 };
float bezierVertices[900000] = { 0 };
float animationVertices[900000] = { 0 };




int factorial(int n) {
	int sum = 1;
	for (int i = 1; i <= n; i++)
		sum *= i;
	return sum;
}

double Bernstein(int i, int n, double t) {
	double sum = 0;
	sum = factorial(n) / (factorial(i) * factorial(n - i)) * pow(t, i) * pow(1 - t, n - i);
	return sum;
}

float normalizeForX(double x) {
	float answer = (float)(x - (SCR_WIDTH / 2 - 1)) / (SCR_WIDTH / 2 - 1);
	if (answer > 1.0f)
		answer = 1.0f;
	return answer;
}

float normalizeForY(double y) {
	float answer = -(float)(y - (SCR_HEIGHT / 2 - 1)) / (SCR_HEIGHT / 2 - 1);
	if (answer > 1.0f)
		answer = 1.0f;
	return answer;
}

void drawBezierCurve() {
	if (points.size() > 1) {
		float length = 0.001f;
		float count = 1 / length;
		float t = 0.0f;
		bezierCurve.clear();

		for (int m = 0; m < count; m++) {
			t += length;
			double x = 0, y = 0;

			for (auto i = 0; i < points.size(); i++) {
				x += points[i].x * Bernstein(i, points.size() - 1, t);
				y += points[i].y * Bernstein(i, points.size() - 1, t);
				
			}
			bezierCurve.push_back(Point(x, y));
		}
	}
	else
		bezierCurve.clear();
}

void setVertice(vector<Point> *arr, float *floatArr) {
	for (auto i = 0; i < arr->size(); i++) {
		floatArr[i * 3] = normalizeForX((*arr)[i].x);
		floatArr[i * 3 + 1] = normalizeForY((*arr)[i].y);
		floatArr[i * 3 + 2] = 0;
	}
}


void renderPoint() {
	if (pointVAO == 0) {
		glGenVertexArrays(1, &pointVAO);
		glGenBuffers(1, &pointVBO);
		glBindVertexArray(pointVAO);
		glBindBuffer(GL_ARRAY_BUFFER, pointVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(pointsVertices), pointsVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	glBindBuffer(GL_ARRAY_BUFFER, pointVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pointsVertices), pointsVertices, GL_STATIC_DRAW);
	glBindVertexArray(pointVAO);
	if (points.size() > 1) {
		for (size_t i = 0; i < points.size() - 1; i++) {
			glDrawArrays(GL_LINES, i, 2);
		}
	}
	glPointSize(15.0f);
	glDrawArrays(GL_POINTS, 0, points.size());
	glPointSize(1.0f);
}

void renderBezierLine()
{
	if (bezierVAO == 0) {
		glGenVertexArrays(1, &bezierVAO);
		glGenBuffers(1, &bezierVBO);
		glBindVertexArray(bezierVAO);
		glBindBuffer(GL_ARRAY_BUFFER, bezierVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(bezierVertices), bezierVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	glBindBuffer(GL_ARRAY_BUFFER, bezierVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bezierVertices), bezierVertices, GL_STATIC_DRAW);
	glBindVertexArray(bezierVAO);
	glDrawArrays(GL_POINTS, 0, bezierCurve.size());
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	double xpos, ypos;
	if (action == GLFW_PRESS) switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		glfwGetCursorPos(window, &xpos, &ypos); 
		points.push_back(Point(xpos, ypos));
		setVertice(&points, pointsVertices);
		drawBezierCurve();
		setVertice(&bezierCurve, bezierVertices);
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		glfwGetCursorPos(window, &xpos, &ypos);
		points.pop_back();
		setVertice(&points, pointsVertices);
		drawBezierCurve();
		setVertice(&bezierCurve, bezierVertices);
		break;
	default:
		return;
	}
	return;
}