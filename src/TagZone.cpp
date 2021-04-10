#include "TagZone.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/utils/utils.h"

#include "custom-types/shared/macros.hpp"


DEFINE_TYPE(MapLoader::TagZone);

namespace MapLoader
{
    void TagZone::Awake()
    {
        canBeTagged = true;
    }

    void TagZone::Trigger(Il2CppObject* collider)
    {
        if (!canBeTagged)
            return;

        canBeTagged = false;
        TagLocalPlayer();

        std::thread ReTag([&](){
            for (int i = 0; i < 1000; i++)
            {
                usleep(1000);
            }

            canBeTagged = true;
        });

        ReTag.detach();
    }

    void TagZone::TagLocalPlayer()
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
}