#ifndef SRC_METRICS_SIMPLE_INT32METRIC
#define SRC_METRICS_SIMPLE_INT32METRIC

#include "SimpleMetric.h"
#include <stdint.h>
#include <tahu.h>

class Int32Metric : public SimpleMetric<int32_t>
{
private:
    /**
     * @brief Construct a new int32_t Sparkplug Metric
     *
     * @param name The name of the Sparkplug Metric
     * @param data The first value of the metric
     */
    Int32Metric(const char *name, int32_t data) : SimpleMetric(name, data, METRIC_DATA_TYPE_INT32){};

public:
    /**
     * @brief Construct a new int32_t Sparkplug Metric shared pointer
     *
     * @param name The name of the Sparkplug Metric
     * @param data The first value of the metric
     * @return std::shared_ptr<Int32Metric>
     */
    static std::shared_ptr<Int32Metric> create(const char *name, int32_t data)
    {
        return std::shared_ptr<Int32Metric>(new Int32Metric(name, data));
    }
};

#endif /* SRC_METRICS_SIMPLE_INT32METRIC */
