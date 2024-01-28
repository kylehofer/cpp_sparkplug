#ifndef SRC_PROPERTIES_SIMPLE_DOUBLEPROPERTY
#define SRC_PROPERTIES_SIMPLE_DOUBLEPROPERTY

#include "SimpleProperty.h"
#include <stdint.h>
#include <tahu.h>

class DoubleProperty : public SimpleProperty<double>
{
private:
    /**
     * @brief Construct a new double Sparkplug Property
     *
     * @param name The name of the Sparkplug Property
     * @param data The first value of the property
     */
    DoubleProperty(const char *name, double data) : SimpleProperty(name, data, PROPERTY_DATA_TYPE_DOUBLE){};

public:
    /**
     * @brief Construct a new double Sparkplug Property shared pointer
     *
     * @param name The name of the Sparkplug Property
     * @param data The first value of the property
     * @return std::shared_ptr<DoubleProperty>
     */
    static std::shared_ptr<DoubleProperty> create(const char *name, double data)
    {
        return std::shared_ptr<DoubleProperty>(new DoubleProperty(name, data));
    }
};

#endif /* SRC_PROPERTIES_SIMPLE_DOUBLEPROPERTY */
