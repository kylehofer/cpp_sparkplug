#ifndef SRC_PROPERTIES_SIMPLE_UINT64PROPERTY
#define SRC_PROPERTIES_SIMPLE_UINT64PROPERTY

#include "SimpleProperty.h"
#include <stdint.h>
#include <tahu.h>

class UInt64Property : public SimpleProperty<uint64_t>
{
private:
    /**
     * @brief Construct a new uint64_t Sparkplug Property
     *
     * @param name The name of the Sparkplug Property
     * @param data The first value of the property
     */
    UInt64Property(const char *name, uint64_t data) : SimpleProperty(name, data, PROPERTY_DATA_TYPE_UINT64){};

public:
    /**
     * @brief Construct a new uint64_t Sparkplug Property shared pointer
     *
     * @param name The name of the Sparkplug Property
     * @param data The first value of the property
     * @return std::shared_ptr<UInt64Property>
     */
    static std::shared_ptr<UInt64Property> create(const char *name, uint64_t data)
    {
        return std::shared_ptr<UInt64Property>(new UInt64Property(name, data));
    }
};

#endif /* SRC_PROPERTIES_SIMPLE_UINT64PROPERTY */
