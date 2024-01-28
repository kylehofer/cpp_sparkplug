#ifndef SRC_PROPERTIES_SIMPLE_STRINGPROPERTY
#define SRC_PROPERTIES_SIMPLE_STRINGPROPERTY

#include "SimpleProperty.h"
#include <stdint.h>
#include <tahu.h>

class StringProperty : public SimpleProperty<string>
{
private:
    /**
     * @brief Construct a new string Sparkplug Property
     *
     * @param name The name of the Sparkplug Property
     * @param data The first value of the property
     */
    StringProperty(const char *name, string data) : SimpleProperty(name, data, PROPERTY_DATA_TYPE_STRING){};

public:
    /**
     * @brief Construct a new string Sparkplug Property shared pointer
     *
     * @param name The name of the Sparkplug Property
     * @param data The first value of the property
     * @return std::shared_ptr<StringProperty>
     */
    static std::shared_ptr<StringProperty> create(const char *name, string data)
    {
        return std::shared_ptr<StringProperty>(new StringProperty(name, data));
    }
};

#endif /* SRC_PROPERTIES_SIMPLE_STRINGPROPERTY */
