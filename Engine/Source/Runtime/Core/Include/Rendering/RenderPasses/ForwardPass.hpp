#pragma once

#include "RenderPass.hpp"

BEGIN_PCCORE
    class ForwardPass : public RenderPass
    {
    public:
        ForwardPass();

        ~ForwardPass() override = default;
    };

END_PCCORE
