#pragma once

#include <string>
#include <vector>

#include "core_header.hpp"

#define SWAPCHAIN_EXT "SWAPCHAIN_EXT"
#define MESH_SHADER_EXT "MESH_SHADER"
#define ACCELERATION_EXT "ACCELERATION"
#define RAY_TRACING_EXT "RAYTRAYCING"
#define DEFFERED_HOST_OP "DEFFERED_HOST_OP"


BEGIN_PCCORE
    constexpr int32_t NULL_PHYSICAL_DEVICE = -1;

    struct PhysicalDevicesCreateInfo
    {
        std::vector<std::string> requestExtensions;
    };

    struct PhysicalDevice
    {
        std::string name;
        uint32_t driverVersion;

        DEFAULT_COPY_MOVE_OPERATIONS(PhysicalDevice)

        PhysicalDevice() = default;

        ~PhysicalDevice() = default;
     };

    class PhysicalDevices
    {
    public:
        PC_CORE_API std::vector<PhysicalDevice> GetPhysicalDevices();

        PC_CORE_API int32_t GetPhysicalDeviceIndex() const;

        PC_CORE_API const PhysicalDevice& GetPhysicalDevice() const;

        PC_CORE_API bool ChangePhysicalDevice(uint32_t _index);

        PC_CORE_API PhysicalDevices(const PhysicalDevicesCreateInfo& _physicalDevicesCreateInfo);

        PC_CORE_API PhysicalDevices() = default;

        PC_CORE_API virtual ~PhysicalDevices() = default;

    protected:
        int32_t m_PhysicalDeviceIndex = NULL_PHYSICAL_DEVICE;

        std::vector<PhysicalDevice> m_PhysicalDevices;
    };

END_PCCORE
