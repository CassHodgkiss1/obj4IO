#pragma once

#include "obj_types.hpp"

#include <string>

namespace obj
{
    class ObjWriter
    {
        public:
            static bool saveObj3(const std::string& filepath, shape_t* shape, attrib_t* attrib, std::string* err);
            static bool saveObj4(const std::string& filepath, shape_t* shape, attrib_t* attrib, std::string* err);
    };
}