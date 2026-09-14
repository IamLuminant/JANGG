// Copyright 2025, Luminant. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FLuminantExpansionModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
