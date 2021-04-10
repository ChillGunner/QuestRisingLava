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

DEFINE_IL2CPP_ARG_TYPE(Vector3, "UnityEngine", "Vector3");
DEFINE_IL2CPP_ARG_TYPE(Vector2, "UnityEngine", "Vector2");
DEFINE_IL2CPP_ARG_TYPE(Color, "UnityEngine", "Color");