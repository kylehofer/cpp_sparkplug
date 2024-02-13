/*
 * File: Property.h
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

#ifndef SRC_PROPERTIES_PROPERTY
#define SRC_PROPERTIES_PROPERTY

#include <tahu.h>

class Property
{
private:
    char *name = NULL;
    void *data = NULL;
    uint8_t dataType;
    size_t size;
    bool dirty = false;

protected:
public:
    Property();
    /**
     * @brief Construct a new Sparkplug property
     *
     * @param name The name of the Sparkplug property
     * @param data A pointer to a piece of data that will be as the first value of the property
     * @param size The memory size required for the data
     * @param dataType Sparkplug Datatype
     */
    Property(const char *name, void *data, size_t size, uint8_t dataType);
    virtual ~Property();

    Property &operator=(const Property &right);
    Property &operator=(Property right);

    /**
     * @brief
     *
     * @param property
     * @return int
     */
    virtual int addToPropertySet(org_eclipse_tahu_protobuf_Payload_PropertySet *propertySet, bool isBirth);
    /**
     * @brief Sets a new value of the property
     *
     * @param data Pointer to the a piece of data that will be copied to the property
     */
    void setValue(void *data);
    /**
     * @brief Whether the property value is dirty, and can be published
     *
     * @return true
     * @return false
     */
    bool isDirty();
    /**
     * @brief Used to mark the property that is had been published
     *
     */
    void published();
    /**
     * @brief Returns the pointer to the property data
     *
     * @return void*
     */
    void *getData();
    /**
     * @brief Returns the pointer to the property data
     *
     * @return void*
     */
    const char *getName();
};

#endif /* SRC_PROPERTIES_PROPERTY */
