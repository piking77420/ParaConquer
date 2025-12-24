#pragma once

#include "RhiObject.hpp"

BEGIN_PCCORE
    class RhiSampler : public RhiObjectT<RhiSampler>
    {
    public:

        DEFAULT_COPY_MOVE_OPERATIONS(RhiSampler)

        explicit RhiSampler(Rhi& _Rhi);

        ~RhiSampler() override = default;

        // Setter

        RhiSampler& SetMagFilter(Filter _Filter)
        {
            m_MagFilter = _Filter;
            return *this;
        }

        RhiSampler& SetMinFilter(Filter _Filter)
        {
            m_MinFilter = _Filter;
            return *this;
        }

        RhiSampler& SetMipmapMode(SamplerMipmapMode _SamplerMipmapMode)
        {
            m_MipmapMode = _SamplerMipmapMode;
            return *this;
        }

        RhiSampler& SetU(SamplerAddressMode _SamplerAddressMode)
        {
            m_U = _SamplerAddressMode;
            return *this;
        }

        RhiSampler& SetV(SamplerAddressMode _SamplerAddressMode)
        {
            m_V = _SamplerAddressMode;
            return *this;
        }

        RhiSampler& SetW(SamplerAddressMode _SamplerAddressMode)
        {
            m_W = _SamplerAddressMode;
            return *this;
        }

        RhiSampler& SetCompareEnable(bool _CompareEnable)
        {
            m_CompareEnable = _CompareEnable;
            return *this;
        }

        RhiSampler& SetCompareOp(CompareOp _CompareOp)
        {
            m_CompareOp = _CompareOp;
            return *this;
        }

        // Getter
        Filter GetMagFilter() const
        {
            return m_MagFilter;
        }

        Filter GetMinFilter() const
        {
            return m_MinFilter;
        }

        SamplerMipmapMode GetMipmapMode() const
        {
            return m_MipmapMode;
        }

        SamplerAddressMode GetU() const
        {
            return m_U;
        }

        SamplerAddressMode GetV() const
        {
            return m_V;
        }

        SamplerAddressMode GetW() const
        {
            return m_W;
        }

        bool GetCompareEnable() const
        {
            return m_CompareEnable;
        }

        CompareOp GetCompareOp() const
        {
            return m_CompareOp;
        }

    protected:
        Filter m_MagFilter{ Filter::Nearest };
        Filter m_MinFilter{ Filter::Nearest };
        SamplerMipmapMode m_MipmapMode{ SamplerMipmapMode::Nearest };

        SamplerAddressMode m_U{ SamplerAddressMode::Repeat };
        SamplerAddressMode m_V{ SamplerAddressMode::Repeat };
        SamplerAddressMode m_W{ SamplerAddressMode::Repeat };

        bool m_CompareEnable = false;
        CompareOp m_CompareOp{ CompareOp::Always };
    };

END_PCCORE
