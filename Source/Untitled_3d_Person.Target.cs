// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class Untitled_3d_PersonTarget : TargetRules
{
	public Untitled_3d_PersonTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_1;
		ExtraModuleNames.Add("Untitled_3d_Person");
        ExtraModuleNames.Add("PlayerCharacter");
        ExtraModuleNames.Add("Struct");
        ExtraModuleNames.Add("Weapon");
    }
}
