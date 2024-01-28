#ifndef SRC_METRICS_SIMPLE_DOUBLEMETRIC
#define SRC_METRICS_SIMPLE_DOUBLEMETRIC

#include "SimpleMetric.h"
#include <stdint.h>
#include <tahu.h>

class DoubleMetric : public SimpleMetric<double>
{
private:
    /**
     * @brief Construct a new double Sparkplug Metric
     *
     * @param name The name of the Sparkplug Metric
     * @param data The first value of the metric
     */
    DoubleMetric(const char *name, double data) : SimpleMetric(name, data, METRIC_DATA_TYPE_DOUBLE){};

public:
    /**
     * @brief Construct a new double Sparkplug Metric shared pointer
     *
     * @param name The name of the Sparkplug Metric
     * @param data The first value of the metric
     * @return std::shared_ptr<DoubleMetric>
     */
    static std::shared_ptr<DoubleMetric> create(const char *name, double data)
    {
        return std::shared_ptr<DoubleMetric>(new DoubleMetric(name, data));
    }
};

#endif /* SRC_METRICS_SIMPLE_DOUBLEMETRIC */
