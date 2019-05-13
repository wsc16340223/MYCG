#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// 摄像机的移动方向
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
	UP,
	DOWN
};

// 摄像机的默认变量设定
// 欧拉角的偏航角 左右
const float YAW         = -90.0f;
// 欧拉角的俯仰角 上下
const float PITCH       =  0.0f;
// 移动速度
const float SPEED       =  2.5f;
// 鼠标灵敏度
const float SENSITIVITY =  0.1f;
// 缩放
const float ZOOM        =  45.0f;


// 摄像机类
class Camera
{
private:
	// 摄像机的各种属性
	// 摄像机的位置
	glm::vec3 Position;
	// 摄像机的方向
	glm::vec3 Front;
	// 摄像机的上轴
	glm::vec3 Up;
	// 摄像机的右轴
	glm::vec3 Right;
	// 世界坐标的上轴
	glm::vec3 WorldUp;
	// 欧拉角
	//偏航角
	float Yaw;
	// 俯仰角
	float Pitch;
	// 移动速度
	float MovementSpeed;
	// 鼠标灵敏度
	float MouseSensitivity;
	// 缩放
	float Zoom;
	// 向量更新
	void updateCameraVectors()
	{

		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);

		Right = glm::normalize(glm::cross(Front, WorldUp));
		Up = glm::normalize(glm::cross(Right, Front));
	}
public:
	void setPosition(glm::vec3 pos) {
		Position = pos;
	}
	void setPosition(float x, float y, float z) {
		Position.x = x;
		Position.y = y;
		Position.z = z;
	}
	glm::vec3 getPosition() { return Position; }

	void setFront(glm::vec3 front) {
		Front = front;
	}
	void setFront(float x, float y, float z) {
		Front.x = x;
		Front.y = y;
		Front.z = z;
	}
	glm::vec3 getFront() { return Front; }

	void setUp(glm::vec3 up) {
		Up = up;
	}
	void setUp(float x, float y, float z) {
		Up.x = x;
		Up.y = y;
		Up.z = z;
	}
	glm::vec3 getUp() { return Up; }

	void setRight(glm::vec3 right) {
		Right = right;
	}
	void setRight(float x, float y, float z) {
		Right.x = x;
		Right.y = y;
		Right.z = z;
	}
	glm::vec3 getRight() { return Right; }

	void setWorldUp(glm::vec3 worldup) {
		WorldUp = worldup;
	}
	void setWorldUp(float x, float y, float z) {
		WorldUp.x = x;
		WorldUp.y = y;
		WorldUp.z = z;
	}
	glm::vec3 getWorldUp() { return WorldUp; }

	void setYaw(float yaw) {
		Yaw = yaw;
	}
	float getYaw() { return Yaw; }

	void setPitch(float pitch) {
		Pitch = pitch;
	}
	float getPitch() { return Pitch; }

	void setMovementSpeed(float speed) {
		MovementSpeed = speed;
	}
	float getMovementSpeed() { return MovementSpeed; }

	void setMouseSensitivity(float sensitivity) {
		MouseSensitivity = sensitivity;
	}
	float getMouseSensitivity() { return MouseSensitivity; }

	void setZoom(float zoom) {
		Zoom = zoom;
	}
	float getZoom() { return Zoom; }

    // 以向量为参数的构造函数
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    // 以标量为参数的构造函数
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // 获取view矩阵
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // 键盘输入之后改变摄像机的位置
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
		if (direction == UP)
			Position += Up * velocity;
		if (direction == DOWN)
			Position -= Up * velocity;
    }

    // 鼠标移动，改变摄像机的欧拉角
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        // 角度到90会逆转视角，因此设置在89
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        updateCameraVectors();
    }

    // 鼠标缩放，改变视野
    void ProcessMouseScroll(float yoffset)
    {
        if (Zoom >= 1.0f && Zoom <= 45.0f)
            Zoom -= yoffset;
        if (Zoom <= 1.0f)
            Zoom = 1.0f;
        if (Zoom >= 45.0f)
            Zoom = 45.0f;
    }


};
#endif