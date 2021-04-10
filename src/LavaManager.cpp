#include "LavaManager.hpp"
#include "il2cpptypes.hpp"
#include "image.hpp"

#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/utils/typedefs.h"

#include "custom-types/shared/macros.hpp"

#include <string>

DEFINE_TYPE(RisingLava::LavaManager);

namespace RisingLava{

    Il2CppObject* transform;
    Il2CppObject* tagManager;
    Il2CppObject* itManager;
    Il2CppObject* room;
    Il2CppObject* renderer;
    double lastGameEnd = 0.0;
    double lastTag = 0.0;

    void LavaManager::Start(){
        Array<uint8_t>* byteArray = CRASH_UNLESS(il2cpp_utils::RunMethod<Array<uint8_t>*>("System", "Convert", "FromBase64String", il2cpp_utils::createcsstr(images::lava)));
    
        Il2CppObject* texture = CRASH_UNLESS(il2cpp_utils::New("UnityEngine", "Texture2D", 0, 0, 3, false));

        using LoadImage = function_ptr_t<unsigned int, Il2CppObject*, Array<uint8_t>*, bool>;
        static LoadImage loadImage = reinterpret_cast<LoadImage>(il2cpp_functions::resolve_icall("UnityEngine.ImageConversion::LoadImage"));
        loadImage(texture, byteArray, false);

        Il2CppObject* shader = *il2cpp_utils::RunMethod("UnityEngine", "Shader", "Find", il2cpp_utils::createcsstr("Standard"));
        Il2CppObject* material = *il2cpp_utils::New("UnityEngine", "Material", shader);
        *il2cpp_utils::RunMethod(material, "SetTexture", il2cpp_utils::createcsstr("_MainTex"), texture);

        auto gameObject = *il2cpp_utils::RunMethod(this, "get_gameObject");
        renderer = *il2cpp_utils::RunGenericMethod(gameObject, "GetComponent", std::vector<Il2CppClass*>{il2cpp_utils::GetClassFromName("UnityEngine", "MeshRenderer")});
        *il2cpp_utils::RunMethod(renderer, "SetMaterial", material);

        transform = *il2cpp_utils::RunMethod(gameObject, "get_transform");
    }

    void LavaManager::Update(){
        LavaUp();

        auto triggerObj = CRASH_UNLESS(il2cpp_utils::RunMethod("UnityEngine", "GameObject", "Find", il2cpp_utils::createcsstr("JoinPublicRoom (tree exit)")));
        auto trigger = CRASH_UNLESS(il2cpp_utils::RunGenericMethod(triggerObj, "GetComponent", std::vector<Il2CppClass*>{il2cpp_utils::GetClassFromName("", "GorillaNetworkJoinTrigger")}));
        CRASH_UNLESS(il2cpp_utils::SetFieldValue(trigger, "gameModeName", il2cpp_utils::createcsstr("RISINGLAVA")));
        auto triggerObj3 = CRASH_UNLESS(il2cpp_utils::RunMethod("UnityEngine", "GameObject", "Find", il2cpp_utils::createcsstr("JoinPublicRoom (end tutorial)")));
        auto trigger3 = CRASH_UNLESS(il2cpp_utils::RunGenericMethod(triggerObj3, "GetComponent", std::vector<Il2CppClass*>{il2cpp_utils::GetClassFromName("", "GorillaNetworkJoinTrigger")}));
        CRASH_UNLESS(il2cpp_utils::SetFieldValue(trigger3, "gameModeName", il2cpp_utils::createcsstr("RISINGLAVA")));

        tagManager = *il2cpp_utils::GetFieldValue("", "GorillaTagManager", "instance");
        if(!tagManager) return;
        Il2CppObject* room = *il2cpp_utils::GetFieldValue(tagManager, "currentRoom");
        Il2CppObject* properties = *il2cpp_utils::RunMethod(room, "get_CustomProperties");
        double time;
        *il2cpp_utils::RunMethod(properties, "TryGetValue", il2cpp_utils::createcsstr("timeInfectedGameEnded"), time);


        il2cpp_utils::getLogger().info(std::to_string(lastGameEnd) + " " + std::to_string(time));
        if(lastGameEnd != time){
            lastGameEnd = time;
            LavaReset();
        }

        if(*il2cpp_utils::RunMethod<int>(room, "get_PlayerCount") < 4){
            LavaReset();
        }
    }

    void LavaManager::LavaUp(){
        Vector3 scale = *il2cpp_utils::RunMethod<Vector3>(transform, "get_localScale");
        scale.y = scale.y + (0.2 * *il2cpp_utils::RunMethod<float>("UnityEngine", "Time", "get_deltaTime"));
        *il2cpp_utils::RunMethod(transform, "set_localScale", scale);
    }

    void LavaManager::LavaReset(){
        *il2cpp_utils::RunMethod(transform, "set_localScale", (Vector3){100, 0, 100});
    }

    void LavaManager::OnTriggerEnter(Il2CppObject* col){
        using SetFogMode = function_ptr_t<unsigned int, int>;
        static SetFogMode setFogMode = reinterpret_cast<SetFogMode>(il2cpp_functions::resolve_icall("UnityEngine.RenderSettings::set_fogMode"));
        setFogMode(1);
        il2cpp_utils::getLogger().info("Fog mode set");

        using SetFogColor = function_ptr_t<unsigned int, Color&>;
        il2cpp_utils::getLogger().info("Fog color function");
        static SetFogColor setFogColor = reinterpret_cast<SetFogColor>(il2cpp_functions::resolve_icall("UnityEngine.RenderSettings::set_fogColor_Injected"));
        il2cpp_utils::getLogger().info("Fog color icall");
        Color color = *il2cpp_utils::RunMethod<Color>("UnityEngine", "Color", "get_red");
        setFogColor(color);
        il2cpp_utils::getLogger().info("Fog color set");

        using SetFogDensity = function_ptr_t<unsigned int, float>;
        static SetFogDensity setFogDensity = reinterpret_cast<SetFogDensity>(il2cpp_functions::resolve_icall("UnityEngine.RenderSettings::set_fogDensity"));
        setFogDensity(0.1f);
        il2cpp_utils::getLogger().info("Fog density set");

        using SetAmbient = function_ptr_t<unsigned int, Color&>;
        static SetAmbient setAmbient = reinterpret_cast<SetAmbient>(il2cpp_functions::resolve_icall("UnityEngine.RenderSettings::set_ambientLight_Injected"));
        setAmbient(color);
        il2cpp_utils::getLogger().info("Ambient set");

        using SetIntensity = function_ptr_t<unsigned int, float>;
        static SetIntensity setIntensity = reinterpret_cast<SetIntensity>(il2cpp_functions::resolve_icall("UnityEngine.RenderSettings::set_ambientIntensity"));
        setIntensity(0.2f);
        il2cpp_utils::getLogger().info("Intensity set");
    }

    void LavaManager::OnTriggerExit(Il2CppObject* col){
        using SetIntensity = function_ptr_t<unsigned int, float>;
        static SetIntensity setIntensity = reinterpret_cast<SetIntensity>(il2cpp_functions::resolve_icall("UnityEngine.RenderSettings::set_ambientIntensity"));
        setIntensity(0);
        il2cpp_utils::getLogger().info("Intensity set");
    }

}