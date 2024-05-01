#include "app.hpp"

#include "obj_loader.hpp"
#include "obj_writer.hpp"
#include "obj_extruder.hpp"

#include <string>
#include <iostream>

namespace obj
{
    void App::run()
    {
        std::string filepathSrc = "cube4DFace.obj4";
        std::string filepathDst = "cube4D.obj4";
        std::string err;

        if(!obj::ObjExtruder::extrudeObj4Face(filepathSrc, filepathDst, ObjExtruder::PRISM, &err))
        {
            std::cout << err << std::endl;
        }
    }
}