#ifndef SRC_PROPERTIES_SIMPLE_INT64PROPERTY
#define SRC_PROPERTIES_SIMPLE_INT64PROPERTY

#include "SimpleProperty.h"
#include <stdint.h>
#include <tahu.h>

class Int64Property : public SimpleProperty<int64_t>
{
private:
    /**
     * @brief Construct a new int64_t Sparkplug Property
     *
     * @param name The name of the Sparkplug Property
     * @param data The first value of the property
     */
    Int64Property(const char *name, int64_t data) : SimpleProperty(name, data, PROPERTY_DATA_TYPE_INT64){};

public:
    /**
     * @brief Construct a new int64_t Sparkplug Property shared pointer
     *
     * @param name The name of the Sparkplug Property
     * @param data The first value of the property
     * @return std::shared_ptr<Int64Property>
     */
    static std::shared_ptr<Int64Property> create(const char *name, int64_t data)
    {
        return std::shared_ptr<Int64Property>(new Int64Property(name, data));
    }
};

#endif /* SRC_PROPERTIES_SIMPLE_INT64PROPERTY */
