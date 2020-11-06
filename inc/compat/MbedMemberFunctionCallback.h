/*
The MIT License (MIT)

Copyright (c) 2017 Lancaster University.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#ifndef MBEDMEMBERFUNCTION_CALLBACK_H
#define MBEDMEMBERFUNCTION_CALLBACK_H

#include "CodalConfig.h"
#include "CodalCompat.h"

/**
  * Class definition for a MbedMemberFunctionCallback.
  *
  * C++ member functions (also known as methods) have a more complex
  * representation than normal C functions. This class allows a reference to
  * a C++ member function to be stored then called at a later date.
  *
  * This class is used extensively by the DeviceMessageBus to deliver
  * events to C++ methods.
  */
namespace codal
{
    class MbedMemberFunctionCallback
    {
        private:
        void* object;
        uint32_t method[4];
        void (*mbed_invoke)(void *object, uint32_t *method);
        template <typename T> static void methodCall(void* object, uint32_t*method);

        public:

        /**
          * Constructor. Creates a MbedMemberFunctionCallback based on a pointer to given method.
          *
          * @param object The object the callback method should be invooked on.
          *
          * @param method The method to invoke.
          */
        template <typename T> MbedMemberFunctionCallback(T* object, void (T::*method)());

        /**
          * A comparison of two MbedMemberFunctionCallback objects.
          *
          * @return true if the given MbedMemberFunctionCallback is equivalent to this one, false otherwise.
          */
        bool operator==(const MbedMemberFunctionCallback &mfc);

        /**
          * Calls the method reference held by this MbedMemberFunctionCallback.
          *
          */
        void fire();
    };

    /**
      * Constructor. Creates a MbedMemberFunctionCallback based on a pointer to given method.
      *
      * @param object The object the callback method should be invooked on.
      *
      * @param method The method to invoke.
      */
    template <typename T>
    MbedMemberFunctionCallback::MbedMemberFunctionCallback(T* object, void (T::*method)())
    {
        this->object = object;
        memclr(this->method, sizeof(this->method));
        memcpy(this->method, &method, sizeof(method));
        mbed_invoke = &MbedMemberFunctionCallback::methodCall<T>;
    }

    /**
      * A template used to create a static method capable of invoking a C++ member function (method)
      * based on the given parameters.
      *
      * @param object The object the callback method should be invooked on.
      *
      * @param method The method to invoke.
      *
      */
    template <typename T>
    void MbedMemberFunctionCallback::methodCall(void *object, uint32_t *method)
    {
        T* o = (T*)object;
        void (T::*m)();
        memcpy(&m, method, sizeof(m));

        (o->*m)();
    }
}

#warning "Use of mbed with CODAL is not recommended! These classes will not always behave as expected and are provided to attempt to support existing extensions. Please write your extension using CODAL."

#endif
