#ifndef SRC_PROPERTIES_SIMPLE_FLOATPROPERTY
#define SRC_PROPERTIES_SIMPLE_FLOATPROPERTY

#include "SimpleProperty.h"
#include <stdint.h>
#include <tahu.h>

class FloatProperty : public SimpleProperty<float>
{
private:
    /**
     * @brief Construct a new float Sparkplug Property
     *
     * @param name The name of the Sparkplug Property
     * @param data The first value of the property
     */
    FloatProperty(const char *name, float data) : SimpleProperty(name, data, PROPERTY_DATA_TYPE_FLOAT){};

public:
    /**
     * @brief Construct a new float Sparkplug Property shared pointer
     *
     * @param name The name of the Sparkplug Property
     * @param data The first value of the property
     * @return std::shared_ptr<FloatProperty>
     */
    static std::shared_ptr<FloatProperty> create(const char *name, float data)
    {
        return std::shared_ptr<FloatProperty>(new FloatProperty(name, data));
    }
};

#endif /* SRC_PROPERTIES_SIMPLE_FLOATPROPERTY */
