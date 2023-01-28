# CreateKonfig
This tool will inject a serverside synched data store in an existing mod. The data class will persist to the server's profile folder and the data will be available on the client also.  You can have multiple data stores by creating multiple data classes with different names. The template is set up to prevent conflicts between the RPC calls. 
### Clone this with caution as I am actively developing the data class. As of 2023.1.28, added a registration system so that the server can inform client when client's data is update. ###

# Usage
![image](https://user-images.githubusercontent.com/44187035/212992705-bb5886aa-e1e0-4e45-9442-31581caf8c27.png)

1. Select the root folder of your mod.  This tool expects the Scripts folder to be a direct subfolder of your root.  e.g. `YourMod/Scripts/3_Game` The tool will inject the code into the appropriate module i.e. `YourMod/Scripts/3_Game/YourClassName`
2. __Config Class Name__ will be the class name of your data store. e.g. `MyPlayerDBStore`
3. __Config Instance Name__ will be the name of the variable that you will use to access your datastore.  It is defined in global space and is available everywhere even in other mods.  Accordingly, use a name that won't be accidentally duplicated. Best practice is to prefix your variable names.  e.g. `kproMyPlayerDB`

# Using the Data Store in Your Code

The data store instance is defined and instantiated in MissionBase:  `ref YourDataClassName<ref TestData> yourInstanceName`

## TestData
`TestData` is a placeholder class in the template. You must replace it with a data class that you create (and rename appropriately) to suit your needs.  

### Example
   
`class TestData
{
   string datamember1;
   int datamember2;
}`

Data is accessed as follows: 
   
`yourInstanceName.GetData().datamember1 = "This is test data";`
   
`yourInstanceName.GetData().datamember2 = 5;`

### RPC Limitations

You should be aware that enfusion RPCs have some limitations. They do not handle polymorphism well and do not handle references to classes well. If your data class includes a variable that is defined as a base class but in reality holds a child object that extends the base, DayZ won't be able to serialize the child and may crash when it tries. Similarly, Dayz will crash if your data class holds a reference to another class that was created outside the data class. For example, a data class that takes another class in its constructor like this will crash: 

`class dataclass 
{
  void dataclass(Myclass myclass) {
   myclass.DoSomething();
  } 
}`

But it appears that any class created inside your data class will serialize just fine:

`Myclass myclass = new Myclass();` 

If your code crashes at the point that the native RPC is called but before the RPC reaches the other machine (i.e. crashes the client on the rpc call and server never gets the RPC), you are probably dealing with the situation. The solution is to pass the data that you need as a string, or other primitive type, rather than as a reference to a class.

## Methods

### On the client
`yourInstanceName.Register(this, "yourhandlerclassname")` to register a client-side handler with the server. The template data class has an example in the comments.

`yourInstanceName.UpdateServer()`  to send client data to server.
   
`yourInstanceName.UpdateClient()` to request data from server.

### On the server
`yourInstanceName.UpdateClient()` to send server data to client.

### On either server or client
`yourInstanceName.SaveFile()`  to save data

`yourInstanceName.OpenFile("Filename")` to open a file of name "Filename.json"

`yourInstanceName.NewFile("Filename")` to create a new file with name "Filename.json"

## Current limitations
The synch between client and server is not instantaneous [you can blame Einstein for this bs law of physics ;)].  This means that the data is *not* immediately available on the client after the client instructs the server to open a new data store and update the data to the client. I've implemented a synching system that goes a long way to overcoming this limitiation. The client can now register itself with the server so that it is notified when the data updates on the client. 


