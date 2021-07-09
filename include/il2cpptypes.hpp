#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
struct Vector3{
    float x;
    float y;
    float z;
};

struct Vector2{
    float x;
    float y;
};

struct Color{
    float r;
    float g;
    float b;
    float a;
};

struct InputDevice{
    uint64_t m_DeviceId;
    bool m_IsInitialized;
};

template<typename T>
struct InputFeatureUsage{
    Il2CppString* m_Name;
    int m_InternalType;
};

DEFINE_IL2CPP_ARG_TYPE(Vector3, "UnityEngine", "Vector3");
DEFINE_IL2CPP_ARG_TYPE(Vector2, "UnityEngine", "Vector2");
DEFINE_IL2CPP_ARG_TYPE(Color, "UnityEngine", "Color");
DEFINE_IL2CPP_ARG_TYPE(InputDevice, "UnityEngine.XR", "InputDevice");
DEFINE_IL2CPP_ARG_TYPE_GENERIC_STRUCT(InputFeatureUsage, "UnityEngine.XR", "InputFeatureUsage`1");