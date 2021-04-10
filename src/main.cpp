// ModInfo
#include "modloader/shared/modloader.hpp"
// Logger & logging
#include "beatsaber-hook/shared/utils/logging.hpp"

// runmethod & findmethod, as well as field values
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/utils/typedefs.h"

#include "custom-types/shared/register.hpp"

#include "LavaManager.hpp"
#include "GorillaMapTriggerBase.hpp"
#include "TagZone.hpp"
#include "il2cpptypes.hpp"

// our modinfo, used for making the logger
static ModInfo modInfo;

// some defines to shorten what we have to type for logging
#define INFO(value...) getLogger().info(value)
#define ERROR(value...) getLogger().error(value)

// making and getting a logger, this makes it so we can log to logcat in a standardized way
Logger& getLogger()
{
    static Logger* logger = new Logger(modInfo, LoggerOptions(false, true));
    return *logger;
}

typedef struct SendOptions {
    int DeliveryMode;
    bool Encrypt;
    uint8_t Channel;
} SendOptions;
DEFINE_IL2CPP_ARG_TYPE(SendOptions, "ExitGames.Client.Photon", "SendOptions");
DEFINE_IL2CPP_ARG_TYPE_GENERIC_CLASS(List, "System.Collections.Generic", "List`1");


Il2CppObject* lavatransform;

MAKE_HOOK_OFFSETLESS(Start, void, Il2CppObject* self){
    Start(self);


    auto lavaObj = *il2cpp_utils::RunMethod("UnityEngine", "GameObject", "CreatePrimitive", 3);
    *il2cpp_utils::RunMethod(lavaObj, "set_layer", 11);
    auto trigger = *il2cpp_utils::RunGenericMethod(lavaObj, "GetComponent", std::vector<Il2CppClass*>{il2cpp_utils::GetClassFromName("UnityEngine", "BoxCollider")});
    *il2cpp_utils::RunMethod(trigger, "set_isTrigger", true);

    CRASH_UNLESS(il2cpp_utils::RunGenericMethod<RisingLava::LavaManager*>(lavaObj, "AddComponent", std::vector<Il2CppClass*>{classof(RisingLava::LavaManager*)}));
    CRASH_UNLESS(il2cpp_utils::RunGenericMethod<MapLoader::TagZone*>(lavaObj, "AddComponent", std::vector<Il2CppClass*>{classof(MapLoader::TagZone*)}));

    lavatransform = *il2cpp_utils::RunMethod(lavaObj, "get_transform");
    *il2cpp_utils::RunMethod(lavatransform, "set_position", (Vector3){-50,-2,-60});
    *il2cpp_utils::RunMethod(lavatransform, "set_localScale", (Vector3){100,1,100});


    auto forest = *il2cpp_utils::RunMethod("UnityEngine", "GameObject", "Find", il2cpp_utils::createcsstr("Forest"));
    auto forestTransform = *il2cpp_utils::RunMethod(forest, "get_transform");
    *il2cpp_utils::RunMethod(lavatransform, "SetParent", forestTransform);
}

MAKE_HOOK_OFFSETLESS(TagPlayer, void, Il2CppObject* self, Il2CppObject* player, Il2CppObject* player2){
    TagPlayer(self, player, player2);

    *il2cpp_utils::RunMethod(lavatransform, "set_localScale", (Vector3){100,1,100});
}

