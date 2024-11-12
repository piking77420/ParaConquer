
#include <glslang/Include/glslang_c_interface.h>
#include <exception>

int main()
{
	int v = 1;

	try
	{
 		glslang_program_SPIRV_generate(reinterpret_cast<glslang_program_t*>(&v), {});

	}
	catch (const std::exception& e )
	{	

	}
}