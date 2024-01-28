#ifndef SRC_METRICS_SIMPLE_FLOATMETRIC
#define SRC_METRICS_SIMPLE_FLOATMETRIC

#include "SimpleMetric.h"
#include <stdint.h>
#include <tahu.h>

class FloatMetric : public SimpleMetric<float>
{
private:
    /**
     * @brief Construct a new float Sparkplug Metric
     *
     * @param name The name of the Sparkplug Metric
     * @param data The first value of the metric
     */
    FloatMetric(const char *name, float data) : SimpleMetric(name, data, METRIC_DATA_TYPE_FLOAT){};

public:
    /**
     * @brief Construct a new float Sparkplug Metric shared pointer
     *
     * @param name The name of the Sparkplug Metric
     * @param data The first value of the metric
     * @return std::shared_ptr<FloatMetric>
     */
    static std::shared_ptr<FloatMetric> create(const char *name, float data)
    {
        return std::shared_ptr<FloatMetric>(new FloatMetric(name, data));
    }
};

#endif /* SRC_METRICS_SIMPLE_FLOATMETRIC */
