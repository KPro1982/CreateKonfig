# CreateKonfig
This tool will inject a serverside synched data store in an existing mod. The data class will persist to the servers profile folder and the data will be available on the client also.  You can have multiple data stores by creating multiple data classes with different names. The template is set up to prevent conflicts between the RPC calls. **Clone this with caution as I am actively developing and still testing the data class. As of 2023.1.16, I would only use this as a learning tool. I will edit this readme after I am finished testing to reflect that fact.**

# Usage

The data class is defined and instantiated in MissionBase:  ref YourDataClassName<ref TestData> yourInstanceName
TestData is a class that you must create (and rename appropriately) to hold your data.  

##Example
class TestData
{
   string datamember1;
   int datamember2;
}

Data is accessed as follows: 
yourInstanceName.GetData().datamember1 = "This is test data";
yourInstanceName.GetData().datamember2 = 5;

## Methods

### On the client
yourInstanceName.UpdateServer()  to send client data to server.

### On the server
yourInstanceName.UpdateClient() to send server data to client.

### On either server or client
yourInstanceName.SaveFile()  to save data

yourInstanceName.OpenFile("Filename") to open a file of name "Filename.json"

yourInstanceName.NewFile("Filename")

## Current limitations
The synch between client and server is not instantaneous.  This means that the data is *not* immediately available on the client after the client instructs the server to open a new data store and update the data to the client. I'm still experimenting with this but it may not be suitable for a highly volatile application. CF has a network synched data class that might be more appropriate for those situation.


