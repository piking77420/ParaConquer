#include "Rendering/Renderer.hpp"

#include <Thread>
#include <PerfRegion.hpp>

#include "LowRenderer/Rhi.hpp"
#include "Rendering/RenderView.hpp"
#include "Rendering/RenderPasses/RenderPass.hpp"

class PC_CORE_API FowardPass : public PC_CORE::Rendering::RenderPass
{
public:

    FowardPass();

    ~FowardPass() override = default;


    IMP_DYNAMIC_REFLECT();
    
    const char* GetName() const
    {
        return "FowardPass";
    }

    std::array<float, 4> GetColor() const
    {
        return
        {
            0.f,
            0.f,
            1.f,
            1.f
        };
    }
	
    void Build(const PC_CORE::Rendering::RenderView&)
    {

    }

    void Execute(PC_CORE::CommandList*, const PC_CORE::Rendering::RenderView&, const PC_CORE::Rendering::RenderingWorldData&) const
    {

    }

private:

};

FowardPass::FowardPass()
{
    DYNAMIC_REFLECT_INIT;
}



namespace PC_CORE::Rendering
{
    Renderer::Renderer(Rhi& _Rhi)
        : m_Rhi(_Rhi)
    {
        static FowardPass fowardPass;

        m_RenderGraph.AddRenderPass<FowardPass>(&fowardPass);
    }

   void Renderer::Update(const RenderView& _view)
   {

   }

   void Renderer::Excute(const RenderView& _view)
   {

   }

}

