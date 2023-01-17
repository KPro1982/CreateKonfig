class npcDefinition<Class T>
{
	// the root should reflect your mod root folder. This is tytically your mod name
	static string Root = "$profile:\\ConfigTest\\";
	protected string m_currentfilename;
	ref T m_data;
	
	void npcDefinition(string filename, ref T data)
	{
		auto trace = CF_Trace_2("npcDefinition", "npcDefinition").Add(filename).Add(data);
		CF_Log.Level = CF_LogLevel.TRACE;	
	

		SetData(data);
		
		
							
		m_currentfilename = filename;
		if(GetGame().IsDedicatedServer())
		{
			if(!OpenFile(m_currentfilename)) // attempt to load it
			{
				SaveFile(m_currentfilename); // if not create it with a blank template
				CF_Log.Info("[npcDefinition::Constructor] no file found, creating file");
				UpdateClient();
				CF_Log.Info("[npcDefinition::Constructor] updating client with loaded data.");
			}
			else
			{
				CF_Log.Info("[npcDefinition::Constructor] file found and loaded");
			}
			
		}
		Init();	
	}
	
	void ~npcDefinition() 
    {	
		auto trace = CF_Trace_0("npcDefinition", "~npcDefinition");
	}

	

	
	void Init()
	{
		auto trace = CF_Trace_0("npcDefinition", "Init");
		
		
		// initialize RPCs
		if(GetGame().IsDedicatedServer())
		{
		
			GetRPCManager().AddRPC( "ConfigTest", "XXX_SendCommandToServer", this, SingeplayerExecutionType.Both );
		}
		else
		{
			GetRPCManager().AddRPC( "ConfigTest", "XXX_SendDataToClient", this, SingeplayerExecutionType.Both );		
		}
		
		//initialize m_data if it is not already initialized
		if(!m_data)
		{


		}
			

	}

	void SetData(T data)
	{
		auto trace = CF_Trace_1("npcDefinition", "SetData").Add(data);
		m_data = data;
	}
	
	ref T GetData()
	{
		auto trace = CF_Trace_0("npcDefinition", "GetData");
		return m_data;
	}

	
	string GetFullBranchPath(string filename)
	{
		string filepath = Root + "\\" + filename + ".json";
		return filepath;
	
	}


	void Process(ref T savedata, string command, string filename)
	{
		auto trace = CF_Trace_3("npcDefinition", "Process").Add(savedata).Add(command).Add(filename);
		if(GetGame().IsDedicatedServer())
		{
	
			switch(command)
			{
			
				case "savefile" : 
					SaveFile(m_currentfilename); 
					break;
				case "newfile" :
					NewFile(filename, savedata);
					break;
				case "openfile" :
					OpenFile(filename);
					break;
				case "updateclient" :
					UpdateClient();	
					break;
				case "updateserver" :
					UpdateServer(savedata);	
					break;								
			}
	
		}
	
	}
	

	void CopyData(T savedata)
	{
	
			string temppath = "$profile:\\65426416temp.kpro";
			JsonFileLoader<T>.JsonSaveFile(temppath, savedata);
			JsonFileLoader<T>.JsonLoadFile(temppath, m_data);
			DeleteFile(temppath);
	}
	
	void DeleteFile(string filename)
	{
			array<ref CF_File> files();
			CF_Directory.GetFiles("$profile:\\65426416temp*", files);
			foreach (auto file : files)
			{
				file.Delete();
			}
	
	}
	
	
	void SaveFile(string filename = "", T savedata = null)  // only save to file name defined in constructor
	{
		auto trace = CF_Trace_1("npcDefinition", "SaveFile").Add(filename);
		ref T k_data;
		
		if(savedata==null)
		{
			k_data = m_data;
		}
		else
		{
			k_data = savedata;
		}
		
		if(filename== "")
		{
			filename = m_currentfilename;
		}
		
		if(GetGame().IsDedicatedServer())
		{
			if (FileExist(Root)==0) 
	        {
	            CF_Log.Warn(Root + "does not exist on server, creating directory");
	            MakeDirectory(Root);
	        }
			
			  					
			JsonFileLoader<T>.JsonSaveFile(GetFullBranchPath(filename), k_data);
			
		} 
		else 
		{		
			SendCommandToServer( k_data, "savefile", filename );
		}		
	}
	
	
	void NewFile(string filename, T newfileData)
	{
		auto trace = CF_Trace_1("npcDefinition", "NewFile").Add(filename);
		if(GetGame().IsDedicatedServer())
		{			
			SaveFile(filename, newfileData );  
		}
		else
		{
			SendCommandToServer( newfileData, "newfile", filename );
			
		}
	}

	 bool OpenFile(string filename){
		auto trace = CF_Trace_1("npcDefinition", "Open").Add(filename);
		
		if(GetGame().IsDedicatedServer())
		{
			
			if (FileExist(Root)==0) 
	        {
	            CF_Log.Warn(Root + "' does not exist on server, creating directory");
	            MakeDirectory(Root);
	        }
			
			if (FileExist(GetFullBranchPath(filename)))
			{ 
			    JsonFileLoader<T>.JsonLoadFile(GetFullBranchPath(filename), m_data);
				UpdateClient();	
				return true;
			} 
			
			return false;
	
						  
		}
		else
		{

			SendCommandToServer(null, "openfile", filename);
			return true;
		}
	}
	
	void SendCommandToServer(ref T sendData, string command, string commandparam)
	{
	
		GetRPCManager().SendRPC("ConfigTest", "XXX_SendCommandToServer", new Param3< ref T, string, string >( sendData, command, commandparam ), true, NULL);
	}
	
	// runs on the server
	void UpdateServer(T savedata = null)
	{
		auto trace = CF_Trace_1("npcDefinition", "UpdateServer").Add(savedata);
		if(GetGame().IsDedicatedServer())
		{
			CopyData(savedata);
			Print("[npcDefinition::UpdateServer()] data: ");
			SaveFile(m_currentfilename);
			
		} 
		else 
		{		
	
			GetRPCManager().SendRPC("ConfigTest", "XXX_SendCommandToServer", new Param3< ref T, string, string >( m_data, "updateserver", "" ), true, NULL);
		}	
					
	}
	
	// runs on the client
	void UpdateClient()
	{
		auto trace = CF_Trace_0("npcDefinition", "UpdateClient");
		if(GetGame().IsDedicatedServer())
		{
			GetRPCManager().SendRPC("ConfigTest", "XXX_SendDataToClient", new Param1< ref T >( m_data ), true, NULL);	

		}
		else
		{
			GetRPCManager().SendRPC("ConfigTest", "XXX_SendCommandToServer", new Param3< ref T, string, string >( null, "updateclient", "" ), true, NULL);
		}

		
	}
	
		// Executed on the server
	void XXX_SendCommandToServer( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target ) 
	{		
		if(GetGame().IsDedicatedServer())
		{
			auto trace = CF_Trace_4("npcDefinition", "XXX_SendCommandToServer").Add(type).Add(ctx).Add(sender).Add(target);
			Param3< ref T, string, string > data;
			if ( !ctx.Read( data ) ) return;
			CF_Log.Info(string.Format("Received command<%1, %2> from client. Processing command on server",data.param2, data.param3));
			Process(data.param1, data.param2, data.param3);
		}
				
	
								
	}

	// Executed on the client
	void XXX_SendDataToClient( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target ) {
		if(!GetGame().IsDedicatedServer())
		{
			auto trace = CF_Trace_4("npcDefinition", "XXX_SendDataToClient").Add(type).Add(ctx).Add(sender).Add(target);;	
			Param1< ref T > data;
			if ( !ctx.Read( data ) ) return;
			CopyData(data.param1);
			//SetData(data.param1);
			CF_Log.Warn(string.Format("Data received by client."));
		}
	}
	
	

	

	
		
}





