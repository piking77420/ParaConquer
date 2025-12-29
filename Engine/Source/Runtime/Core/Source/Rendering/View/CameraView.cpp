#include "Rendering/RenderGraph.hpp"
#include "Rendering/View/CameraView.hpp"
#include "Rendering/View/ViewUniformBuffer.hpp"
#include "Rendering/Camera.hpp"
#include "Time/CoreTime.hpp"

namespace PC_CORE::Rendering {

	CameraView::CameraView(PC_CORE::Rhi& _Rhi, Tbx::Vector2i _viewPortSize)
		: View(_Rhi, _viewPortSize)
	{
	}

	CameraView::~CameraView()
	{

	}

    void CameraView::DeclarePass(Rendering::RenderGraph* RenderGraph)
    {

    }

	void CameraView::UpdateView()
	{
        PERF_REGION_SCOPED;
        PERF_REGION_COLOR(PerfRegion::Rendering);

        assert(m_CameraPtr != nullptr);

        if (m_CameraPtr == nullptr)
            return;

        auto StreamIntoMat4 = [](Rendering::mat4& Into, const Tbx::Matrix4x4d& from)
            {
                for (size_t i = 0; i < 16; i++)
                {
                    Into[i] = static_cast<float>(from[i]);
                }
            };

        if (char* ptr = SceneBufferUbo->BeginFullDynamicBufferUpdateForCurrentFrame())
        {
            Rendering::Gpu::ViewUniformBuffer* ViewUniformBuffer = reinterpret_cast<Rendering::Gpu::ViewUniformBuffer*>(ptr);

            StreamIntoMat4(ViewUniformBuffer->View, m_CameraPtr->GetViewMatrix());
            StreamIntoMat4(ViewUniformBuffer->ViewInv, m_CameraPtr->GetViewInvMatrix());
            StreamIntoMat4(ViewUniformBuffer->Projection, m_CameraPtr->GetProjection());
            StreamIntoMat4(ViewUniformBuffer->ProjectionInv, m_CameraPtr->GetProjectionInv());
            StreamIntoMat4(ViewUniformBuffer->ViewProjection, m_CameraPtr->GetViewProjection());
            StreamIntoMat4(ViewUniformBuffer->ViewProjectionInv, m_CameraPtr->GetViewProjectionInv());

            ViewUniformBuffer->CameraNear = m_CameraPtr->GetNear();
            ViewUniformBuffer->CameraFar = m_CameraPtr->GetNear();
            ViewUniformBuffer->Time = static_cast<float>(Time::GetTime());
            ViewUniformBuffer->Deltatime = static_cast<float>(Time::DeltaTime());

            SceneBufferUbo->EndFullDynamicBufferUpdateForCurrentFrame();
        }
	}

	void CameraView::SetCamera(Camera* _camera)
	{
        m_CameraPtr = _camera;
	}

	Camera const* CameraView::GetCamera() const
	{
		return m_CameraPtr;
	}



}

