#include "low_renderer/physical_devices.hpp"

#include "log.hpp"

std::vector<PC_CORE::PhysicalDevice> PC_CORE::PhysicalDevices::GetPhysicalDevices()
{
    return m_PhysicalDevices;
}

int32_t PC_CORE::PhysicalDevices::GetPhysicalDeviceIndex() const
{
    return m_PhysicalDeviceIndex;
}

const PC_CORE::PhysicalDevice& PC_CORE::PhysicalDevices::GetPhysicalDevice() const
{
    return m_PhysicalDevices[m_PhysicalDeviceIndex];
}

bool PC_CORE::PhysicalDevices::ChangePhysicalDevice(uint32_t _index)
{
    return false;
}

PC_CORE::PhysicalDevices::PhysicalDevices(const PhysicalDevicesCreateInfo& _physicalDevicesCreateInfo)
{
    PC_LOG("Initialize physical devices");
}
