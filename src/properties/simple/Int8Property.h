#ifndef SRC_PROPERTIES_SIMPLE_INT8PROPERTY
#define SRC_PROPERTIES_SIMPLE_INT8PROPERTY

#include "SimpleProperty.h"
#include <stdint.h>
#include <tahu.h>

class Int8Property : public SimpleProperty<int8_t>
{
private:
    /**
     * @brief Construct a new int8_t Sparkplug Property
     *
     * @param name The name of the Sparkplug Property
     * @param data The first value of the property
     */
    Int8Property(const char *name, int8_t data) : SimpleProperty(name, data, PROPERTY_DATA_TYPE_INT8){};

public:
    /**
     * @brief Construct a new int8_t Sparkplug Property shared pointer
     *
     * @param name The name of the Sparkplug Property
     * @param data The first value of the property
     * @return std::shared_ptr<Int8Property>
     */
    static std::shared_ptr<Int8Property> create(const char *name, int8_t data)
    {
        return std::shared_ptr<Int8Property>(new Int8Property(name, data));
    }
};

#endif /* SRC_PROPERTIES_SIMPLE_INT8PROPERTY */
