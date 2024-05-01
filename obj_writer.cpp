#include "obj_writer.hpp"

#include <fstream>
#include <iostream>

#include <string>

namespace obj
{
    bool ObjWriter::saveObj3(const std::string& filepath, shape_t* shape, attrib_t* attrib, std::string* err)
    {
        return false;
    }

    bool ObjWriter::saveObj4(const std::string& filepath, shape_t* shape, attrib_t* attrib, std::string* err)
    {
        std::cout << "saving obj4" << std::endl;
        std::ofstream ofs(filepath, std::ofstream::out | std::ofstream::trunc);

        ofs << "o " << shape->name << std::endl;

        if(attrib->vertices.size() % 4 != 0)
        {
            if(err) *err = "Vert Position data not a multiple of 4, cannot convert to x y z w";
            return false;
        }

        for(int i = 0; i < attrib->vertices.size(); i += 4)
        {
            ofs << "v "
                << attrib->vertices[i + 0] << ' '
                << attrib->vertices[i + 1] << ' '
                << attrib->vertices[i + 2] << ' '
                << attrib->vertices[i + 3] << std::endl;
        }

        if(attrib->normals.size() % 4 != 0)
        {
            if(err) *err = "Vert Normals data not a multiple of 4, cannot convert to x y z w";
            return false;
        }

        for(int i = 0; i < attrib->normals.size(); i += 4)
        {
            ofs << "vn "
                << attrib->normals[i + 0] << ' '
                << attrib->normals[i + 1] << ' '
                << attrib->normals[i + 2] << ' '
                << attrib->normals[i + 3] << std::endl;
        }

        if(attrib->texcoords.size() % 3 != 0)
        {
            if(err) *err = "Vert Texture Coords data not a multiple of 3, cannot convert to u v x";
            return false;
        }

        for(int i = 0; i < attrib->texcoords.size(); i += 3)
        {
            ofs << "vt "
                << attrib->texcoords[i + 0] << ' '
                << attrib->texcoords[i + 1] << ' '
                << attrib->texcoords[i + 2] << std::endl;
        }

        if(shape->mesh.indeces.size() % 4 != 0)
        {
            if(err) *err = "Shape Indeces Coords data not a multiple of 4, cannot convert to v/vn/vt";
            return false;
        }

        for(int i = 0; i < shape->mesh.indeces.size(); i+= 4)
        {
            ofs << "f "
                << shape->mesh.indeces[i + 0].vertex_index   << '/'
                << shape->mesh.indeces[i + 0].normal_index   << '/'
                << shape->mesh.indeces[i + 0].texcoord_index << ' '
                << shape->mesh.indeces[i + 1].vertex_index   << '/'
                << shape->mesh.indeces[i + 1].normal_index   << '/'
                << shape->mesh.indeces[i + 1].texcoord_index << ' '
                << shape->mesh.indeces[i + 2].vertex_index   << '/'
                << shape->mesh.indeces[i + 2].normal_index   << '/'
                << shape->mesh.indeces[i + 2].texcoord_index << ' '
                << shape->mesh.indeces[i + 3].vertex_index   << '/'
                << shape->mesh.indeces[i + 3].normal_index   << '/'
                << shape->mesh.indeces[i + 3].texcoord_index << std::endl;
        }

        ofs.close();

        return false;
    }
}