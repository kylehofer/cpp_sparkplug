#ifndef SRC_PROPERTIES_COMPLEX_PROPERTYSET
#define SRC_PROPERTIES_COMPLEX_PROPERTYSET

#include "properties/Property.h"
#include <vector>
#include <memory>

/**
 * @brief Represents a Property Set as a Property
 */
class PropertySet : public Property
{
private:
    std::vector<std::shared_ptr<Property>> properties;
    PropertySet(const char *name) : Property(name, nullptr, 0, PROPERTY_DATA_TYPE_PROPERTYSET){};

protected:
public:
    /**
     * @brief Construct a new Property Set Sparkplug Property shared pointer
     *
     * @param name The name of the Sparkplug Property Set
     * @return std::shared_ptr<PropertySet>
     */
    static std::shared_ptr<PropertySet> create(const char *name);
    static std::shared_ptr<PropertySet> create(const char *name, const std::shared_ptr<Property> &property);
    static std::shared_ptr<PropertySet> create(const char *name, const std::vector<std::shared_ptr<Property>> &properties);

    /**
     * @brief Adds the property set to a property set
     *
     * @param property
     * @return int
     */
    int addToPropertySet(org_eclipse_tahu_protobuf_Payload_PropertySet *propertySet, bool isBirth) override;

    /**
     * @brief Add a property to the property set
     *
     * @param property
     */
    void addProperty(const std::shared_ptr<Property> &property);
    void addProperties(const std::vector<std::shared_ptr<Property>> &properties);
};

#endif /* SRC_PROPERTIES_COMPLEX_PROPERTYSET */
