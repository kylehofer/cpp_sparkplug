#ifndef SRC_PROPERTIES_SIMPLE_UINT32PROPERTY
#define SRC_PROPERTIES_SIMPLE_UINT32PROPERTY

#include "SimpleProperty.h"
#include <stdint.h>
#include <tahu.h>

class UInt32Property : public SimpleProperty<uint32_t>
{
private:
    /**
     * @brief Construct a new uint32_t Sparkplug Property
     *
     * @param name The name of the Sparkplug Property
     * @param data The first value of the property
     */
    UInt32Property(const char *name, uint32_t data) : SimpleProperty(name, data, PROPERTY_DATA_TYPE_UINT32){};

public:
    /**
     * @brief Construct a new uint32_t Sparkplug Property shared pointer
     *
     * @param name The name of the Sparkplug Property
     * @param data The first value of the property
     * @return std::shared_ptr<UInt32Property>
     */
    static std::shared_ptr<UInt32Property> create(const char *name, uint32_t data)
    {
        return std::shared_ptr<UInt32Property>(new UInt32Property(name, data));
    }
};

#endif /* SRC_PROPERTIES_SIMPLE_UINT32PROPERTY */
