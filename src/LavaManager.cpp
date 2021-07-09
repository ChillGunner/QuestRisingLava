#include "LavaManager.hpp"
#include "il2cpptypes.hpp"
#include "image.hpp"

#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/utils/typedefs.h"

#include "UnityEngine/Transform.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Time.hpp"
#include "UnityEngine/MeshRenderer.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/Shader.hpp"
#include "UnityEngine/PrimitiveType.hpp"
#include "UnityEngine/Color.hpp"
#include "UnityEngine/Texture.hpp"
#include "UnityEngine/Texture2D.hpp"

#include "GlobalNamespace/GorillaNetworkJoinTrigger.hpp"
#include "GlobalNamespace/GorillaTagManager.hpp"

#include "GorillaLocomotion/Player.hpp"

#include "Photon/Realtime/Room.hpp"

#include "ExitGames/Client/Photon/Hashtable.hpp"

#include "custom-types/shared/macros.hpp"

#include <string>

DEFINE_TYPE(RisingLava::LavaManager);

namespace RisingLava{

    UnityEngine::Transform* transform;
    double lastGameEnd = 0.0;
    double lastTag = 0.0;

    void LavaManager::Start(){
        transform = get_transform();
    }


    void LavaManager::Update(){
        auto tagManager = GlobalNamespace::GorillaTagManager::_get_instance();
        if(!tagManager) return;

        LavaUp();

        //Get timeInfectedGameEnded
        Photon::Realtime::Room* room = tagManager->currentRoom;
        ExitGames::Client::Photon::Hashtable* properties = room->get_CustomProperties();
        double time;
        il2cpp_utils::RunMethod(properties, "TryGetValue", il2cpp_utils::createcsstr("timeInfectedGameEnded"), time);

        //Reset lava at game end
        if(lastGameEnd != time){
            lastGameEnd = time;
            LavaReset();
        }

        //Reset lava when players below 0
        if(room->get_PlayerCount() < 4){
            LavaReset();
        }
    }

    void LavaManager::LavaUp(){
        UnityEngine::Vector3 position = transform->get_position();
        position.y = position.y + (0.1 * UnityEngine::Time::get_deltaTime());
        transform->set_position(position);
    }

    void LavaManager::LavaReset(){
        transform->set_position(UnityEngine::Vector3(-50,-3,-50));
    }

     
}