MAKE_HOOK_OFFSETLESS(GorillaTagManager_ReportTag, void, Il2CppObject* self, Il2CppObject* taggedPlayer, Il2CppObject* taggingPlayer)
{
    getLogger().info("Player Tagged!");
    GorillaTagManager_ReportTag(self, taggedPlayer, taggingPlayer);
    Il2CppObject* photonView = *il2cpp_utils::RunMethod(self, "get_photonView");
    bool IsMine = *il2cpp_utils::RunMethod<bool>(photonView, "get_IsMine");

    if (IsMine && taggedPlayer == taggingPlayer)
    {
        static Il2CppClass* raiseEventOptionsKlass = il2cpp_utils::GetClassFromName("Photon.Realtime", "RaiseEventOptions");
        Il2CppObject* raiseEventOptions = *il2cpp_utils::New(raiseEventOptionsKlass);
        static Il2CppClass* webFlagsKlass = il2cpp_utils::GetClassFromName("Photon.Realtime", "WebFlags");
        Il2CppObject* flags = *il2cpp_utils::New(webFlagsKlass, (uint8_t) 1);
        il2cpp_utils::SetFieldValue(raiseEventOptions, "Flags", flags);

        bool isCurrentlyTag = *il2cpp_utils::GetFieldValue<bool>(self, "isCurrentlyTag");

        Il2CppString* taggingPlayerID = *il2cpp_utils::RunMethod<Il2CppString*>(taggingPlayer, "get_UserId");
        Il2CppString* taggedPlayerID = *il2cpp_utils::RunMethod<Il2CppString*>(taggedPlayer, "get_UserId");

        if (isCurrentlyTag)
        {
            Il2CppObject* currentIt = *il2cpp_utils::GetFieldValue(self, "currentIt");
            if (currentIt != taggedPlayer)
            {
                *il2cpp_utils::RunMethod(lavatransform, "set_localScale", (Vector3){100,1,100});
                il2cpp_utils::RunMethod(self, "ChangeCurrentIt", taggedPlayer);

                static Il2CppClass* HashtableKlass = il2cpp_utils::GetClassFromName("ExitGames.Client.Photon", "Hashtable");
                
                /*
                Il2CppObject* hashTable = *il2cpp_utils::New(HashtableKlass);
                static Il2CppString* lastTag = il2cpp_utils::createcsstr("lastTag", il2cpp_utils::StringType::Manual);
                double time = *il2cpp_utils::RunMethod<double>("Photon.Pun", "PhotonNetwork", "get_Time");
                il2cpp_utils::RunMethod(hashTable, "Add", lastTag, time);
                Il2CppObject* currentRoom = *il2cpp_utils::GetFieldValue(self, "currentRoom");
                
                Il2CppObject* expectedProperties = *il2cpp_utils::New(HashtableKlass);
                Il2CppObject* otherFlags = *il2cpp_utils::New(webFlagsKlass, (uint8_t)1);
                auto* SetCustomProperties = il2cpp_utils::FindMethodUnsafe(currentRoom, "SetCustomProperties", 3);]
                ::il2cpp_utils::RunMethodThrow<bool, false>(currentRoom, SetCustomProperties, hashTable, nullptr, nullptr);
                */

                double time = (double)*il2cpp_utils::RunMethod<float>("UnityEngine", "Time", "get_time");
                il2cpp_utils::SetFieldValue(self, "lastTag", time);

                Array<Il2CppObject*>* eventContent = reinterpret_cast<Array<Il2CppObject*>*>(il2cpp_functions::array_new(classof(Il2CppObject*), 2));

                eventContent->values[0] = (Il2CppObject*)taggingPlayerID;
                eventContent->values[1] = (Il2CppObject*)taggedPlayerID;
                SendOptions options = *il2cpp_utils::GetFieldValue<SendOptions>("ExitGames.Client.Photon", "SendOptions", "SendReliable");
                il2cpp_utils::RunMethod("Photon.Pun", "PhotonNetwork", "RaiseEvent", (uint8_t)1, (Il2CppObject*)eventContent, raiseEventOptions, options);
            }
            else getLogger().info("Player Was already it!");
        }
        else
        {
            List<Il2CppObject*>* currentInfected = *il2cpp_utils::GetFieldValue<List<Il2CppObject*>*>(self, "currentInfected");
            bool contains = false;
            
            for (int i = 0; i < currentInfected->size; i++)
            {
                if (currentInfected->items->values[i] == taggedPlayer) 
                {
                    contains = true;
                    break;
                }
            }

            if (!contains)
            {
                il2cpp_utils::RunMethod(self, "AddInfectedPlayer", taggedPlayer);

                Array<Il2CppObject*>* eventContent = reinterpret_cast<Array<Il2CppObject*>*>(il2cpp_functions::array_new(classof(Il2CppObject*), 3));
                eventContent->values[0] = (Il2CppObject*)taggingPlayerID;
                eventContent->values[1] = (Il2CppObject*)taggedPlayerID;
                eventContent->values[2] = reinterpret_cast<Il2CppObject*>(*il2cpp_utils::RunMethod<int>(currentInfected, "get_Count"));
                SendOptions options = *il2cpp_utils::GetFieldValue<SendOptions>("ExitGames.Client.Photon", "SendOptions", "SendReliable");
                il2cpp_utils::RunMethod("Photon.Pun", "PhotonNetwork", "RaiseEvent", 2, eventContent, raiseEventOptions, options);
            }
            else getLogger().info("Player Was already infected!");
        }
    }
    else
    {
        getLogger().error("IsMine: %d, TaggedPlayer: %p, TaggingPlayer: %p", IsMine, taggedPlayer, taggingPlayer);
    }
}

MAKE_HOOK_OFFSETLESS(PhotonNetworkController_OnJoinedRoom, void, Il2CppObject* self)
{
    PhotonNetworkController_OnJoinedRoom(self);

    *il2cpp_utils::RunMethod(lavatransform, "set_localScale", (Vector3){100,200,100});
}

// setup lets the modloader know the mod ID and version as defined in android.mk
extern "C" void setup(ModInfo& info)
{
    info.id = ID;
    info.version = VERSION;
    modInfo = info;
    INFO("Hellowo World from setup!");
}

// load needs to be declared like this so the modloader can call it
extern "C" void load()
{
    INFO("Hello World From Load!");
    // store a reference to the logger
    Logger& logger = getLogger();

    // best to call the init method
    il2cpp_functions::Init();
    INFO("Installing hooks...");

    // installing a hook follows the principle of logger, hookname, findmethod, where findmethod takes args namespace, class, method, argcouny
    INSTALL_HOOK_OFFSETLESS(logger, Start, il2cpp_utils::FindMethodUnsafe("GorillaLocomotion", "Player", "Awake", 0));
    INSTALL_HOOK_OFFSETLESS(logger, TagPlayer, il2cpp_utils::FindMethodUnsafe("", "Gorilla1v1TagManager", "ReportTag", 2));
    INSTALL_HOOK_OFFSETLESS(logger, GorillaTagManager_ReportTag, il2cpp_utils::FindMethodUnsafe("", "GorillaTagManager", "ReportTag", 2));
    INSTALL_HOOK_OFFSETLESS(logger, PhotonNetworkController_OnJoinedRoom, il2cpp_utils::FindMethodUnsafe("", "PhotonNetworkController", "OnJoinedRoom", 0));
    custom_types::Register::RegisterType<RisingLava::LavaManager>();
    custom_types::Register::RegisterType<MapLoader::GorillaMapTriggerBase>();
    custom_types::Register::RegisterType<MapLoader::TagZone>();

    INFO("Installed hooks!");
}