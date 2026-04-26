#include <Color.hpp>

// helper type for the visitor #4
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

namespace PC_CORE
{
	Color::Color(const FloatRGBA& Color)
		: m_Data(Color)
	{

	}

	Color::Color(const ByteRGBA& ByteRGBA)
		: m_Data(ByteRGBA)
	{

	}

	Color::Color(PackedRGBA PackedRGBA)
		: m_Data(PackedRGBA)
	{

	}

	PackedRGBA Color::ToPackedRGBA() const
	{
		 return std::visit(overloaded{
			[](const FloatRGBA& _C) -> PackedRGBA
			{
				const uint32_t r = static_cast<uint32_t>(_C.value[0] * 255.0f);
				const uint32_t g = static_cast<uint32_t>(_C.value[1] * 255.0f);
				const uint32_t b = static_cast<uint32_t>(_C.value[2] * 255.0f);
				const uint32_t a = static_cast<uint32_t>(_C.value[3] * 255.0f);

				return PackedRGBA{
					(r << 24) | (g << 16) | (b << 8) | a
				};
			},
			[](const ByteRGBA& _C) -> PackedRGBA
			{
				
				return {};
			},
			[](const PackedRGBA& _C) -> PackedRGBA
			{
				 return _C;
			}
		 }, m_Data);
	}


} // PC_CORE

