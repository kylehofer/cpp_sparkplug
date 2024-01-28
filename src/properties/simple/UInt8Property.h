#ifndef SRC_PROPERTIES_SIMPLE_UINT8PROPERTY
#define SRC_PROPERTIES_SIMPLE_UINT8PROPERTY

#include "SimpleProperty.h"
#include <stdint.h>
#include <tahu.h>

class UInt8Property : public SimpleProperty<uint8_t>
{
private:
    /**
     * @brief Construct a new uint8_t Sparkplug Property
     *
     * @param name The name of the Sparkplug Property
     * @param data The first value of the property
     */
    UInt8Property(const char *name, uint8_t data) : SimpleProperty(name, data, PROPERTY_DATA_TYPE_UINT8){};

public:
    /**
     * @brief Construct a new uint8_t Sparkplug Property shared pointer
     *
     * @param name The name of the Sparkplug Property
     * @param data The first value of the property
     * @return std::shared_ptr<UInt8Property>
     */
    static std::shared_ptr<UInt8Property> create(const char *name, uint8_t data)
    {
        return std::shared_ptr<UInt8Property>(new UInt8Property(name, data));
    }
};

#endif /* SRC_PROPERTIES_SIMPLE_UINT8PROPERTY */
