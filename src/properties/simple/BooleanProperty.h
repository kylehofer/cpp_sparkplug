#ifndef SRC_PROPERTIES_SIMPLE_BOOLEANPROPERTY
#define SRC_PROPERTIES_SIMPLE_BOOLEANPROPERTY

#include "SimpleProperty.h"
#include <stdint.h>
#include <tahu.h>

class BooleanProperty : public SimpleProperty<bool>
{
private:
    /**
     * @brief Construct a new bool Sparkplug Property
     *
     * @param name The name of the Sparkplug Property
     * @param data The first value of the property
     */
    BooleanProperty(const char *name, bool data) : SimpleProperty(name, data, PROPERTY_DATA_TYPE_BOOLEAN){};

public:
    /**
     * @brief Construct a new bool Sparkplug Property shared pointer
     *
     * @param name The name of the Sparkplug Property
     * @param data The first value of the property
     * @return std::shared_ptr<BooleanProperty>
     */
    static std::shared_ptr<BooleanProperty> create(const char *name, bool data)
    {
        return std::shared_ptr<BooleanProperty>(new BooleanProperty(name, data));
    }
};

#endif /* SRC_PROPERTIES_SIMPLE_BOOLEANPROPERTY */
