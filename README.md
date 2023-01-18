# CreateKonfig
This tool will inject a serverside synched data store in an existing mod. The data class will persist to the server's profile folder and the data will be available on the client also.  You can have multiple data stores by creating multiple data classes with different names. The template is set up to prevent conflicts between the RPC calls. 
### Clone this with caution as I am actively developing and still testing the data class. As of 2023.1.16, I would only use this as a learning tool. I will edit this readme after I am finished testing to reflect that fact. ###

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

## Methods

### On the client
`yourInstanceName.UpdateServer()`  to send client data to server.
   
`yourInstanceName.UpdateClient()` to request data from server.

### On the server
`yourInstanceName.UpdateClient()` to send server data to client.

### On either server or client
`yourInstanceName.SaveFile()`  to save data

`yourInstanceName.OpenFile("Filename")` to open a file of name "Filename.json"

`yourInstanceName.NewFile("Filename")` to create a new file with name "Filename.json"

## Current limitations
The synch between client and server is not instantaneous.  This means that the data is *not* immediately available on the client after the client instructs the server to open a new data store and update the data to the client. I'm still experimenting with this but it may not be suitable for a highly volatile application. CF has a network synched data class that might be more appropriate for those situations.


