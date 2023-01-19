<#	
	.NOTES
	===========================================================================
	 Created with: 	SAPIEN Technologies, Inc., PowerShell Studio 2022 v5.8.213
	 Created on:   	12/11/2022 11:10 AM
	 Created by:   	Kpro1982
	 Organization: 	
	 Filename:     	ReplacementTools.ps1
	===========================================================================
	.DESCRIPTION
		A description of the file.
#>



function Rename-Folders
{
	[CmdletBinding()]
	param
	(
		[string]$Root,
		[string]$Source,
		[string]$Replace,
		[string]$ExcludeFolders

	)
	
	$folders = Get-ChildItem -Path $Root -Recurse -Directory
	# $TargetScriptPath = "$rootfolder\$Name\$TargetScripts"
	# $templateScripts = "$rootfolder\$Name\TemplateMod_Scripts"
	$excludedfolders = "*.git*"
	
	# Go through all the folders
	foreach ($item in $folders)
	{
#		$Parent = Split-Path $item.FullName
#		$Child = Split-Path -Path $item.FullName -leaf
		
		
		if ($item.FullName -NotLike $ExcludeFolders)
		{
			if ($item.fullname.Contains($Source))
			{
				$newname = $item.fullname
				$newname = $newname.Replace($Source, $Replace)
				Rename-Item $item.FullName -NewName $newname
				
			}
			
		}
		
		
		
	}
}


function Edit-TemplateTokens
{
	[CmdletBinding()]
	param
	(
		[Parameter(Mandatory = $true)]
		[string]$Source,
		[Parameter(Mandatory = $true)]
		[AllowEmptyString()]
		[AllowNull()]
		[string]$Replace,
		[Parameter(Mandatory = $true)]
		[string]$Folder,
		[string]$File,
		[string]$ExcludeFolders,
		[switch]$Recurse
	)
	
	if ($Recurse)
	{
		
		$targetfiles = Get-ChildItem $Folder *.* -File -rec
		
		foreach ($tfile in $targetfiles)
		{
			if ($tfile.FullName -NotLike $ExcludeFolders)
			{
				(Get-Content $tfile.PSPath) |
				Foreach-Object { $_ -replace $Source, $Replace } |
				Set-Content $tfile.PSPath
			}
		}
		
	}
	else # just the one file
	{
		
		(Get-Content -Path "$Folder\$File") |
		Foreach-Object { $_ -replace $Source, $Replace } |
		Set-Content -Path "$Folder\$File"
		
	}
}

function Edit-TemplateByRegion
{
	[CmdletBinding()]
	param
	(
		[string]$RegionStart,
		[string]$RegionEnd,
		[string]$Folder,
		[string]$File,
		[string]$ExcludeFolders,
		[switch]$Recurse
	)
	
	
	

	if ($Recurse)
	{
		
		$targetfiles = Get-ChildItem $Folder *.* -File -rec
		
		foreach ($file in $targetfiles)
		{
			if ($file.FullName -NotLike $ExcludeFolders)
			{
				
				Edit-TemplateFileByRegion -File $file -RegionStart $RegionStart -RegionEnd $RegionEnd
			}
		}
	}
	else # just the one file
	{
		$filepath = Add-Folder -Source $Folder -Folder $File
		Edit-TemplateFileByRegion -File $filepath -RegionStart $RegionStart -RegionEnd $RegionEnd		
	}
	
	
}

function Edit-TemplateFileByRegion
{
	[CmdletBinding()]
	param
	(
		[string]$RegionStart,
		[string]$RegionEnd,
		[string]$FilePath
	)
	
	$i = 0;
	$ii = 0;
	$starti = 0;
	$endi = 0;
	

	$arraylist = [System.Collections.ArrayList](Get-Content -Path $FilePath)
	
	foreach ($codeline in $arraylist) 
	{
		if ($codeline.Contains($RegionStart))
		{
			$starti = $i
		}
		if ($codeline.Contains($RegionEnd))
		{
			$endi = $i
			if (-not $starti)
			{
				
				return # no start so its a parsing error
			}
			if ($starti -gt $endi)
			{
				return # start after end is a parsing error
				
			}
			
			# remove all lines between start and end
			for ($ii = $endi; $ii -ge $starti; $ii--)
			{
				$todelete += @($ii)
			}
		}
		$i += 1
		
	}
	
	foreach ($di in $todelete)
	{
		$arraylist.RemoveAt($di)
	}
	
	
	$arraylist | Set-Content -Path $FilePath
}


