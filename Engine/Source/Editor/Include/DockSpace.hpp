#pragma once

#include "EditorHeader.hpp"

BEGIN_EDITOR_PCCORE
    class DockSpace
    {
    public:
        DockSpace() = default;

        ~DockSpace() = default;

        void BeginDockSpace();

        void EndDockSpace();
    };

END_EDITOR_PCCORE
