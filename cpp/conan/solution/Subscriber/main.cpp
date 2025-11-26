#include <iostream>

// Include DDSBus Fast DDS headers


// Include PubType header of your generated type


// Create your own DataReaderListener by inheriting from ddsbus::core::DataReaderListener<TopicType>


int main()
{
    // Perform DDS Setup

    // Start executing listener callbacks

    // Block to keep the subscriber alive

    // Stop dataReader internals

    return 0;
}
