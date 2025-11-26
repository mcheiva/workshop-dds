# Exercise 3: Make Your Own Subscriber
In this exercise we create a simple Subscriber with a DataReader that connects and listens on a single topic based on the initial IDL provided in the `idls` folder.
Open the Subscriber project under `Solution/Subscriber` and include the using statements required for the API. These pull in the native Fast DDS components, reduce boilerplate, and manage internal DDS entity lifecycles just like the Publisher example.

```csharp
using Eiva.DDSBus;
using Eiva.DDSBus.Core;
```

Next, include the topic types. This is identical to the Publisher example since we wish to connect to the same topic type.

```csharp
using IDL;
```

Next define a listener. We create `MyExampleListener` inheriting from the API abstract class `DataReaderListener<TopicType>`. This is where the Subscriber code differs slightly from the Publisher code because the main logic for handling incoming samples lives here. I recommend studying the [documentation](https://fast-dds.docs.eprosima.com/en/latest/fastdds/dds_layer/subscriber/dataReaderListener/dataReaderListener.html#datareaderlistener) for DataReaderListener to understand what the callbacks mean.

Key points for the listener implementation:
- We override `OnSubscriptionMatched` and `OnRequestedIncompatibleQos` just like the Publisher example.
- We also implement `OnDataAvailable`, invoked for every received data sample.
- The class is using generics so it knows the concrete `<TopicType>` (here `Awesome`).
- In this example we only acknowledge the received sample (no storage or expensive processing).
```csharp
// ----------- Example Listener Class ---------//
class MyExampleListener : Eiva.DDSBus.Core.DataReaderListener<Awesome>
{
    public override void OnDataAvailable(Awesome data)
    {
        Console.WriteLine($"[Subscriber] Received data sample with id = { data.id() }");
    }

    public override void OnSubscriptionMatched(FastDDS.SubscriptionMatchedStatus status)
    {
        Console.WriteLine(
            $"[Subscriber] Match status changed: {status.current_count} publisher(s) connected." +
            $" {status.total_count} total connection(s)");
    }

    public override void OnRequestedIncompatibleQos(FastDDS.IncompatibleQosStatus status)
    {
        Console.WriteLine(
            $"[Subscriber] Incompatible QoS requested. Total count: { status.total_count } " +
            $", last violated policy ID: { status.last_policy_id }");
    }
}
```
Similarly to the DataWriter, the actual setup of the DDS entities is trivial and closely resembles the Publisher. There are two types of DataReaders: a special implementation called `WaitsetDataReader` and a regular `DataReader` (constructed using `CreateDataReader`). The `WaitsetDataReader` uses an internal thread to process and invoke listener callbacks, freeing resources so the underlying DataReader can continue processing new samples. In the regular DataReader, callbacks such as `OnDataAvailable` run inline and block further processing. This is fine for small topics, but client code should generally favor `WaitsetDataReader` as advised by eProsima.
```csharp
// ----------- SETUP ----------- //

Participant parcipant = new Participant(69);
Subscriber subscriber = parcipant.CreateSubscriber();
Topic<Awesome> topic = parcipant.CreateTopic<Awesome>("MCH/AwesomeTopic");

MyExampleListener listener = new MyExampleListener();
IDataReader dataReader = subscriber.CreateWaitsetDataReader(topic, listener: listener);
```
Similarly to the Publisher example, in production code you might instantiate the `WaitsetDataReader` inside the `MyExampleListener` constructor so logging and lifecycle concerns remain co‑located. In that case pass `this` instead of `&listener`. For reference (not explicitly used in the exercises to follow) such an example might look like. This should be seen as a reference for later, but for now stick to the original listener implementation provided above.
```csharp
// ----------- Example Listener Class ---------//
class MyExampleListener :  Eiva.DDSBus.Core.DataReaderListener<Awesome>
{
    private IDataReader _dataReader;
    public MyExampleListener(Participant participant, Subscriber subscriber)
    {
        Topic<Awesome> topic = participant.CreateTopic<Awesome>("MyAwesomeTopicName");
        _dataReader = subscriber.CreateWaitsetDataReader(topic, listener: this);
    }

    // your overrides here
    // ......
}
```

Client code after setting up the DDS entities is simple for the DataReader use case: tell the DataReader to start processing callbacks. In the case of `WaitsetDataReader` this starts an internal processing thread for maximum performance.

```csharp
// ----------- Start Listening ----------- //
dataReader.Start();
```

Similarly to the Publisher we keep the application alive until user input tells us to stop. This can be done in many ways; here we simply block the terminal.
```csharp
// ----------- Block Application ----------- //
Console.WriteLine("Subscriber is running. Press any key to stop...");
Console.ReadKey();
```

After the user prompts exit, stop the internal thread to allow DDS to exit gracefully.
```csharp
// ----------- Stop Listening ----------- //
reader.Stop();
```

Start the Subscriber and verify that everything compiles. You should now be able to test it with the Publisher that was created in the previous exercise. You should see that the `OnSubscriptionMatched` callback has been called on the subscriber side and that `OnPublicationMatched` was called on the publisher, this indicates that they are connected and samples should be received and processed in the `OnDataAvailable` callback when your implemented publish flow calls `Publish` on the DataWriter.

If everything went well you should have a working two working Pub/Sub applications, you can experiment with altering the IDL to include more nested types or bigger arrays (just remember to bound them! ). In the next exercise we will explore how altering the quality of service settings we omitted in these setup exercises affect the performance.