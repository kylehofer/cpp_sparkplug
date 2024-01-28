#ifndef SRC_METRICS_SIMPLE_INT64METRIC
#define SRC_METRICS_SIMPLE_INT64METRIC

#include "SimpleMetric.h"
#include <stdint.h>
#include <tahu.h>

class Int64Metric : public SimpleMetric<int64_t>
{
private:
    /**
     * @brief Construct a new int64_t Sparkplug Metric
     *
     * @param name The name of the Sparkplug Metric
     * @param data The first value of the metric
     */
    Int64Metric(const char *name, int64_t data) : SimpleMetric(name, data, METRIC_DATA_TYPE_INT64){};

public:
    /**
     * @brief Construct a new int64_t Sparkplug Metric shared pointer
     *
     * @param name The name of the Sparkplug Metric
     * @param data The first value of the metric
     * @return std::shared_ptr<Int64Metric>
     */
    static std::shared_ptr<Int64Metric> create(const char *name, int64_t data)
    {
        return std::shared_ptr<Int64Metric>(new Int64Metric(name, data));
    }
};

#endif /* SRC_METRICS_SIMPLE_INT64METRIC */
