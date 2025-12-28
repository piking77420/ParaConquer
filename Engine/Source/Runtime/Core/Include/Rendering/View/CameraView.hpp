#pragma once

#include "View.hpp"

namespace PC_CORE
{
	class Camera;
}

namespace PC_CORE::Rendering
{
	class PC_CORE_API CameraView : public View
	{
	public:
		explicit CameraView(PC_CORE::Rhi& _Rhi, Tbx::Vector2i _viewPortSize);

		~CameraView() override;

		void UpdateView() override;

		void DeclarePass(Rendering::RenderGraph* RenderGraph) override;

		void SetCamera(Camera* _camera);

		const Camera const* GetCamera() const;

	private:
		Camera* m_CameraPtr{ nullptr };


	};

}