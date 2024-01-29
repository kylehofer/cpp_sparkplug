/*
 * File: BooleanProperty.h
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

#ifndef SRC_PROPERTIES_SIMPLE_BOOLEANPROPERTY
#define SRC_PROPERTIES_SIMPLE_BOOLEANPROPERTY

#include "SimpleProperty.h"
#include <stdint.h>
#include <tahu.h>

class BooleanProperty : public SimpleProperty<bool>
{
private:
    /**
     * @brief Construct a new bool Sparkplug Property
     *
     * @param name The name of the Sparkplug Property
     * @param data The first value of the property
     */
    BooleanProperty(const char *name, bool data) : SimpleProperty(name, data, PROPERTY_DATA_TYPE_BOOLEAN){};

public:
    /**
     * @brief Construct a new bool Sparkplug Property shared pointer
     *
     * @param name The name of the Sparkplug Property
     * @param data The first value of the property
     * @return std::shared_ptr<BooleanProperty>
     */
    static std::shared_ptr<BooleanProperty> create(const char *name, bool data)
    {
        return std::shared_ptr<BooleanProperty>(new BooleanProperty(name, data));
    }
};

#endif /* SRC_PROPERTIES_SIMPLE_BOOLEANPROPERTY */
