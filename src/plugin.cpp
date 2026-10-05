namespace {
    void OnMessage(SKSE::MessagingInterface::Message* a_message) {
        if (a_message->type == SKSE::MessagingInterface::kDataLoaded) {
        }
    }
}

#ifdef SKYRIM_SUPPORT_AE
constexpr SKSE::PluginVersionData Describe() {
    SKSE::PluginVersionData v;
    v.PluginVersion(REL::Version{Version::MAJOR, Version::MINOR, Version::PATCH});
    v.PluginName(Version::PROJECT);
    v.UsesAddressLibrary();
    v.UsesUpdatedStructs();
    v.CompatibleVersions({SKSE::RUNTIME_SSE_LATEST});

    if (SKSE::RUNTIME_SSE_LATEST < REL::Version{1, 7, 99, 0}) {
        v.MinimumRequiredXSEVersion(REL::Version{2, 2, 5});
    } else {
        v.MinimumRequiredXSEVersion(REL::Version{2, 3, 0});
    }

    return v;
}

SKSE_PLUGIN_VERSION = Describe();
#else
SKSE_PLUGIN_QUERY(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info) {
    a_info->infoVersion = SKSE::PluginInfo::kVersion;
    a_info->name = Version::PROJECT.data();
    a_info->version = Version::MAJOR;
    return !a_skse->IsEditor() && a_skse->RuntimeVersion() >= SKSE::RUNTIME_SSE_1_5_39;
}
#endif

SKSE_PLUGIN_LOAD(const SKSE::LoadInterface* a_skse) {
    SKSE::Init(a_skse);
    SKSE::GetMessagingInterface()->RegisterListener(OnMessage);
    return true;
}
