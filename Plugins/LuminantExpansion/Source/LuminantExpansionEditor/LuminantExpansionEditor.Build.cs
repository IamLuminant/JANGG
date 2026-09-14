using UnrealBuildTool;

public class LuminantExpansionEditor : ModuleRules
{
    public LuminantExpansionEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                // ... other runtime dependencies your editor module needs ...
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Slate",
                "SlateCore",
                "UnrealEd", // Required for editor-only functionality
                "BlueprintGraph",
                "KismetCompiler",
                "LuminantExpansion", // Dependency on your plugin's runtime module
                // ... other private dependencies ...
            }
        );
    }
}
