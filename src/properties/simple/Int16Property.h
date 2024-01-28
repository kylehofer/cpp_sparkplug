#ifndef SRC_PROPERTIES_SIMPLE_INT16PROPERTY
#define SRC_PROPERTIES_SIMPLE_INT16PROPERTY

#include "SimpleProperty.h"
#include <stdint.h>
#include <tahu.h>

class Int16Property : public SimpleProperty<int16_t>
{
private:
    /**
     * @brief Construct a new int16_t Sparkplug Property
     *
     * @param name The name of the Sparkplug Property
     * @param data The first value of the property
     */
    Int16Property(const char *name, int16_t data) : SimpleProperty(name, data, PROPERTY_DATA_TYPE_INT16){};

public:
    /**
     * @brief Construct a new int16_t Sparkplug Property shared pointer
     *
     * @param name The name of the Sparkplug Property
     * @param data The first value of the property
     * @return std::shared_ptr<Int16Property>
     */
    static std::shared_ptr<Int16Property> create(const char *name, int16_t data)
    {
        return std::shared_ptr<Int16Property>(new Int16Property(name, data));
    }
};

#endif /* SRC_PROPERTIES_SIMPLE_INT16PROPERTY */
