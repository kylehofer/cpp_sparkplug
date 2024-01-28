#ifndef SRC_METRICS_SIMPLE_UINT64METRIC
#define SRC_METRICS_SIMPLE_UINT64METRIC

#include "SimpleMetric.h"
#include <stdint.h>
#include <tahu.h>

class UInt64Metric : public SimpleMetric<uint64_t>
{
private:
    /**
     * @brief Construct a new uint64_t Sparkplug Metric
     *
     * @param name The name of the Sparkplug Metric
     * @param data The first value of the metric
     */
    UInt64Metric(const char *name, uint64_t data) : SimpleMetric(name, data, METRIC_DATA_TYPE_UINT64){};

public:
    /**
     * @brief Construct a new uint64_t Sparkplug Metric shared pointer
     *
     * @param name The name of the Sparkplug Metric
     * @param data The first value of the metric
     * @return std::shared_ptr<UInt64Metric>
     */
    static std::shared_ptr<UInt64Metric> create(const char *name, uint64_t data)
    {
        return std::shared_ptr<UInt64Metric>(new UInt64Metric(name, data));
    }
};

#endif /* SRC_METRICS_SIMPLE_UINT64METRIC */
