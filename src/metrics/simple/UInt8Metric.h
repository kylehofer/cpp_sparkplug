#ifndef SRC_METRICS_SIMPLE_UINT8METRIC
#define SRC_METRICS_SIMPLE_UINT8METRIC

#include "SimpleMetric.h"
#include <stdint.h>
#include <tahu.h>

class UInt8Metric : public SimpleMetric<uint8_t>
{
private:
    /**
     * @brief Construct a new uint8_t Sparkplug Metric
     *
     * @param name The name of the Sparkplug Metric
     * @param data The first value of the metric
     */
    UInt8Metric(const char *name, uint8_t data) : SimpleMetric(name, data, METRIC_DATA_TYPE_UINT8){};

public:
    /**
     * @brief Construct a new uint8_t Sparkplug Metric shared pointer
     *
     * @param name The name of the Sparkplug Metric
     * @param data The first value of the metric
     * @return std::shared_ptr<UInt8Metric>
     */
    static std::shared_ptr<UInt8Metric> create(const char *name, uint8_t data)
    {
        return std::shared_ptr<UInt8Metric>(new UInt8Metric(name, data));
    }
};

#endif /* SRC_METRICS_SIMPLE_UINT8METRIC */
