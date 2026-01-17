#pragma once

#include "Core/Base.h"

#include "glm/gtx/quaternion.hpp"

namespace PathTracer {

	class Window;

	static constinit glm::vec3 UP_VECTOR = glm::vec3(0.0f, 1.0f, 0.0f);
	static constinit glm::vec3 FORWARD_VECTOR = glm::vec3(0.0f, 0.0f, -1.0f);
	static constinit glm::vec3 RIGHT_VECTOR = glm::vec3(1.0f, 0.0f, 0.0f);

	class Camera {
	public:
		Camera() = default;
		virtual ~Camera() = default;

		virtual void Update(const std::shared_ptr<Window>& window, double deltaTime) = 0;
	protected:
		virtual void CalculateViewMatrix() = 0;
		virtual void CalculateProjMatrix(float width, float height) = 0;
	};

	class PerspectiveCamera : public Camera {
	public:
		PerspectiveCamera(glm::vec3 position, float fov, float nearPlane, float farPlane);
		PerspectiveCamera(float fov, float nearPlane, float farPlane);
		virtual ~PerspectiveCamera() = default;

		inline const glm::vec3& GetPosition() const { return m_Position; }
		inline const glm::quat& GetRotation() const { return m_Rotation; }

		inline float GetFOV() const { return m_FOV; }

		std::tuple<glm::vec3, glm::vec3, glm::vec3> GetCameraVectors() const;
	protected:
		virtual void Update(const std::shared_ptr<Window>& window, double deltaTime);

		void SetPosition(const glm::vec3& position) { m_Position = position; }
		void SetRotation(const glm::quat& rotation) { m_Rotation = rotation; }

		glm::vec3 m_Position{ 0, 0, 0 };
		glm::quat m_Rotation;
	private:
		void CalculateViewMatrix() override;
		void CalculateProjMatrix(float width, float height) override;

		float m_FOV;
		float m_Near;
		float m_Far;

		glm::mat4 m_ViewMatrix{ 1.0f };
		glm::mat4 m_ProjMatrix{ 1.0f };
	};

	class FPSCamera final : public PerspectiveCamera {
	public:
		FPSCamera(glm::vec3 position, float fov, float nearPlane, float farPlane);
		virtual ~FPSCamera() = default;

		void Update(const std::shared_ptr<Window>& window, double deltaTime) override;
		void End();

		inline bool IsMouseMoved() const { return m_MouseMoved; }
		inline bool IsCameraMoved() const { return m_CameraMoved; }

		inline void UpdateOverride(bool updateOverride) { m_UpdateOverride = updateOverride; }
	private:
		void ProcessMouseMovement(double dx, double dy, bool constrainPitch = true);
		void ProcessKeyboardInput(glm::vec3 direction, double deltaTime);

		float m_Yaw = -90.0f;
		float m_Pitch = 0.0f;

		double m_LastX = 0.0;
		double m_LastY = 0.0;

		float m_Sensitivity = 0.01f;
		float m_Speed = 0.5f;

		bool m_MouseMoved = false;
		bool m_CameraMoved = false;

		bool m_UpdateOverride = false;
	};
}