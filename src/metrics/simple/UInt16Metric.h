#ifndef SRC_METRICS_SIMPLE_UINT16METRIC
#define SRC_METRICS_SIMPLE_UINT16METRIC

#include "SimpleMetric.h"
#include <stdint.h>
#include <tahu.h>

class UInt16Metric : public SimpleMetric<uint16_t>
{
private:
    /**
     * @brief Construct a new uint16_t Sparkplug Metric
     *
     * @param name The name of the Sparkplug Metric
     * @param data The first value of the metric
     */
    UInt16Metric(const char *name, uint16_t data) : SimpleMetric(name, data, METRIC_DATA_TYPE_UINT16){};

public:
    /**
     * @brief Construct a new uint16_t Sparkplug Metric shared pointer
     *
     * @param name The name of the Sparkplug Metric
     * @param data The first value of the metric
     * @return std::shared_ptr<UInt16Metric>
     */
    static std::shared_ptr<UInt16Metric> create(const char *name, uint16_t data)
    {
        return std::shared_ptr<UInt16Metric>(new UInt16Metric(name, data));
    }
};

#endif /* SRC_METRICS_SIMPLE_UINT16METRIC */
