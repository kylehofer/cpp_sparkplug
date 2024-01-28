#ifndef SRC_METRICS_SIMPLE_INT16METRIC
#define SRC_METRICS_SIMPLE_INT16METRIC

#include "SimpleMetric.h"
#include <stdint.h>
#include <tahu.h>

class Int16Metric : public SimpleMetric<int16_t>
{
private:
    /**
     * @brief Construct a new int16_t Sparkplug Metric
     *
     * @param name The name of the Sparkplug Metric
     * @param data The first value of the metric
     */
    Int16Metric(const char *name, int16_t data) : SimpleMetric(name, data, METRIC_DATA_TYPE_INT16){};

public:
    /**
     * @brief Construct a new int16_t Sparkplug Metric shared pointer
     *
     * @param name The name of the Sparkplug Metric
     * @param data The first value of the metric
     * @return std::shared_ptr<Int16Metric>
     */
    static std::shared_ptr<Int16Metric> create(const char *name, int16_t data)
    {
        return std::shared_ptr<Int16Metric>(new Int16Metric(name, data));
    }
};

#endif /* SRC_METRICS_SIMPLE_INT16METRIC */
