#include "GorillaMapTriggerBase.hpp"
#include "custom-types/shared/macros.hpp"

DEFINE_TYPE(MapLoader::GorillaMapTriggerBase);

extern Logger& getLogger();

namespace MapLoader
{
    void GorillaMapTriggerBase::OnTriggerEnter(Il2CppObject* collider)
    {
        if(isTriggering) return;
        if (!CheckColliderComponents(collider)) return;

        isTriggering = true;
        this->collider = collider;

        if (Delay == 0.0f)
        {
            RunTrigger(collider);
        }
        else touchedTime = 0.0f;
    }

    void GorillaMapTriggerBase::OnTriggerStay(Il2CppObject* collider)
    {
        if (Delay == 0.0f) return;
        if (!CheckColliderComponents(collider)) return;

        float fixedDeltaTime = *il2cpp_utils::RunMethod<float>("UnityEngine", "Time", "get_fixedDeltaTime");
        touchedTime += fixedDeltaTime;

        if (touchedTime >= Delay)
        {
            touchedTime = 0.0f;
            RunTrigger(collider);
        }
    }

    void GorillaMapTriggerBase::OnTriggerExit(Il2CppObject* collider)
    {
        if (!CheckColliderComponents(collider)) return;
        if (this->collider != nullptr && this->collider != collider) return;

        isTriggering = false;
        collider = nullptr;
    }

    bool GorillaMapTriggerBase::CheckColliderComponents(Il2CppObject* collider)
    {
        static std::vector<Il2CppClass*> gorillaTriggerColliderHandIndicatorKlass = {il2cpp_utils::GetClassFromName("", "GorillaTriggerColliderHandIndicator")};
        static std::vector<Il2CppClass*> playerKlass = {il2cpp_utils::GetClassFromName("GorillaLocomotion", "Player")};

        Il2CppObject* colliderGO = *il2cpp_utils::RunMethod(collider, "get_gameObject");
        Il2CppObject* handIndicator = *il2cpp_utils::RunGenericMethod(colliderGO, "GetComponentInParent", gorillaTriggerColliderHandIndicatorKlass);
        Il2CppObject* player = *il2cpp_utils::RunGenericMethod(colliderGO, "GetComponentInParent", playerKlass);

        switch (touchType)
        {
            case TouchType::Any:
                if (!handIndicator && !player)
                    return false;
                break;
            case TouchType::Hands:
                if (!handIndicator)
                    return false;
                break;
            case TouchType::Head:
                if (!player || (player && handIndicator))
                    return false;
                break;
            default:
                return false;
        }
        return true;
    }

    void GorillaMapTriggerBase::RunTrigger(Il2CppObject* collider)
    {
        auto* trigger = il2cpp_functions::class_get_method_from_name(il2cpp_utils::ExtractClass(this), "Trigger", 1);
        if (trigger)
            il2cpp_utils::RunMethod(this, trigger, collider);

        isTriggering = false;
        this->collider = nullptr;
    }
}