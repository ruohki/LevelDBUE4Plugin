// Some copyright should be here...

using UnrealBuildTool;
using System.IO;
using System;

public class LevelDBPlugin : ModuleRules
{
    private string ModulePath
    {   //Module Path: C:\StoneRage\ruohki_workstation\master\Engine\StoneRage\Plugins\LevelDBPlugin\Source\LevelDBPlugin
        get { return Path.GetDirectoryName(RulesCompiler.GetModuleFilename(this.GetType().Name)); }
    }

    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../ThirdParty/")); }
    }

    public LevelDBPlugin(TargetInfo Target)
	{
	    //TODO: Check if the absolute include path for leveldb is really needed	
		PublicIncludePaths.AddRange( new string[] { "LevelDBPlugin/Public", "LevelDBPlugin/ThirdParty/LevelDB/Include" });
		PrivateIncludePaths.AddRange( new string[] { "LevelDBPlugin/Private", });
        PublicDependencyModuleNames.AddRange( new string[] { "Core", });					
		PrivateDependencyModuleNames.AddRange( new string[] { "CoreUObject", "Engine", "Slate", "SlateCore", });

        System.Diagnostics.Debug.Print(String.Format("Module Path: {0}", ModulePath));

        LoadLibrary(Target, "LevelDB");

    }

    public bool LoadLibrary(TargetInfo Target, string LibraryName)
    {
        bool isLibrarySupported = false;

        if ((Target.Platform == UnrealTargetPlatform.Win64) || (Target.Platform == UnrealTargetPlatform.Win32))
        {
            isLibrarySupported = true;

            string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "x86";
            string LibrariesPath = Path.Combine(ThirdPartyPath, LibraryName, "Libraries");

            //PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, LibraryName + "." + PlatformString + ".lib"));
            System.Diagnostics.Debug.Print(String.Format("Searching for Libs at: {0}", LibrariesPath + "\\" + PlatformString));
            //System.Diagnostics.Debug.Print(String.Format("Found {0} Libraries", new DirectoryInfo(LibrariesPath + "\\" + PlatformString).GetFiles().Length));

            foreach (FileInfo File in new DirectoryInfo(LibrariesPath + "\\" + PlatformString).GetFiles())
            {
                //System.Diagnostics.Debug.Print(File.FullName);
                //System.Diagnostics.Debug.Print(String.Format("Adding Library: {0}", File.FullName));
                PublicAdditionalLibraries.Add(File.FullName);
            }
        }

        if (isLibrarySupported)
        {
            //System.Diagnostics.Debug.Print(Path.Combine(ThirdPartyPath, LibraryName, "Includes"));
            // Include path
            PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, LibraryName, "Includes"));
        }

        Definitions.Add(string.Format("WITH_" + this.GetType().Name + "_BINDING={0}", isLibrarySupported ? 1 : 0));

        return isLibrarySupported;
    }
}
