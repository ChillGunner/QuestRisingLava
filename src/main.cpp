// ModInfo
#include "modloader/shared/modloader.hpp"
// Logger & logging
#include "beatsaber-hook/shared/utils/logging.hpp"

// runmethod & findmethod, as well as field values
#include "quest-cosmetic-loader/shared/CosmeticLoader.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/utils/typedefs.h"

#include "custom-types/shared/register.hpp"
#include "monkecodegen/include/UnityEngine/AssetBundle.hpp"
#include "monkecodegen/include/UnityEngine/GameObject.hpp"
#include "monkecodegen/include/UnityEngine/Transform.hpp"
#include "monkecodegen/include/UnityEngine/Vector3.hpp"
#include "monkecodegen/include/GlobalNamespace/PhotonNetworkController.hpp"

#include "GorillaLocomotion/Player.hpp"

#include "GlobalNamespace/GorillaPlaySpace.hpp"
#include "GlobalNamespace/GorillaTagger.hpp"
#include "GlobalNamespace/VRRig.hpp"
#include "GlobalNamespace/VRMap.hpp"

#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/PlayerPrefs.hpp"

#include "monkecomputer/shared/GorillaUI.hpp"
#include "monkecomputer/shared/CustomQueues.hpp"

#include "LavaManager.hpp"
#include "GorillaMapTriggerBase.hpp"
#include "TagZone.hpp"
#include "il2cpptypes.hpp"

using namespace CosmeticsLoader;

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

bool canBeTagged = true;

UnityEngine::GameObject* lavaVision;
UnityEngine::GameObject* lava;
UnityEngine::Transform* lavaTransform;

UnityEngine::Vector3 forestStart = UnityEngine::Vector3(-50,-3,-50);
UnityEngine::Vector3 caveStart = UnityEngine::Vector3(-50,-20,-50);
UnityEngine::Vector3 canyonStart = UnityEngine::Vector3(-100, -7, -150);

RisingLava::LavaManager* lavaManager;


//Stolen from Map Loader
void TagLocalPlayer()
    {
        bool inRoom = *il2cpp_utils::RunMethod<bool>("Photon.Pun", "PhotonNetwork", "get_InRoom");
        if (inRoom)
        {
            Il2CppObject* gorillaTagManager = *il2cpp_utils::GetFieldValue("", "GorillaTagManager", "instance");
            static std::vector<Il2CppClass*> gameManagerKlass = {il2cpp_utils::GetClassFromName("", "GorillaGameManager")};
            Il2CppObject* gameManager = *il2cpp_utils::RunGenericMethod(gorillaTagManager, "GetComponent", gameManagerKlass);
            Il2CppObject* photonView = *il2cpp_utils::RunMethod("Photon.Pun", "PhotonView", "Get", gameManager);
            
            Il2CppObject* localPlayer = *il2cpp_utils::RunMethod("Photon.Pun", "PhotonNetwork", "get_LocalPlayer");

            Array<Il2CppObject*>* sendArr = reinterpret_cast<Array<Il2CppObject*>*>(il2cpp_functions::array_new(classof(Il2CppObject*), 1));
            sendArr->values[0] = localPlayer;
            static Il2CppString* reportTagRPC = il2cpp_utils::createcsstr("ReportTagRPC", il2cpp_utils::StringType::Manual);
            int target = 2;

            il2cpp_utils::RunMethod(photonView, "RPC", reportTagRPC, target, sendArr);
        }
    }

UnityEngine::GameObject* LoadLava(){

    std::string path = "/sdcard/ModData/com.AnotherAxiom.GorillaTag/Mods/RisingLava/LavaAsset";
    std::string path2 = "/sdcard/ModData/com.AnotherAxiom.GorillaTag/Mods/RisingLava/LavaVision";
    auto loader = new CosmeticLoader(path, [&](std::string name, Il2CppObject* lavaAsset){
                lava = (UnityEngine::GameObject*)lavaAsset;
            }, "_Hat", il2cpp_utils::GetSystemType("UnityEngine", "GameObject"));
    auto transform = lava->get_transform();
    transform->set_position(UnityEngine::Vector3(-50, 10, -50));

    auto visionLoader = new CosmeticLoader(path2, [&](std::string name, Il2CppObject* lavaAsset){
                lavaVision = (UnityEngine::GameObject*)lavaAsset;
            }, "_Hat", il2cpp_utils::GetSystemType("UnityEngine", "GameObject"));
    lavaVision->get_transform()->set_position(UnityEngine::Vector3(-50, 10, -50));

    return lava;
}


