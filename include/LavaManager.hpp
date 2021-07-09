#pragma once
#include "custom-types/shared/macros.hpp"
#include "UnityEngine/MonoBehaviour.hpp"

DECLARE_CLASS_CODEGEN(RisingLava, LavaManager, UnityEngine::MonoBehaviour,
    DECLARE_METHOD(void, Start);
    DECLARE_METHOD(void, Update);
    DECLARE_METHOD(void, LavaUp);
    DECLARE_METHOD(void, LavaReset);
    DECLARE_INSTANCE_FIELD_DEFAULT(UnityEngine::Vector3, startingPos, UnityEngine::Vector3(-50,-3,-50));

    REGISTER_FUNCTION(
        REGISTER_METHOD(Start);
        REGISTER_METHOD(Update);
        REGISTER_METHOD(LavaUp);
        REGISTER_METHOD(LavaReset);
        REGISTER_FIELD(startingPos);
    )
)