function Edit-ScriptFiles
{
	[CmdletBinding()]
	param
	(
		
	)
	

	$modfolder = Read-GlobalParam -key "ModFolder"
	$modname = Read-FinalPathName -Source $modfolder
	
	$classname = Read-GlobalParam -key "ClassName"
	$instancename = Read-GlobalParam -key "InstanceName"
	
	$tempfolder = Get-ScriptDirectory
	$tempfolder = Add-Folder -Source $tempfolder -Folder "Temp"
	
	$targetfiles = Get-ChildItem $tempfolder  *.* -File -rec
	
	foreach ($file in $targetfiles)
	{
		if ($file.Extension -eq ".c")
		{
			# KPConfigBase
			Edit-TemplateTokens -Source "KPConfigBase" -Replace $classname -File $file -Folder $tempfolder -Recurse
			
			
			# instancename
			Edit-TemplateTokens -Source "instancename" -Replace $instancename -File $file -Folder $tempfolder -Recurse
			
			
			# modfolder
			Edit-TemplateTokens -Source "ModName" -Replace $modname -File $file -Folder $tempfolder -Recurse
			
			# XXX
			Edit-TemplateTokens -Source "XXX" -Replace "_$classname" -File $file -Folder $tempfolder -Recurse
		}
		
	}
}


function Create-TempFolder
{
	[CmdletBinding()]
	param
	(
		
	)
	
	$templatefolder = Get-ScriptDirectory
	$templatefolder = Add-Folder -Source $templatefolder -Folder "ConfigTemplate"
	$targetfolder = Read-GlobalParam -key "ModFolder"
	$target_3 = Add-Folder -Source $targetfolder -Folder "3_Game"
	$target_5 = Add-Folder -Source $targetfolder -Folder "5_Mission"
	
	# Delete Temp if exists
	$tempfolder = Get-ScriptDirectory
	$tempfolder = Add-Folder -Source $tempfolder -Folder "Temp"
	
	if (Test-Path -Path $tempfolder)
	{
		Remove-Item -Path $tempfolder -Force -Recurse
		
	}
	
	# Create Temp
	$modfolder = Read-GlobalParam -key "ModFolder"
	$modname = Read-FinalPathName -Source $modfolder
	
	New-Item -Path $tempfolder -ItemType "Directory"
	Copy-Item -Path $templatefolder\* -Destination $tempfolder -Include "*" -Container -Recurse
	
	# Rename Folders in Temp
	$classname = Read-GlobalParam -key "ClassName"
	Rename-Folders -Root $tempfolder -Source "ConfigName" -Replace $classname
	
	# Rename Files	
	
	$configbasepath = Add-Folder -source $tempfolder -Folder "3_Game\$classname\KPConfigBase.c"
	Rename-Item -Path $configbasepath -NewName "$classname.c"

	
}
function Copy-TempFiles
{
	
	[CmdletBinding()]
	param
	(
		
	)
	
	$classname = Read-GlobalParam -key "ClassName"
	
	$modfolder = Read-GlobalParam -key "ModFolder"
	$modname = Read-FinalPathName -Source $modfolder
	
	$targetfolder = Read-GlobalParam -key "ModFolder"

	$target_3 = Add-Folder -Source $targetfolder -Folder "Scripts\3_Game\"
	$target_5 = Add-Folder -Source $targetfolder -Folder "Scripts\5_Mission\"
	
	# Delete Temp if exists
	$tempfolder = Get-ScriptDirectory
	$tempfolder = Add-Folder -Source $tempfolder -Folder "Temp"
	$temp_3 = Add-Folder -Source $tempfolder -Folder "3_Game\"
	$temp_5 = Add-Folder -Source $tempfolder -Folder "5_Mission\"
	
	
	Copy-Item -Path $temp_3\* -Destination $target_3 -Include "*" -Container -Recurse
	Copy-Item -Path $temp_5\* -Destination $target_5 -Include "*" -Container -Recurse
	
}