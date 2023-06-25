#include "SaveSystem.h"

DEFINE_LOG_CATEGORY(LogSaveSystem);

#define LOCTEXT_NAMESPACE "FSaveSystemModule"

void FSaveSystemModule::StartupModule()
{
}

void FSaveSystemModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FSaveSystemModule, SaveSystem)
