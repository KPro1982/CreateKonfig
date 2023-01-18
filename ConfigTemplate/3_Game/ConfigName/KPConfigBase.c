class KPConfigBase<Class T>
{
	// the root should reflect your mod root folder. This is tytically your mod name
	static string Root = "$profile:\\ModName\\";
	protected string m_currentfilename;
	ref T m_data;
	
	void KPConfigBase(string filename, ref T data)
	{
		auto trace = CF_Trace_2("KPConfigBase", "KPConfigBase").Add(filename).Add(data);
		CF_Log.Level = CF_LogLevel.TRACE;	
	

		SetData(data);
		
		
							
		m_currentfilename = filename;
		if(GetGame().IsDedicatedServer())
		{
			if(!OpenFile(m_currentfilename)) // attempt to load it
			{
				SaveFile(m_currentfilename); // if not create it with a blank template
				CF_Log.Info("[KPConfigBase::Constructor] no file found, creating file");
				UpdateClient();
				CF_Log.Info("[KPConfigBase::Constructor] updating client with loaded data.");
			}
			else
			{
				CF_Log.Info("[KPConfigBase::Constructor] file found and loaded");
			}
			
		}
		Init();	
	}
	
	void ~KPConfigBase() 
    {	
		auto trace = CF_Trace_0("KPConfigBase", "~KPConfigBase");
	}

	

	
	void Init()
	{
		auto trace = CF_Trace_0("KPConfigBase", "Init");
		
		
		// initialize RPCs
		if(GetGame().IsDedicatedServer())
		{
		
			GetRPCManager().AddRPC( "ModName", "XXX_SendCommandToServer", this, SingleplayerExecutionType.Both );
		}
		else
		{
			GetRPCManager().AddRPC( "ModName", "XXX_SendDataToClient", this, SingleplayerExecutionType.Both );		
		}
		
		//initialize m_data if it is not already initialized
		if(!m_data)
		{


		}
			

	}

	void SetData(T data)
	{
		auto trace = CF_Trace_1("KPConfigBase", "SetData").Add(data);
		m_data = data;
	}
	
	ref T GetData()
	{
		auto trace = CF_Trace_0("KPConfigBase", "GetData");
		return m_data;
	}

	
	string GetFullBranchPath(string filename)
	{
		string filepath = Root + "\\" + filename + ".json";
		return filepath;
	
	}


	void Process(ref T savedata, string command, string filename)
	{
		auto trace = CF_Trace_3("KPConfigBase", "Process").Add(savedata).Add(command).Add(filename);
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
		auto trace = CF_Trace_1("KPConfigBase", "SaveFile").Add(filename);
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
		auto trace = CF_Trace_1("KPConfigBase", "NewFile").Add(filename);
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
	
		GetRPCManager().SendRPC("ModName", "XXX_SendCommandToServer", new Param3< ref T, string, string >( sendData, command, commandparam ), true, NULL);
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
	
			GetRPCManager().SendRPC("ModName", "XXX_SendCommandToServer", new Param3< ref T, string, string >( m_data, "updateserver", "" ), true, NULL);
		}	
					
	}
	
	// runs on the client
	void UpdateClient()
	{
		auto trace = CF_Trace_0("KPConfigBase", "UpdateClient");
		if(GetGame().IsDedicatedServer())
		{
			GetRPCManager().SendRPC("ModName", "XXX_SendDataToClient", new Param1< ref T >( m_data ), true, NULL);	

		}
		else
		{
			GetRPCManager().SendRPC("ModName", "XXX_SendCommandToServer", new Param3< ref T, string, string >( null, "updateclient", "" ), true, NULL);
		}

		
	}
	
		// Executed on the server
	void XXX_SendCommandToServer( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target ) 
	{		
		if(GetGame().IsDedicatedServer())
		{
			auto trace = CF_Trace_4("KPConfigBase", "XXX_SendCommandToServer").Add(type).Add(ctx).Add(sender).Add(target);
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
			auto trace = CF_Trace_4("KPConfigBase", "XXX_SendDataToClient").Add(type).Add(ctx).Add(sender).Add(target);;	
			Param1< ref T > data;
			if ( !ctx.Read( data ) ) return;
			CopyData(data.param1);
			//SetData(data.param1);
			CF_Log.Warn(string.Format("Data received by client."));
		}
	}
	
	

	

	
		
}





