#ifndef SRC_METRICS_SIMPLE_BOOLEANMETRIC
#define SRC_METRICS_SIMPLE_BOOLEANMETRIC

#include "SimpleMetric.h"
#include <stdint.h>
#include <tahu.h>

class BooleanMetric : public SimpleMetric<bool>
{
private:
    /**
     * @brief Construct a new bool Sparkplug Metric
     *
     * @param name The name of the Sparkplug Metric
     * @param data The first value of the metric
     */
    BooleanMetric(const char *name, bool data) : SimpleMetric(name, data, METRIC_DATA_TYPE_BOOLEAN){};

public:
    /**
     * @brief Construct a new bool Sparkplug Metric shared pointer
     *
     * @param name The name of the Sparkplug Metric
     * @param data The first value of the metric
     * @return std::shared_ptr<BooleanMetric>
     */
    static std::shared_ptr<BooleanMetric> create(const char *name, bool data)
    {
        return std::shared_ptr<BooleanMetric>(new BooleanMetric(name, data));
    }
};

#endif /* SRC_METRICS_SIMPLE_BOOLEANMETRIC */
