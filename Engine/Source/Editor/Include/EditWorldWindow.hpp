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

        float m_BaseCameraSpeed = 5.f;

        float m_CameraSpeedValue = m_BaseCameraSpeed;

        float pitch = 0.0f;

        float yaw = 90.f;

        float cameraSensitivity = 1.f;

        Tbx::Vector3d m_CameraSpeed = Tbx::Vector3d::Zero();

        float smoothTime = 0.3f;

        void RotateCamera(float _deltatime, bool* _isDirty);

        void CameratMovment(float _deltatime, bool* _isDirty);

        void CameraChangeSpeed(float _deltatime);

        void HideCursor();
    };

END_EDITOR_PCCORE
