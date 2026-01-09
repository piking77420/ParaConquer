#pragma once

#include "EditorHeader.hpp"
#include "WorldViewWindow.hpp"
#include "Io/LowPassFilter.hpp"

BEGIN_EDITOR_PCCORE
    class EditWorldWindow : public WorldViewWindow
    {
    public:
        void OnPlayButton() override
        {
        }

        void OnResetScene() override
        {
        }

        explicit EditWorldWindow(Editor& _editor, const std::string& _name);

        ~EditWorldWindow() override = default;

        void Update() override;

    protected:
        void MoveCameraUpdate();

        PC_CORE::LowPassFilter<Tbx::Vector2f, 4> deltass;

        float m_BaseCameraSpeed = 25.0f;

        float m_CameraSpeedValue = m_BaseCameraSpeed;

        float pitch = 0.0f;

        float yaw = 90.f;

        float cameraSensitivity = 1.f;

        Tbx::Vector3d m_DesiredVelocity = Tbx::Vector3d::Zero();

        Tbx::Vector3d m_CameraVelocity = Tbx::Vector3d::Zero();

        static constexpr float AccTime = 10.0f; 

        static constexpr float ScrolWheelForce = 50.f;

        void RotateCamera(float _deltatime);

        void CameratMovment(float _deltatime);

        void CameraChangeSpeed(float _deltatime);

        void HideCursor();

        void ScroolWheelMovement(float _Deltatime);

        void UpdatePosition(float _Deltatime);
    };

END_EDITOR_PCCORE
