

f = open("./Int8Property.h", "r")
contents = f.read()



map = [
    {
        "name": "UInt8Property",
        "replace": [
            ["INT8", "UINT8"],
            ["int8_t", "uint8_t"],
            ["Int8", "UInt8"],
        ]
    },
    {
        "name": "UInt16Property",
        "replace": [
            ["INT8", "UINT16"],
            ["int8_t", "uint16_t"],
            ["Int8", "UInt16"],
        ]
    },
    {
        "name": "UInt32Property",
        "replace": [
            ["INT8", "UINT32"],
            ["int8_t", "uint32_t"],
            ["Int8", "UInt32"],
        ]
    },
    {
        "name": "UInt64Property",
        "replace": [
            ["INT8", "UINT64"],
            ["int8_t", "uint64_t"],
            ["Int8", "UInt64"],
        ]
    },
    {
        "name": "DateTimeProperty",
        "replace": [
            ["SRC_PROPERTIES_SIMPLE_INT8PROPERTY", "SRC_PROPERTIES_SIMPLE_DATETIMEPROPERTY"],
            ["PROPERTY_DATA_TYPE_INT8", "PROPERTY_DATA_TYPE_DATETIME"],
            ["int8_t", "uint64_t"],
            ["Int8", "DateTime"],
        ]
    },
    {
        "name": "Int16Property",
        "replace": [
            ["INT8", "INT16"],
            ["int8_t", "int16_t"],
            ["Int8", "Int16"],
        ]
    },
    {
        "name": "Int32Property",
        "replace": [
            ["INT8", "INT32"],
            ["int8_t", "int32_t"],
            ["Int8", "Int32"],
        ]
    },
    {
        "name": "Int64Property",
        "replace": [
            ["INT8", "INT64"],
            ["int8_t", "int64_t"],
            ["Int8", "Int64"],
        ]
    },
    {
        "name": "FloatProperty",
        "replace": [
            ["INT8", "FLOAT"],
            ["int8_t", "float"],
            ["Int8", "Float"],
        ]
    },
    {
        "name": "DoubleProperty",
        "replace": [
            ["INT8", "DOUBLE"],
            ["int8_t", "double"],
            ["Int8", "Double"],
        ]
    },
    {
        "name": "BooleanProperty",
        "replace": [
            ["INT8", "BOOLEAN"],
            ["int8_t", "bool"],
            ["Int8", "Boolean"],
        ]
    },
    {
        "name": "StringProperty",
        "replace": [
            ["INT8", "STRING"],
            ["int8_t", "string"],
            ["Int8", "String"],
        ]
    }
]

for item in map:
    newContents = contents
    for replace in item["replace"]:
        newContents = newContents.replace(replace[0], replace[1])
    f = open("./" + item["name"] + ".h", "w")
    f.write(newContents)
    f.close()
