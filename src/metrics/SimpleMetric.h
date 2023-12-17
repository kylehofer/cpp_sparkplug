#ifndef INCLUDE_METRICS_SIMPLEMETRIC
#define INCLUDE_METRICS_SIMPLEMETRIC

#include "metrics/Metric.h"

/**
 * @brief Simple Metric implementation.
 * Does not react to commands.
 *
 */
class SimpleMetric : public Metric
{
public:
    /**
     * @brief Construct a new Sparkplug Metric
     *
     */
    SimpleMetric() : Metric(){};
    /**
     * @brief Construct a new Sparkplug Metric
     *
     * @param name The name of the Sparkplug Metric
     * @param data A pointer to a piece of data that will be as the first value of the metric
     * @param size The memory size required for the data
     * @param dataType Sparkplug Datatype
     */
    SimpleMetric(const char *name, void *data, size_t size, uint8_t dataType) : Metric(name, data, size, dataType){};
    /**
     * @brief Construct a new Sparkplug Metric
     *
     * @tparam T The data type that will be used to construct the data. Does not work with pointers.
     * @param name The name of the Sparkplug Metric
     * @param data The piece of data that will be as the first value of the metric
     * @param dataType Sparkplug Datatype
     */
    template <typename T>
    SimpleMetric(const char *name, T data, uint8_t dataType) : Metric(name, &data, sizeof(T), dataType){};

    /**
     * @brief Does nothing when a command is received.
     *
     * @param payload
     */
    void onCommand(org_eclipse_tahu_protobuf_Payload_Metric __attribute__((unused)) * payload){};
};

#endif /* INCLUDE_METRICS_SIMPLEMETRIC */
