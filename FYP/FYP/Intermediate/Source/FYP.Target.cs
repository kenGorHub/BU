using UnrealBuildTool;

public class FYPTarget : TargetRules
{
	public FYPTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		ExtraModuleNames.Add("FYP");
	}
}
