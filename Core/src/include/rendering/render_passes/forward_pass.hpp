#pragma once

#include "render_pass.hpp"

BEGIN_PCCORE

class ForwardPass : public RenderPass
{
public:

    ForwardPass();

    ~ForwardPass() override = default;

    void Build() override;
    
    void Execute() override; 

private:
};

END_PCCORE