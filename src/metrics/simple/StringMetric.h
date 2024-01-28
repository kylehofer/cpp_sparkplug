#ifndef SRC_METRICS_SIMPLE_STRINGMETRIC
#define SRC_METRICS_SIMPLE_STRINGMETRIC

#include "SimpleMetric.h"
#include <stdint.h>
#include <tahu.h>

class StringMetric : public SimpleMetric<string>
{
private:
    /**
     * @brief Construct a new string Sparkplug Metric
     *
     * @param name The name of the Sparkplug Metric
     * @param data The first value of the metric
     */
    StringMetric(const char *name, string data) : SimpleMetric(name, data, METRIC_DATA_TYPE_STRING){};

public:
    /**
     * @brief Construct a new string Sparkplug Metric shared pointer
     *
     * @param name The name of the Sparkplug Metric
     * @param data The first value of the metric
     * @return std::shared_ptr<StringMetric>
     */
    static std::shared_ptr<StringMetric> create(const char *name, string data)
    {
        return std::shared_ptr<StringMetric>(new StringMetric(name, data));
    }
};

#endif /* SRC_METRICS_SIMPLE_STRINGMETRIC */
