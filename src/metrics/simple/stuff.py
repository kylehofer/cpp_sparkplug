

f = open("./Int8Metric.h", "r")
contents = f.read()

map = [
    {
        "name": "UInt8Metric",
        "replace": [
            ["INT8", "UINT8"],
            ["int8_t", "uint8_t"],
            ["Int8", "UInt8"],
        ]
    },
    {
        "name": "UInt16Metric",
        "replace": [
            ["INT8", "UINT16"],
            ["int8_t", "uint16_t"],
            ["Int8", "UInt16"],
        ]
    },
    {
        "name": "UInt32Metric",
        "replace": [
            ["INT8", "UINT32"],
            ["int8_t", "uint32_t"],
            ["Int8", "UInt32"],
        ]
    },
    {
        "name": "UInt64Metric",
        "replace": [
            ["INT8", "UINT64"],
            ["int8_t", "uint64_t"],
            ["Int8", "UInt64"],
        ]
    },
    {
        "name": "DateTimeMetric",
        "replace": [
            ["SRC_METRICS_SIMPLE_INT8METRIC", "SRC_METRICS_SIMPLE_DATETIMEMETRIC"],
            ["METRIC_DATA_TYPE_INT8", "METRIC_DATA_TYPE_DATETIME"],
            ["int8_t", "uint64_t"],
            ["Int8", "DateTime"],
        ]
    },
    {
        "name": "Int16Metric",
        "replace": [
            ["INT8", "INT16"],
            ["int8_t", "int16_t"],
            ["Int8", "Int16"],
        ]
    },
    {
        "name": "Int32Metric",
        "replace": [
            ["INT8", "INT32"],
            ["int8_t", "int32_t"],
            ["Int8", "Int32"],
        ]
    },
    {
        "name": "Int64Metric",
        "replace": [
            ["INT8", "INT64"],
            ["int8_t", "int64_t"],
            ["Int8", "Int64"],
        ]
    },
    {
        "name": "FloatMetric",
        "replace": [
            ["INT8", "FLOAT"],
            ["int8_t", "float"],
            ["Int8", "Float"],
        ]
    },
    {
        "name": "DoubleMetric",
        "replace": [
            ["INT8", "DOUBLE"],
            ["int8_t", "double"],
            ["Int8", "Double"],
        ]
    },
    {
        "name": "BooleanMetric",
        "replace": [
            ["INT8", "BOOLEAN"],
            ["int8_t", "bool"],
            ["Int8", "Boolean"],
        ]
    },
    {
        "name": "StringMetric",
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
