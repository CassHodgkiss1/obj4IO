#include "obj_loader.hpp"

#include <fstream>
#include <iostream>

#include <vector>
#include <string>
#include <string.h>

#include <bits/stdc++.h>

// Some Code Modified, or Copied from
// https://github.com/tinyobjloader/tinyobjloader/tree/release
namespace obj
{
    // http://stackoverflow.com/questions/6089231/getting-std-ifstream-to-handle-lf-cr-and-crlf
    static std::istream &safeGetline(std::istream &is, std::string &t) {
        t.clear();

        // The characters in the stream are read one-by-one using a std::streambuf.
        // That is faster than reading them one-by-one using the std::istream.
        // Code that uses streambuf this way must be guarded by a sentry object.
        // The sentry object performs various tasks,
        // such as thread synchronization and updating the stream state.

        std::istream::sentry se(is, true);
        std::streambuf *sb = is.rdbuf();

        if (se) {
          for (;;) {
            int c = sb->sbumpc();
            switch (c) {
              case '\n':
                return is;
              case '\r':
                if (sb->sgetc() == '\n') sb->sbumpc();
                return is;
              case EOF:
                // Also handle the case when the last line has no line ending
                if (t.empty()) is.setstate(std::ios::eofbit);
                return is;
              default:
                t += static_cast<char>(c);
            }
          }
        }

        return is;
    }

    bool isSpace(char x)
    {
        return (((x) == ' ') || ((x) == '\t'));
    }

    bool isEndLine(char x)
    {
        return (((x) == '\r') || ((x) == '\n') || ((x) == '\0'));
    }

    std::string ObjLoader::Buffer::getNextString()
    {
        std::stringstream ss;

        while(!isSpace(peek(0)) && !isEndLine(peek(0)))
        {
            ss << get();
        }

        return ss.str();
    }

    float ObjLoader::Buffer::getNextFloat()
    {
        auto string = getNextString();

        try
        {
            return std::stof(string);
        }
        catch(const std::exception& e)
        {
            std::cout << "Cannot parse: " << lineBuf << '\n';
            std::cout << "Cannot convert: " << string << " to float" << '\n';
            throw e;
        }
    }

    int ObjLoader::Buffer::getNextInt()
    {
        std::stringstream ss;

        while(isdigit(peek(0)))
        {
            ss << get();
        }

        try
        {
            return std::stoi(ss.str());
        }
        catch(const std::exception& e)
        {
            std::cout << "Cannot parse: " << lineBuf << std::endl;
            std::cout << "Cannot convert: " << ss.str() << " to int" << '\n';
            throw e;
        }
    }

    void ObjLoader::Buffer::skipSpaces()
    {
        while(isSpace(peek(0)))
        {
            bufferIndex++;
        }
    }

    char ObjLoader::Buffer::peek(int at)
    {
        return lineBuf[bufferIndex + at];
    }
    
    char ObjLoader::Buffer::get()
    {
        bufferIndex++;
        return lineBuf[bufferIndex - 1];
    }
     
    index_t ObjLoader::parseIndex(Buffer& buffer)
    {
        index_t index{};
        
        index.vertex_index = static_cast<uint32_t>(buffer.getNextInt());
        buffer.bufferIndex++;
        index.normal_index = static_cast<uint32_t>(buffer.getNextInt());
        buffer.bufferIndex++;
        index.texcoord_index = static_cast<uint32_t>(buffer.getNextInt());

        return index;
    }
        
    bool ObjLoader::loadObj3(const std::string& filepath, shape_t* shape, attrib_t* attrib, std::string* err, bool KeepOneIndexing)
    {
        shape->name.clear();
        attrib->vertices.clear();
        attrib->normals.clear();
        attrib->texcoords.clear();
        shape->mesh.indeces.clear();

        Buffer buffer{};

        std::ifstream fs(filepath);
        if (!fs.is_open())
        {
            if(err) *err = "Failed to open " + filepath;
            return false;
        }
        
        for (int lineNum = 0; fs.peek() != -1; lineNum++) 
        {
            safeGetline(fs, buffer.lineBuf);

            if (buffer.lineBuf.empty()) continue;

            buffer.bufferIndex = 0;
            buffer.bufferSize = buffer.lineBuf.size();

            buffer.skipSpaces();

            if(buffer.peek(0) == 'o' && isSpace(buffer.peek(1)))
            {
                buffer.bufferIndex += 2;
                std::stringstream ss;
                while(buffer.bufferIndex < buffer.bufferSize)
                {
                    if(!isEndLine(buffer.peek(0)))
                    {
                        ss << buffer.get();
                    }
                    else break;
                }
                shape->name = ss.str();
                continue;
            }

            if(buffer.peek(0) == 'v' && isSpace(buffer.peek(1)))
            {
                buffer.bufferIndex += 2;

                attrib->vertices.push_back(buffer.getNextFloat());
                buffer.skipSpaces();
                attrib->vertices.push_back(buffer.getNextFloat());
                buffer.skipSpaces();
                attrib->vertices.push_back(buffer.getNextFloat());
                continue;
            }

            if(buffer.peek(0) == 'v' && buffer.peek(1) == 't' && isSpace(buffer.peek(2)))
            {
                buffer.bufferIndex += 3;

                attrib->texcoords.push_back(buffer.getNextFloat());
                buffer.skipSpaces();
                attrib->texcoords.push_back(buffer.getNextFloat());
                continue;
            }

            if(buffer.peek(0) == 'v' && buffer.peek(1) == 'n' && isSpace(buffer.peek(2)))
            {
                buffer.bufferIndex += 3;

                attrib->normals.push_back(buffer.getNextFloat());
                buffer.skipSpaces();
                attrib->normals.push_back(buffer.getNextFloat());
                buffer.skipSpaces();
                attrib->normals.push_back(buffer.getNextFloat());
                continue;
            }

            if(buffer.peek(0) == 'f' && isSpace(buffer.peek(1)))
            {
                buffer.bufferIndex += 2;
                shape->mesh.indeces.push_back(parseIndex(buffer));
                buffer.skipSpaces();
                shape->mesh.indeces.push_back(parseIndex(buffer));
                buffer.skipSpaces();
                shape->mesh.indeces.push_back(parseIndex(buffer));
                continue;
            }
        }

        if(!KeepOneIndexing)
        {
            for(auto& i : shape->mesh.indeces)
            {
                i.vertex_index--;
                i.normal_index--;
                i.texcoord_index--;
            }
        }

        fs.close();

        return true;
    }

