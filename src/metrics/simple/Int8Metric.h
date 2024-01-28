#ifndef SRC_METRICS_SIMPLE_INT8METRIC
#define SRC_METRICS_SIMPLE_INT8METRIC

#include "SimpleMetric.h"
#include <stdint.h>
#include <tahu.h>

class Int8Metric : public SimpleMetric<int8_t>
{
private:
    /**
     * @brief Construct a new int8_t Sparkplug Metric
     *
     * @param name The name of the Sparkplug Metric
     * @param data The first value of the metric
     */
    Int8Metric(const char *name, int8_t data) : SimpleMetric(name, data, METRIC_DATA_TYPE_INT8){};

public:
    /**
     * @brief Construct a new int8_t Sparkplug Metric shared pointer
     *
     * @param name The name of the Sparkplug Metric
     * @param data The first value of the metric
     * @return std::shared_ptr<Int8Metric>
     */
    static std::shared_ptr<Int8Metric> create(const char *name, int8_t data)
    {
        return std::shared_ptr<Int8Metric>(new Int8Metric(name, data));
    }
};

#endif /* SRC_METRICS_SIMPLE_INT8METRIC */
