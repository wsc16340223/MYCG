#include<learnopengl/shader.h>
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

//计算阶乘
int factorial(int n);
//伯恩斯坦基函数
double Bernstein(int i, int n, double t);
//对x进行标准化
float normalizeForX(double x);
//对y进行标准化
float normalizeForY(double y);
//计算bezier曲线
void drawBezierCurve();
//将vector坐标存入数组中
void setVertice(vector<Point> *arr, float *floatArr);
//鼠标点击时间，左键画点，右键消点
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
//获取曲线上对应的点
Point getPointFromCurve(Point A, Point B, double step);
//获取动态的点
void getAnimationPoints(double step, int time, bool isFirst, int alreadyPush);
//动态连接曲线
void showAnimationCurve(int time, int alreadyShow);
//绘制点击的位置所在点
void renderPoint();
//绘制bezier曲线
void renderBezierCurve();
//动态绘制bezier曲线
void renderAnimation();


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

Point getPointFromCurve(Point A, Point B, double step) {
	double x = A.x + (B.x - A.x) * step;
	double y = A.y + (B.y - A.y) * step;
	Point temp(x, y);
	return temp;
}

void getAnimationPoints(double step, int time, bool isFirst, int alreadyPush) {
	if (time == 0) {
		return;
	}
	if (isFirst) {
		for (int i = 0; i < time; i++) {
			animationCurve.push_back(getPointFromCurve(points[i], points[i + 1], step));
		}
		getAnimationPoints(step, time - 1, false, 0);
	}
	else {
		for (int i = alreadyPush; i < time + alreadyPush; i++) {
			animationCurve.push_back(getPointFromCurve(animationCurve[i], animationCurve[i + 1], step));
		}
		getAnimationPoints(step, time - 1, false, alreadyPush + time + 1);
	}
}

void showAnimationCurve(int time, int alreadyShow) {
	if (time == 0) {
		return;
	}
	for (size_t i = alreadyShow; i < time + alreadyShow; i++) {
		glDrawArrays(GL_LINES, i, 2);
	}
	showAnimationCurve(time - 1, alreadyShow + time + 1);
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

void renderBezierCurve() {
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

void renderAnimation() {
	if (animationVAO == 0) {
		glGenVertexArrays(1, &animationVAO);
		glGenBuffers(1, &animationVBO);
		glBindVertexArray(animationVAO);
		glBindBuffer(GL_ARRAY_BUFFER, animationVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(animationVertices), animationVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	glBindBuffer(GL_ARRAY_BUFFER, animationVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(animationVertices), animationVertices, GL_STATIC_DRAW);
	glBindVertexArray(animationVAO);
	showAnimationCurve(points.size() - 2, 0);
	glPointSize(10.0f);
	glDrawArrays(GL_POINTS, 0, animationCurve.size());
	glPointSize(1.0f);
}