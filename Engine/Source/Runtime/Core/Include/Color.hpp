#ifndef PC_CORE_COLOR
#define PC_CORE_COLOR

#include <cstdint>
#include <array>
#include <variant>

namespace PC_CORE
{
	struct FloatRGBA { std::array<float, 4> value; };
	struct ByteRGBA { std::array<uint8_t, 4> value; };
	struct PackedRGBA { uint32_t value; };
	class Color
	{
	public:
		Color() = default;
		explicit Color(const FloatRGBA& Color);
		explicit Color(const ByteRGBA& ByteRGBA);
		explicit Color(PackedRGBA PackedRGBA);
		~Color() = default;

		PackedRGBA ToPackedRGBA() const;
		
	private:
		std::variant<FloatRGBA, ByteRGBA, PackedRGBA> m_Data;
	};
} // namespace PC_CORE

#endif //  PC_CORE_COLOR