    bool ObjLoader::loadObj4(const std::string& filepath, shape_t* shape, attrib_t* attrib, std::string* err, bool KeepOneIndexing)
    {
        shape->name.clear();
        attrib->vertices.clear();
        attrib->normals.clear();
        attrib->texcoords.clear();
        shape->mesh.indeces.clear();

        Buffer buffer{};

        std::ifstream fs(filepath);
        if (!fs.is_open())
        {
            if(err) *err = "Failed to open " + filepath;
            return false;
        }
        
        for (int lineNum = 0; fs.peek() != -1; lineNum++) 
        {
            safeGetline(fs, buffer.lineBuf);

            if (buffer.lineBuf.empty()) continue;

            buffer.bufferIndex = 0;
            buffer.bufferSize = buffer.lineBuf.size();

            buffer.skipSpaces();

            if(buffer.peek(0) == 'o' && isSpace(buffer.peek(1)))
            {
                buffer.bufferIndex += 2;
                std::stringstream ss;
                while(buffer.bufferIndex < buffer.bufferSize)
                {
                    if(!isEndLine(buffer.peek(0)))
                    {
                        ss << buffer.get();
                    }
                    else break;
                }
                shape->name = ss.str();
                continue;
            }

            if(buffer.peek(0) == 'v' && isSpace(buffer.peek(1)))
            {
                buffer.bufferIndex += 2;

                attrib->vertices.push_back(buffer.getNextFloat());
                buffer.skipSpaces();
                attrib->vertices.push_back(buffer.getNextFloat());
                buffer.skipSpaces();
                attrib->vertices.push_back(buffer.getNextFloat());
                buffer.skipSpaces();
                attrib->vertices.push_back(buffer.getNextFloat());
                continue;
            }

            if(buffer.peek(0) == 'v' && buffer.peek(1) == 't' && isSpace(buffer.peek(2)))
            {
                buffer.bufferIndex += 3;

                attrib->texcoords.push_back(buffer.getNextFloat());
                buffer.skipSpaces();
                attrib->texcoords.push_back(buffer.getNextFloat());
                buffer.skipSpaces();
                attrib->texcoords.push_back(buffer.getNextFloat());
                continue;
            }

            if(buffer.peek(0) == 'v' && buffer.peek(1) == 'n' && isSpace(buffer.peek(2)))
            {
                buffer.bufferIndex += 3;

                attrib->normals.push_back(buffer.getNextFloat());
                buffer.skipSpaces();
                attrib->normals.push_back(buffer.getNextFloat());
                buffer.skipSpaces();
                attrib->normals.push_back(buffer.getNextFloat());
                buffer.skipSpaces();
                attrib->normals.push_back(buffer.getNextFloat());
                continue;
            }

            if(buffer.peek(0) == 'f' && isSpace(buffer.peek(1)))
            {
                buffer.bufferIndex += 2;
                shape->mesh.indeces.push_back(parseIndex(buffer));
                buffer.skipSpaces();
                shape->mesh.indeces.push_back(parseIndex(buffer));
                buffer.skipSpaces();
                shape->mesh.indeces.push_back(parseIndex(buffer));
                buffer.skipSpaces();
                shape->mesh.indeces.push_back(parseIndex(buffer));
                continue;
            }

        }

        if(!KeepOneIndexing)
        {
            for(auto& i : shape->mesh.indeces)
            {
                i.vertex_index--;
                i.normal_index--;
                i.texcoord_index--;
            }
        }

        fs.close();

        return true;
    }
}