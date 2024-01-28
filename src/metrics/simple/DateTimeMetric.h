#ifndef SRC_METRICS_SIMPLE_DATETIMEMETRIC
#define SRC_METRICS_SIMPLE_DATETIMEMETRIC

#include "SimpleMetric.h"
#include <stdint.h>
#include <tahu.h>

class DateTimeMetric : public SimpleMetric<uint64_t>
{
private:
    /**
     * @brief Construct a new uint64_t Sparkplug Metric
     *
     * @param name The name of the Sparkplug Metric
     * @param data The first value of the metric
     */
    DateTimeMetric(const char *name, uint64_t data) : SimpleMetric(name, data, METRIC_DATA_TYPE_DATETIME){};

public:
    /**
     * @brief Construct a new uint64_t Sparkplug Metric shared pointer
     *
     * @param name The name of the Sparkplug Metric
     * @param data The first value of the metric
     * @return std::shared_ptr<DateTimeMetric>
     */
    static std::shared_ptr<DateTimeMetric> create(const char *name, uint64_t data)
    {
        return std::shared_ptr<DateTimeMetric>(new DateTimeMetric(name, data));
    }
};

#endif /* SRC_METRICS_SIMPLE_DATETIMEMETRIC */
