#pragma once

#include "obj_types.hpp"

#include <string>

namespace obj
{
    class ObjExtruder
    {
        private:
            static bool extrudeObj4PRISM(const std::string& filepathSrc, const std::string& filepathDst, shape_t* shape, attrib_t* attrib, shape_t* oShape, attrib_t* oAttrib, std::string* err);


        public:
            enum ExtrusionType
            {
                PRISM,
            };

            static bool extrudeObj4Face(const std::string& filepathSrc, const std::string& filepathDst, ExtrusionType extrusionType, std::string* err);
            static bool extrudeObj3(const std::string& filepathSrc, const std::string& filepathDst, ExtrusionType extrusionType, std::string* err);
            static bool Obj3ToObj4Face(shape_t& shape3, attrib_t& attrib3, shape_t& shape4, attrib_t& attrib4);
    };
}