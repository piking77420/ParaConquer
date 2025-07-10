#pragma once

#include "render_pass.hpp"

BEGIN_PCCORE

class ForwardPass : public RenderPass
{
public:

    ForwardPass();

    ~ForwardPass() override = default;
    
private:
};

END_PCCORE