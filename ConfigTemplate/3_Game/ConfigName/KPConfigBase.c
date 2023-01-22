class KPConfigBase<Class T>
{
	// the root should reflect your mod root folder. This is tytically your mod name
	static string Root = "$profile:\\ModName\\";
	protected string m_currentfilename;
	ref T m_data;
	
	void KPConfigBase(string filename, T data)
	{
		auto trace = CF_Trace_2("KPConfigBase", "KPConfigBase").Add(filename).Add(data);
		CF_Log.Level = CF_LogLevel.TRACE;	
	

		SetData(data);
				
		// initialize RPCs
		if(GetGame().IsDedicatedServer())
		{
		
			GetRPCManager().AddRPC( "ModName", "XXX_ReceiveCommandOnServer", this, SingleplayerExecutionType.Both );
		}
		else
		{
			GetRPCManager().AddRPC( "ModName", "XXX_ReceiveDataOnClient", this, SingleplayerExecutionType.Both );		
		}
		
							
		m_currentfilename = filename;
		if(GetGame().IsDedicatedServer())
		{
			if(!OpenFile(m_currentfilename)) // attempt to load it
			{
				SaveFile(m_currentfilename); // if not create it with a blank template
				CF_Log.Info("[KPConfigBase::Constructor] no file found, creating file");
				CF_Log.Info("[KPConfigBase::Constructor] updating client with loaded data.");
			}
			else
			{
				CF_Log.Info("[KPConfigBase::Constructor] file found and loaded");
			}

		}
																									UpdateClient(); // this results in update requests one initiating on server and one on client.
		Init();	
	}
	
	void ~KPConfigBase() 
    {	
		auto trace = CF_Trace_0("KPConfigBase", "~KPConfigBase");
	}

	

	
	void Init()
	{
		auto trace = CF_Trace_0("KPConfigBase", "Init");
		

		
		//initialize m_data if it is not already initialized
		if(!m_data)
		{


		}
			

	}

	
	void SetFilename(string fn)
	{
		m_currentfilename = fn;
	}
	
	string GetFilename()
	{
		return m_currentfilename;
	}
	

	void SetData(T data)
	{
		auto trace = CF_Trace_1("KPConfigBase", "SetData").Add(data);
		m_data = data;
	}
	
	T GetData()
	{
		auto trace = CF_Trace_0("KPConfigBase", "GetData");
		return m_data;
	}

	
	string GetFullBranchPath(string filename)
	{
		string filepath = Root + "\\" + filename + ".json";
		return filepath;
	
	}


	void Process(T savedata, string command, string filename)
	{
		auto trace = CF_Trace_3("KPConfigBase", "Process").Add(savedata).Add(command).Add(filename);
		if(GetGame().IsDedicatedServer())
		{
	
			switch(command)
			{
			
				case "savefile" : 
					SaveFile(filename); 
					break;
				case "newfile" :
					NewFile(filename, savedata);
					break;
				case "savefileas" : 
					SaveFile(filename); 
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
		auto trace = CF_Trace_1("KPConfigBase", "SaveFile").Add(filename);
		
		if(savedata != null)
		{
			CopyData(savedata);
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
			
			  					
			JsonFileLoader<T>.JsonSaveFile(GetFullBranchPath(filename), m_data);
			
		} 
		else 
		{		
			SendCommandToServer( m_data, "savefile", filename );
		}		
	}
	void SaveFileAs(string filename, T newfileData)
	{
		auto trace = CF_Trace_1("npcDialogStoreBase", "NewFile").Add(filename);
		if(GetGame().IsDedicatedServer())
		{			
			SaveFile(filename, newfileData );  
		}
		else
		{
			SetFilename(filename);
			SendCommandToServer( newfileData, "savefileas", filename );
			
		}
	}
	
	void NewFile(string filename, T newfileData)
	{
		auto trace = CF_Trace_1("KPConfigBase", "NewFile").Add(filename);
		if(GetGame().IsDedicatedServer())
		{			
			SaveFile(filename, newfileData );  
		}
		else
		{
			CopyData(newfileData);  // Is this really needed
			SetFilename(filename);	// is this really needed		 			 
			SendCommandToServer( newfileData, "newfile", filename );
			
		}
	}

	 bool OpenFile(string filename){
		auto trace = CF_Trace_1("KPConfigBase", "Open").Add(filename);
		
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
				SendDataToClient(m_data);
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
	
	void SendCommandToServer(T sendData, string command, string commandparam)
	{
		auto trace = CF_Trace_3("KPConfigBase", "SendCommandToServer").Add(sendData).Add(command).Add(commandparam);	
		GetRPCManager().SendRPC("ModName", "XXX_ReceiveCommandOnServer", new Param3< T, string, string >( sendData, command, commandparam ), true, NULL);
	}
	
	void SendDataToClient(T sendData)
	{
	
		GetRPCManager().SendRPC("ModName", "XXX_ReceiveDataOnClient", new Param1< T>( sendData ), true, NULL);
	}
	
	// runs on the server
	void UpdateServer(T savedata = null)
	{
		auto trace = CF_Trace_1("KPConfigBase", "UpdateServer").Add(savedata);
		if(GetGame().IsDedicatedServer())
		{
			CopyData(savedata);
			Print("[KPConfigBase::UpdateServer()] data: ");
			SaveFile(m_currentfilename);
			
		} 
		else 
		{		
	
			SendCommandToServer( m_data, "updateserver", "" );
		}	
					
	}
	
	// runs on the client
	void UpdateClient()
	{
		auto trace = CF_Trace_0("KPConfigBase", "UpdateClient");
		if(GetGame().IsDedicatedServer())
		{	
			SendDataToClient( m_data );

		}
		else
		{
			SendCommandToServer( null, "updateclient", "" );
		}

		
	}
	
		// Executed on the server
	void XXX_ReceiveCommandOnServer( CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target ) 
	{		
		if(GetGame().IsDedicatedServer())
		{
			auto trace = CF_Trace_4("KPConfigBase", "XXX_ReceiveCommandOnServer").Add(type).Add(ctx).Add(sender).Add(target);
			Param3< T, string, string > data;
			if ( !ctx.Read( data ) ) return;
			T newdata;
			CastTo(newdata, data.param1);  // see if T was sent
			if(newdata)
			{
				CopyData(newdata);
			}
			CF_Log.Info(string.Format("Received command<%1, %2> from client. Processing command on server",data.param2, data.param3));
			Process(data.param1, data.param2, data.param3);
		}							
	}

	// Executed on the client
	void XXX_ReceiveDataOnClient( CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target ) {
		if(!GetGame().IsDedicatedServer())
		{
			auto trace = CF_Trace_4("KPConfigBase", "XXX_ReceiveDataOnClient").Add(type).Add(ctx).Add(sender).Add(target);;	
			Param1< ref T > data;
			if ( !ctx.Read( data ) ) return;
			CopyData(data.param1);								
			CF_Log.Warn(string.Format("Data received by client."));
		}
	}
	
	

	

	
		
}





