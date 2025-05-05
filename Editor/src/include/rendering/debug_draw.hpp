#pragma once

#include "editor_header.hpp"
#include "math/toolbox_typedef.hpp"

BEGIN_EDITOR_PCCORE

class DebugDraw
{
public:
	static void DrawLine(Tbx::Vector3f p1, Tbx::Vector3f p2, float thickNess = 1.f);


	DebugDraw() = default;

	~DebugDraw() = default;

private:

	

};


END_EDITOR_PCCORE