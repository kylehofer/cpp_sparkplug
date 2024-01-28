#ifndef SRC_PROPERTIES_SIMPLE_DATETIMEPROPERTY
#define SRC_PROPERTIES_SIMPLE_DATETIMEPROPERTY

#include "SimpleProperty.h"
#include <stdint.h>
#include <tahu.h>

class DateTimeProperty : public SimpleProperty<uint64_t>
{
private:
    /**
     * @brief Construct a new uint64_t Sparkplug Property
     *
     * @param name The name of the Sparkplug Property
     * @param data The first value of the property
     */
    DateTimeProperty(const char *name, uint64_t data) : SimpleProperty(name, data, PROPERTY_DATA_TYPE_DATETIME){};

public:
    /**
     * @brief Construct a new uint64_t Sparkplug Property shared pointer
     *
     * @param name The name of the Sparkplug Property
     * @param data The first value of the property
     * @return std::shared_ptr<DateTimeProperty>
     */
    static std::shared_ptr<DateTimeProperty> create(const char *name, uint64_t data)
    {
        return std::shared_ptr<DateTimeProperty>(new DateTimeProperty(name, data));
    }
};

#endif /* SRC_PROPERTIES_SIMPLE_DATETIMEPROPERTY */
