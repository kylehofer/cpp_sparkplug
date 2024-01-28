#ifndef SRC_PROPERTIES_SIMPLE_SIMPLEPROPERTY
#define SRC_PROPERTIES_SIMPLE_SIMPLEPROPERTY

#include "properties/Property.h"
#include <memory>

/**
 * @brief Simple Property implementation.
 * Does not react to commands.
 *
 */
template <typename T>
class SimpleProperty : public Property
{
public:
    /**
     * @brief Construct a new Sparkplug Property
     *
     */
    SimpleProperty() : Property(){};
    /**
     * @brief Construct a new Sparkplug Property
     *
     * @param name The name of the Sparkplug Property
     * @param data A pointer to a first value of the property
     * @param size The memory size required for the data
     * @param dataType Sparkplug Datatype
     */
    SimpleProperty(const char *name, void *data, size_t size, uint8_t dataType) : Property(name, data, size, dataType){};
    /**
     * @brief Construct a new Sparkplug Property
     *
     * @param name The name of the Sparkplug Property
     * @param data The first value of the property
     * @param dataType Sparkplug Datatype
     */
    SimpleProperty(const char *name, T data, uint8_t dataType) : Property(name, &data, sizeof(T), dataType){};

    /**
     * @brief Sets a new value of the property
     *
     * @param data Pointer to the a piece of data that will be copied to the property
     */
    void setValue(T value)
    {
        Property::setValue(&value);
    };

    T &getValue()
    {
        return *(T *)data;
    };
};

#endif /* SRC_PROPERTIES_SIMPLE_SIMPLEPROPERTY */