Il2CppObject* lavatransform;
Il2CppObject* cubeTransform;

bool lavaActive = false;

MAKE_HOOK_OFFSETLESS(Start, void, Il2CppObject* self){
    Start(self);
    
    LoadLava();
    lavaManager = lava->AddComponent<RisingLava::LavaManager*>();
    lavaTransform = lava->get_transform();
}


//Stolen from Map Loader
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

    Il2CppObject* currentRoom = CRASH_UNLESS(il2cpp_utils::RunMethod("Photon.Pun", "PhotonNetwork", "get_CurrentRoom"));
    Il2CppString* queue = UnityEngine::PlayerPrefs::GetString(il2cpp_utils::createcsstr("currentQueue", il2cpp_utils::StringType::Manual), il2cpp_utils::createcsstr("DEFAULT"));
    std::string queuecpp = to_utf8(csstrtostr(queue));

    if (currentRoom)
    {
        if (queuecpp.find("RISINGLAVA") != std::string::npos)
        {
            lavaActive = true;
        }
        else lavaActive = false;
    }
    else lavaActive = false;

    auto photonNetworkController = GlobalNamespace::PhotonNetworkController::_get_instance();
    auto gameType = to_utf8(csstrtostr(photonNetworkController->currentGameType));
}

void Tag(){
    if(!canBeTagged) return;

    TagLocalPlayer();
    canBeTagged = false;

    std::thread ReTag([&](){
            for (int i = 0; i < 1000; i++)
            {
                usleep(1000);
            }

            canBeTagged = true;
        });

        ReTag.detach();
}

MAKE_HOOK_OFFSETLESS(Update, void, GorillaLocomotion::Player* self){
    Update(self);

    auto rig = GlobalNamespace::GorillaTagger::get_Instance()->offlineVRRig;
    auto head = rig->head;
    auto target = head->rigTarget;

    if(lavaActive){
        lava->SetActive(true);
        UnityEngine::Vector3 plrPos = self->lastPosition;
        UnityEngine::Vector3 lavaPos = lavaTransform->get_position();

        if(lavaPos.y > target->get_position().y){
            Tag();
            lavaVision->SetActive(true);
            
            if(rig){
                lavaVision->get_transform()->SetParent(target);
                lavaVision->get_transform()->set_localPosition(UnityEngine::Vector3(0, 0, 0.3f));
                lavaVision->get_transform()->set_localRotation(UnityEngine::Quaternion::Euler(0, 90, 0));
            }
        }
        else{
            lavaVision->SetActive(false);
        }
    }
    else{
        lava->SetActive(false);
        lavaVision->SetActive(false);
    }
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
    GorillaUI::Init();
    INFO("Installing hooks...");

    // installing a hook follows the principle of logger, hookname, findmethod, where findmethod takes args namespace, class, method, argcouny
    INSTALL_HOOK_OFFSETLESS(logger, Start, il2cpp_utils::FindMethodUnsafe("GorillaLocomotion", "Player", "Awake", 0));
    INSTALL_HOOK_OFFSETLESS(logger, Update, il2cpp_utils::FindMethodUnsafe("GorillaLocomotion", "Player", "Update", 0));
    INSTALL_HOOK_OFFSETLESS(logger, GorillaTagManager_ReportTag, il2cpp_utils::FindMethodUnsafe("", "GorillaTagManager", "ReportTag", 2));
    INSTALL_HOOK_OFFSETLESS(logger, PhotonNetworkController_OnJoinedRoom, il2cpp_utils::FindMethodUnsafe("", "PhotonNetworkController", "OnJoinedRoom", 0));
    custom_types::Register::RegisterType<RisingLava::LavaManager>();
    custom_types::Register::RegisterType<MapLoader::GorillaMapTriggerBase>();
    custom_types::Register::RegisterType<MapLoader::TagZone>();
    GorillaUI::CustomQueues::add_queue("RISINGLAVA", "Rising Lava", "Lava is rising, touch it and you get caught.");

    INFO("Installed hooks!");
}