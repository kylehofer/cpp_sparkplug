#include "PropertySet.h"
#include "pb_decode.h"
#include <iostream>

int PropertySet::addToPropertySet(org_eclipse_tahu_protobuf_Payload_PropertySet *propertySet, bool isBirth)
{
    if (isBirth)
    {
        org_eclipse_tahu_protobuf_Payload_PropertySet nestedSet;
        memset(&nestedSet, 0, sizeof(org_eclipse_tahu_protobuf_Payload_PropertySet));

        bool added = false;

        for (auto &property : properties)
        {
            property->addToPropertySet(&nestedSet, isBirth);
            added = true;
        }

        if (added)
        {
            int result = add_property_to_set(
                propertySet,
                getName(),
                PROPERTY_DATA_TYPE_PROPERTYSET,
                &nestedSet,
                0);
            // We need to release our property set here as when adding a property set to a property set
            // all data will be duplicated
            pb_release(org_eclipse_tahu_protobuf_Payload_PropertySet_fields, &nestedSet);
            return result;
        }
    }
    return 0;
}

void PropertySet::addProperty(const std::shared_ptr<Property> &property)
{
    properties.push_back(std::move(property));
}

void PropertySet::addProperties(const std::vector<std::shared_ptr<Property>> &properties)
{

    addProperties(properties);
}
std::shared_ptr<PropertySet> PropertySet::create(const char *name)
{

    return std::shared_ptr<PropertySet>(new PropertySet(name));
}

std::shared_ptr<PropertySet> PropertySet::create(const char *name, const std::shared_ptr<Property> &property)
{
    auto set = create(name);
    set->addProperty(property);
    return set;
}

std::shared_ptr<PropertySet> PropertySet::create(const char *name, const std::vector<std::shared_ptr<Property>> &properties)
{
    auto set = create(name);
    set->addProperties(properties);
    return set;
}
