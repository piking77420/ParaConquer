#pragma once

#include "core_header.hpp"

#pragma once

#include <array>

#include "core_header.hpp"

BEGIN_PCCORE

template <typename T, size_t Size>
class LowPassFilter
{
public:
    using Type = T;


    PC_CORE_API LowPassFilter() = default;

    PC_CORE_API ~LowPassFilter() = default;

    template <typename U>
    U GetAvarage() const
    {
        static_assert(Size != 0, "Size can't be equal to zero");

        return m_Sum / Size;
    }

    PC_CORE_API void AddSample(T sample)
    {
        m_Sum = m_Sum - m_Data[m_CurrentFrameIndex];
        m_Data.at(m_CurrentFrameIndex) = sample;
        m_Sum = m_Sum + sample;
        m_CurrentFrameIndex += 1;

        if (m_CurrentFrameIndex >= Size)
            m_CurrentFrameIndex = 0;
    }

    PC_CORE_API void Reset()
    {
        for (T& t : m_Data)
        {
            t = {};
        }

        m_CurrentFrameIndex = 0;
        m_Sum = {};
    }


private:
    std::array<Type, Size> m_Data;
    T m_Sum;
    size_t m_CurrentFrameIndex = 0;
};

END_PCCORE

