#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "obj_types.hpp"

#include <vector>
#include <string>


namespace obj
{
    // Some Code Modified, or Copied from
    // https://github.com/tinyobjloader/tinyobjloader/tree/release
    class ObjLoader
    {
        public:
            static bool loadObj3(const std::string& filepath, shape_t* shape, attrib_t* attrib, std::string* err, bool KeepOneIndexing = false);
            static bool loadObj4(const std::string& filepath, shape_t* shape, attrib_t* attrib, std::string* err, bool KeepOneIndexing = false);
        
        private:
            struct Buffer
            {
                std::string lineBuf;
                int bufferIndex;
                int bufferSize;

                float getNextFloat();
                std::string getNextString();
                int getNextInt();
                void skipSpaces();
                char peek(int at);
                char get();
            };

            static index_t parseIndex(Buffer& buffer);
    };
}