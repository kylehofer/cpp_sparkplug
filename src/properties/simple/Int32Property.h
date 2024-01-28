#ifndef SRC_PROPERTIES_SIMPLE_INT32PROPERTY
#define SRC_PROPERTIES_SIMPLE_INT32PROPERTY

#include "SimpleProperty.h"
#include <stdint.h>
#include <tahu.h>

class Int32Property : public SimpleProperty<int32_t>
{
private:
    /**
     * @brief Construct a new int32_t Sparkplug Property
     *
     * @param name The name of the Sparkplug Property
     * @param data The first value of the property
     */
    Int32Property(const char *name, int32_t data) : SimpleProperty(name, data, PROPERTY_DATA_TYPE_INT32){};

public:
    /**
     * @brief Construct a new int32_t Sparkplug Property shared pointer
     *
     * @param name The name of the Sparkplug Property
     * @param data The first value of the property
     * @return std::shared_ptr<Int32Property>
     */
    static std::shared_ptr<Int32Property> create(const char *name, int32_t data)
    {
        return std::shared_ptr<Int32Property>(new Int32Property(name, data));
    }
};

#endif /* SRC_PROPERTIES_SIMPLE_INT32PROPERTY */
