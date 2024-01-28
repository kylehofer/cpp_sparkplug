#ifndef SRC_METRICS_SIMPLE_UINT32METRIC
#define SRC_METRICS_SIMPLE_UINT32METRIC

#include "SimpleMetric.h"
#include <stdint.h>
#include <tahu.h>

class UInt32Metric : public SimpleMetric<uint32_t>
{
private:
    /**
     * @brief Construct a new uint32_t Sparkplug Metric
     *
     * @param name The name of the Sparkplug Metric
     * @param data The first value of the metric
     */
    UInt32Metric(const char *name, uint32_t data) : SimpleMetric(name, data, METRIC_DATA_TYPE_UINT32){};

public:
    /**
     * @brief Construct a new uint32_t Sparkplug Metric shared pointer
     *
     * @param name The name of the Sparkplug Metric
     * @param data The first value of the metric
     * @return std::shared_ptr<UInt32Metric>
     */
    static std::shared_ptr<UInt32Metric> create(const char *name, uint32_t data)
    {
        return std::shared_ptr<UInt32Metric>(new UInt32Metric(name, data));
    }
};

#endif /* SRC_METRICS_SIMPLE_UINT32METRIC */
