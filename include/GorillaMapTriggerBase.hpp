#pragma once

#include "custom-types/shared/macros.hpp"

namespace MapLoader {
    enum class TouchType {
        Any,
        Head,
        Hands
    };
}

DECLARE_CLASS(MapLoader, GorillaMapTriggerBase, "UnityEngine", "MonoBehaviour", sizeof(Il2CppObject) + sizeof(void*) + sizeof(MapLoader::TouchType), 

        DECLARE_METHOD(void, OnTriggerEnter, Il2CppObject* collider);
        DECLARE_METHOD(void, OnTriggerStay, Il2CppObject* collider);
        DECLARE_METHOD(void, OnTriggerExit, Il2CppObject* collider);
        DECLARE_METHOD(void, RunTrigger, Il2CppObject* collider);
        DECLARE_METHOD(bool, CheckColliderComponents, Il2CppObject* collider);
        DECLARE_INSTANCE_FIELD(float, Delay);
        DECLARE_INSTANCE_FIELD(float, touchedTime);
        DECLARE_INSTANCE_FIELD(bool, isTriggering);
        DECLARE_INSTANCE_FIELD(Il2CppObject*, collider);

    public:
        TouchType touchType = TouchType::Any;

    REGISTER_FUNCTION(GorillaMapTriggerBase,
        REGISTER_METHOD(OnTriggerEnter);
        REGISTER_METHOD(OnTriggerStay);
        REGISTER_METHOD(OnTriggerExit);
        REGISTER_METHOD(RunTrigger);
        REGISTER_METHOD(CheckColliderComponents);
        REGISTER_FIELD(Delay);
        REGISTER_FIELD(touchedTime);
        REGISTER_FIELD(isTriggering);
        REGISTER_FIELD(collider);
    )
)