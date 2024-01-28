#ifndef SRC_PROPERTIES_SIMPLE_UINT16PROPERTY
#define SRC_PROPERTIES_SIMPLE_UINT16PROPERTY

#include "SimpleProperty.h"
#include <stdint.h>
#include <tahu.h>

class UInt16Property : public SimpleProperty<uint16_t>
{
private:
    /**
     * @brief Construct a new uint16_t Sparkplug Property
     *
     * @param name The name of the Sparkplug Property
     * @param data The first value of the property
     */
    UInt16Property(const char *name, uint16_t data) : SimpleProperty(name, data, PROPERTY_DATA_TYPE_UINT16){};

public:
    /**
     * @brief Construct a new uint16_t Sparkplug Property shared pointer
     *
     * @param name The name of the Sparkplug Property
     * @param data The first value of the property
     * @return std::shared_ptr<UInt16Property>
     */
    static std::shared_ptr<UInt16Property> create(const char *name, uint16_t data)
    {
        return std::shared_ptr<UInt16Property>(new UInt16Property(name, data));
    }
};

#endif /* SRC_PROPERTIES_SIMPLE_UINT16PROPERTY */
