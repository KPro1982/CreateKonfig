ref npcDefinition<ref TestData> npcInstance;

// Missionbase runs early on both server and client and so is a good place to instantiate the config objects
modded class MissionBase
{
	override void OnMissionLoaded()
	{
		super.OnMissionLoaded();
		Print("[Missionbase]:: OnMissionLoaded(): Initializing KPConfig.");
		
		// consider whether this should only run on the client and only after the player is created.
		npcInstance = new npcDefinition<ref TestData>("TestData", new TestData());  

	}
	
}