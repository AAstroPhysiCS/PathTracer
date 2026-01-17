#include "Camera.h"

#include "Core/Window.h"

#include "Core/Event.h"

namespace PathTracer {
	
	PerspectiveCamera::PerspectiveCamera(glm::vec3 position, float fov, float nearPlane, float farPlane)
		: m_Position(position), m_FOV(fov), m_Near(nearPlane), m_Far(farPlane) {
	}

	PerspectiveCamera::PerspectiveCamera(float fov, float nearPlane, float farPlane)
		: m_FOV(fov), m_Near(nearPlane), m_Far(farPlane) {
	}

	void PerspectiveCamera::Update(const std::shared_ptr<Window>& window, double deltaTime) {
		auto [width, height] = window->GetFramebufferSize();

		CalculateViewMatrix();
		CalculateProjMatrix(static_cast<float>(width), static_cast<float>(height));
	}

	std::tuple<glm::vec3, glm::vec3, glm::vec3> PerspectiveCamera::GetCameraVectors() const {
		glm::vec3 right = glm::vec3(m_ViewMatrix[0][0], m_ViewMatrix[1][0], m_ViewMatrix[2][0]);
		glm::vec3 up = glm::vec3(m_ViewMatrix[0][1], m_ViewMatrix[1][1], m_ViewMatrix[2][1]);
		glm::vec3 direction = -glm::vec3(m_ViewMatrix[0][2], m_ViewMatrix[1][2], m_ViewMatrix[2][2]);

		right = glm::normalize(right);
		up = glm::normalize(up);
		direction = glm::normalize(direction);

		return std::make_tuple(right, up, direction);
	}

	void PerspectiveCamera::CalculateViewMatrix() {
		//Rotate the world opposite to the camera’s orientation, therefore conjugate
		glm::mat4 rotationMatrix = glm::toMat4(glm::conjugate(m_Rotation));
		glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), -m_Position);
		m_ViewMatrix = rotationMatrix * translationMatrix;
	}

	void PerspectiveCamera::CalculateProjMatrix(float width, float height) {
		if (height == 0)
			return;
		m_ProjMatrix = glm::perspective(glm::radians(m_FOV), width / height, m_Near, m_Far);
	}

	FPSCamera::FPSCamera(glm::vec3 position, float fov, float nearPlane, float farPlane)
		: PerspectiveCamera(position, fov, nearPlane, farPlane) {

		EventSystem::AddEventListener<KeyEvent>([&](const KeyEvent& evt) {
			if (evt.GetAction() == GLFW_PRESS)
				return;

			switch (evt.GetKey()) {
				case GLFW_KEY_W: {
					ProcessKeyboardInput(glm::vec3(0.0f, 0.0f, 1.0f), evt.GetDeltaTime());
					break;
				}
				case GLFW_KEY_S: {
					ProcessKeyboardInput(glm::vec3(0.0f, 0.0f, -1.0f), evt.GetDeltaTime());
					break;
				}
				case GLFW_KEY_A: {
					ProcessKeyboardInput(glm::vec3(-1.0f, 0.0f, 0.0f), evt.GetDeltaTime());
					break;
				}
				case GLFW_KEY_D: {
					ProcessKeyboardInput(glm::vec3(1.0f, 0.0f, 0.0f), evt.GetDeltaTime());
					break;
				}
				case GLFW_KEY_SPACE: {
					ProcessKeyboardInput(glm::vec3(0.0f, 1.0f, 0.0f), evt.GetDeltaTime());
					break;
				}
				case GLFW_KEY_RIGHT_CONTROL: {
					ProcessKeyboardInput(glm::vec3(0.0f, -1.0f, 0.0f), evt.GetDeltaTime());
					break;
				}
				case GLFW_KEY_LEFT_SHIFT: {
					m_Speed = 50.0f;
					break;
				}
			}
		});

		EventSystem::AddEventListener<MouseCursorEvent>([&](const MouseCursorEvent& evt) {
			auto [xPos, yPos] = evt.GetPos();

			static bool firstMouseInsertion = true;

			if (firstMouseInsertion) {
				m_LastX = xPos;
				m_LastY = yPos;
				firstMouseInsertion = false;
			}

			double xOffset = xPos - m_LastX;
			double yOffset = m_LastY - yPos;

			ProcessMouseMovement(xOffset, yOffset, true);

			m_LastX = xPos;
			m_LastY = yPos;
		});
	}

	void FPSCamera::Update(const std::shared_ptr<Window>& window, double deltaTime) {
		if (m_UpdateOverride)
			return;
		PerspectiveCamera::Update(window, deltaTime);
	}

	void FPSCamera::End() {
		m_CameraMoved = false;
		m_MouseMoved = false;
	}

	void FPSCamera::ProcessMouseMovement(double dx, double dy, bool constrainPitch) {
		if (m_UpdateOverride)
			return;
		m_MouseMoved = true;

		float yaw = dx * m_Sensitivity;
		float pitch = dy * m_Sensitivity;

		m_Yaw += static_cast<float>(dx);
		m_Pitch += static_cast<float>(dy);

		m_Pitch = glm::clamp(m_Pitch, -89.0f, 89.0f);

		glm::quat qYaw = glm::angleAxis(glm::radians(m_Yaw), UP_VECTOR);
		glm::quat qPitch = glm::angleAxis(glm::radians(m_Pitch), RIGHT_VECTOR);

		m_Rotation = qYaw * qPitch;
	}

	void FPSCamera::ProcessKeyboardInput(glm::vec3 direction, double deltaTime) {
		if (m_UpdateOverride)
			return;
		m_CameraMoved = true;

		float velocity = m_Speed * static_cast<float>(deltaTime);
		
		glm::vec3 front = m_Rotation * FORWARD_VECTOR;
		glm::vec3 right = m_Rotation * RIGHT_VECTOR;
		glm::vec3 up = UP_VECTOR;

		if (direction.z != 0.0f)
			m_Position += front * direction.z * velocity;
		if (direction.x != 0.0f)
			m_Position += right * direction.x * velocity;
		if (direction.y != 0.0f)
			m_Position += up * direction.y * velocity;
	}
}
