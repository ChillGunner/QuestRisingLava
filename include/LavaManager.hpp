#include "custom-types/shared/macros.hpp"

DECLARE_CLASS(RisingLava, LavaManager, "UnityEngine", "MonoBehaviour", sizeof(Il2CppObject) + sizeof(void*),
    DECLARE_METHOD(void, Start);
    DECLARE_METHOD(void, Update);
    DECLARE_METHOD(void, LavaUp);
    DECLARE_METHOD(void, LavaReset);
    DECLARE_METHOD(void, OnTriggerEnter, Il2CppObject* col);
    DECLARE_METHOD(void, OnTriggerExit, Il2CppObject* col);

    REGISTER_FUNCTION(LavaManager,
        REGISTER_METHOD(Start);
        REGISTER_METHOD(Update);
        REGISTER_METHOD(LavaUp);
        REGISTER_METHOD(LavaReset);
        REGISTER_METHOD(OnTriggerExit);
        REGISTER_METHOD(OnTriggerEnter);
    )
)