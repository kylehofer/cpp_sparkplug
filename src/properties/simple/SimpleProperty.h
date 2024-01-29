/*
 * File: SimpleProperty.h
 * Project: cpp_sparkplug
 * Created Date: Sunday January 28th 2024
 * Author: Kyle Hofer
 *
 * MIT License
 *
 * Copyright (c) 2024 Kyle Hofer
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * HISTORY:
 */

#ifndef SRC_PROPERTIES_SIMPLE_SIMPLEPROPERTY
#define SRC_PROPERTIES_SIMPLE_SIMPLEPROPERTY

#include "properties/Property.h"
#include <memory>

/**
 * @brief Simple Property implementation.
 * Does not react to commands.
 *
 */
template <typename T>
class SimpleProperty : public Property
{
public:
    /**
     * @brief Construct a new Sparkplug Property
     *
     */
    SimpleProperty() : Property(){};
    /**
     * @brief Construct a new Sparkplug Property
     *
     * @param name The name of the Sparkplug Property
     * @param data A pointer to a first value of the property
     * @param size The memory size required for the data
     * @param dataType Sparkplug Datatype
     */
    SimpleProperty(const char *name, void *data, size_t size, uint8_t dataType) : Property(name, data, size, dataType){};
    /**
     * @brief Construct a new Sparkplug Property
     *
     * @param name The name of the Sparkplug Property
     * @param data The first value of the property
     * @param dataType Sparkplug Datatype
     */
    SimpleProperty(const char *name, T data, uint8_t dataType) : Property(name, &data, sizeof(T), dataType){};

    /**
     * @brief Sets a new value of the property
     *
     * @param data Pointer to the a piece of data that will be copied to the property
     */
    void setValue(T value)
    {
        Property::setValue(&value);
    };

    T &getValue()
    {
        return *(T *)data;
    };
};

#endif /* SRC_PROPERTIES_SIMPLE_SIMPLEPROPERTY */
