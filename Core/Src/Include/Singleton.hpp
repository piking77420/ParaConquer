/*
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */

#pragma once
#include "CoreHeader.hpp"

#include <memory>

BEGIN_PCCORE

/**
 * \brief Templated Singleton class
 *
 * Note - There is not destructor as this object exists
 *        the entire time of the program.
 *        All resources will be released when the program is terminated.
 *        Any implementation of a Singleton should not hold resources
 *        such as open files, ports, or other items that would be
 *        cleaned up at destruction time.
 */
template <class _class_>
class Singleton
{
public:
    
    /**
     * Returns a reference to the singleton.
     * This is a static function.
     *
     * Example use case:
     *
     *      class SingletonTest1 : public Singleton<SingletonTest1> {};
     *      SingletonTest1::instance();
     *
     * @return Returns a reference to the singleton object
     */
    static _class_& Instance()
    {
        if (instance_ == nullptr)
        {
            instance_ = std::unique_ptr<_class_>(new _class_);
        }
        
        return *instance_;
    }
    
private:
    
    /// The Singleton pointer for the class.
    static inline std::unique_ptr<_class_> instance_ = nullptr;
};

END_PCCORE

// Thanks to BobBrownConsulting
// https://github.com/BobBrownConsulting/bbc/blob/master/src/utils/Singleton.h