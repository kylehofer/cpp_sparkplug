#ifndef SRC_PROPERTIES_PROPERTY
#define SRC_PROPERTIES_PROPERTY

// PROPERTY_DATA_TYPE_INT8

#include <tahu.h>

class Property
{
private:
    char *name = NULL;
    void *data = NULL;
    uint8_t dataType;
    size_t size;
    bool dirty = false;

protected:
public:
    Property();
    /**
     * @brief Construct a new Sparkplug property
     *
     * @param name The name of the Sparkplug property
     * @param data A pointer to a piece of data that will be as the first value of the property
     * @param size The memory size required for the data
     * @param dataType Sparkplug Datatype
     */
    Property(const char *name, void *data, size_t size, uint8_t dataType);
    virtual ~Property();

    Property &operator=(const Property &right);
    Property &operator=(Property right);

    /**
     * @brief
     *
     * @param property
     * @return int
     */
    int addToPropertySet(org_eclipse_tahu_protobuf_Payload_PropertySet *propertySet, bool isBirth);
    /**
     * @brief Sets a new value of the property
     *
     * @param data Pointer to the a piece of data that will be copied to the property
     */
    void setValue(void *data);
    /**
     * @brief Whether the property value is dirty, and can be published
     *
     * @return true
     * @return false
     */
    bool isDirty();
    /**
     * @brief Used to mark the property that is had been published
     *
     */
    void published();
    /**
     * @brief Returns the pointer to the property data
     *
     * @return void*
     */
    void *getData();
    /**
     * @brief Returns the pointer to the property data
     *
     * @return void*
     */
    const char *getName();
};

#endif /* SRC_PROPERTIES_PROPERTY */
