#pragma once

#include <String>
#include <Vector>

#include "CoreHeader.hpp"
#include "RhiTypedef.h"

BEGIN_PCCORE
    constexpr int32_t NULL_PHYSICAL_DEVICE = -1;

    struct PhysicalDevicesCreateInfo
    {
        std::vector<RhiExtension> requestExtensions;
    };

    struct PhysicalDevice
    {
        std::string name;

        uint32_t driverVersion;

        DEFAULT_COPY_MOVE_OPERATIONS(PhysicalDevice)

        PhysicalDevice() = default;

        virtual ~PhysicalDevice() = default;

        virtual float GetMaxSamplerAnisotropy() const = 0;

        virtual uint32_t GetMaxUsableSampleCount() const = 0;

        virtual uint32_t GetUniformBufferOffsetAlignment() const = 0;

        virtual uint32_t GetStorageBufferOffsetAlignment() const = 0;


        constexpr bool DoesSupportExtension(RhiExtension _queryExtSupport)
        {
            return true; //  TODO
        }
    };

    class PhysicalDevices
    {
    public:
        PC_CORE_API std::vector<PhysicalDevice*> GetPhysicalDevices();

        PC_CORE_API int32_t GetPhysicalDeviceIndex() const;

        PC_CORE_API const PhysicalDevice& GetPhysicalDevice() const;

        PC_CORE_API bool ChangePhysicalDevice(uint32_t _index);

        PC_CORE_API PhysicalDevices(const PhysicalDevicesCreateInfo& _physicalDevicesCreateInfo);

        PC_CORE_API PhysicalDevices() = default;

        PC_CORE_API virtual ~PhysicalDevices() = default;

    protected:
        int32_t m_PhysicalDeviceIndex = NULL_PHYSICAL_DEVICE;

        std::vector<PhysicalDevice*> m_PhysicalDevices;
    };

END_PCCORE
