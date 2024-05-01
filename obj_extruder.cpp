#include "obj_extruder.hpp"
#include "obj_types.hpp"
#include "obj_loader.hpp"
#include "obj_writer.hpp"
#include "obj_utils.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <iostream>
#include <map>
#include <cassert>
#include <cstring>
#include <vector>

namespace std
{
	template<>
	struct hash<obj::vert4_t>
	{
		size_t operator()(obj::vert4_t const& vertex) const
		{
			size_t seed = 0;
			obj::hashCombine(seed, vertex.position, vertex.normal, vertex.uv);
			return seed;
		}
	};

    template<>
	struct hash<obj::face3_t>
	{
		size_t operator()(obj::face3_t const& face) const
		{
			size_t seed = 0;
			obj::hashCombine(seed, face.a, face.b, face.c);
			return seed;
		}
	};
}

namespace obj
{
	void addTriangle(uint32_t a, uint32_t b, uint32_t c, std::unordered_map<face3_t, bool>& faces)
	{
		face3_t face{a, b, c};

		if(faces.count(face) == 0) faces[face] = true;
		else faces[face] = false;
	}

    bool ObjExtruder::extrudeObj4Face(const std::string& filepathSrc, const std::string& filepathDst, ExtrusionType extrusionType, std::string* err)
    {
        shape_t shape{};
        attrib_t attrib{};

        if(!ObjLoader::loadObj4(filepathSrc, &shape, &attrib, err)) return false;

        shape_t oShape{};
        attrib_t oAttrib{};

        switch(extrusionType)
        {
            case PRISM: 
                if(!extrudeObj4PRISM(filepathSrc, filepathDst, &shape, &attrib, &oShape, &oAttrib, err)) return false;
                break;
        }

        if(!ObjWriter::saveObj4(filepathDst, &oShape, &oAttrib, err)) return false;
        
        return true;
    }

	bool ObjExtruder::extrudeObj3(const std::string& filepathSrc, const std::string& filepathDst, ExtrusionType extrusionType, std::string* err)
    {
        shape_t shape3{};
        attrib_t attrib3{};

        if(!ObjLoader::loadObj3(filepathSrc, &shape3, &attrib3, err)) return false;

		shape_t shape4{};
        attrib_t attrib4{};

        if(!Obj3ToObj4Face(shape3, attrib3, shape4, attrib4)) return false;

        shape_t oShape{};
        attrib_t oAttrib{};

        switch(extrusionType)
        {
            case PRISM: 
                if(!extrudeObj4PRISM(filepathSrc, filepathDst, &shape4, &attrib4, &oShape, &oAttrib, err)) return false;
                break;
        }

        if(!ObjWriter::saveObj4(filepathDst, &oShape, &oAttrib, err)) return false;
        
        return true;
    }

	bool ObjExtruder::Obj3ToObj4Face(shape_t& shape3, attrib_t& attrib3, shape_t& shape4, attrib_t& attrib4)
	{
		shape4.name = shape3.name;

		//TODO 

		return true;
	}

	void getData(std::vector<glm::vec4>& vertPositions, std::vector<index_t>& indices, shape_t* shape, attrib_t* attrib)
	{
		std::unordered_map<glm::vec4, uint32_t> uniquePositions;

		for(int i = 0; i < shape->mesh.indeces.size(); i++) 
		{
			glm::vec4 pos{};

			auto& index = shape->mesh.indeces[i];

			if(index.vertex_index >= 0)
			{
				pos = 
				{
					attrib->vertices[4 * index.vertex_index + 0],
					attrib->vertices[4 * index.vertex_index + 1],
					attrib->vertices[4 * index.vertex_index + 2],
					attrib->vertices[4 * index.vertex_index + 3],
				};

			}

			if(uniquePositions.count(pos) == 0)
			{
				uniquePositions[pos] = static_cast<uint32_t>(vertPositions.size());
				vertPositions.push_back(pos);
			}

			indices.push_back(
				index_t{
					uniquePositions[pos] + 1,
					1,
					1
				}
			);
		}
	}

	void addTetra(u_int32_t iA, u_int32_t iB, u_int32_t iC, u_int32_t iD, glm::vec4& a, glm::vec4& b, glm::vec4& c, glm::vec4& d, std::vector<glm::vec4>& vertPositionsSide, std::vector<index_t>& indicesSide)
	{
		vertPositionsSide.push_back(a);
		vertPositionsSide.push_back(b);
		vertPositionsSide.push_back(c);
		vertPositionsSide.push_back(d);

		indicesSide.push_back({iA, 1, 1});
		indicesSide.push_back({iB, 1, 1});
		indicesSide.push_back({iC, 1, 1});
		indicesSide.push_back({iD, 1, 1});
	}

	void getOpenFaces(std::vector<face3_t>& openFaces, std::vector<glm::vec4>& vertPositions, std::vector<index_t>& indices)
	{
        std::unordered_map<face3_t, bool> faces;

		for(int i = 0; i < indices.size(); i += 4)
		{
			auto a = indices[i + 0].vertex_index;
			auto b = indices[i + 1].vertex_index;
			auto c = indices[i + 2].vertex_index;
			auto d = indices[i + 3].vertex_index;

			addTriangle(a, b, c, faces);
			addTriangle(a, b, d, faces);
			addTriangle(a, c, d, faces);
			addTriangle(b, c, d, faces);
		}

		for(auto& kv : faces)
            if(kv.second) openFaces.push_back(kv.first);
	}

    bool ObjExtruder::extrudeObj4PRISM(const std::string& filepathSrc, const std::string& filepathDst, shape_t* shape, attrib_t* attrib, shape_t* oShape, attrib_t* oAttrib, std::string* err)
    {
        std::vector<glm::vec4> vertPositionsBottom{};
		std::vector<index_t> indicesBottom{};
		getData(vertPositionsBottom, indicesBottom, shape, attrib);
        
        oShape->name = shape->name + "_4D";

		// Add Top Face

		std::vector<glm::vec4> vertPositionsTop{};
		std::vector<index_t> indicesTop{};

		int vertexPerSize = vertPositionsBottom.size();
		for(int i = 0; i < vertexPerSize; i++)
		{
			vertPositionsBottom[i].w -= 1;
			glm::vec4 ePos{vertPositionsBottom[i]};
			ePos.w += 2;

			vertPositionsTop.push_back(ePos);
		}

		int indecesPerSize = indicesBottom.size();
		for(int i = 0; i < indecesPerSize; i++)
        {
			index_t eIndex
			{
				indicesBottom[i].vertex_index + vertexPerSize,
				1,
				1,
			};
			indicesTop.push_back(eIndex);
        }

		//Add Sides

		std::vector<glm::vec4> vertPositionsSide{};
		std::vector<index_t> indicesSide{};

		std::vector<face3_t> openFaces{};
		getOpenFaces(openFaces, vertPositionsBottom, indicesBottom);

		for(auto& face : openFaces)
		{
			glm::vec4& a = vertPositionsBottom[face.a - 1];
			glm::vec4& b = vertPositionsBottom[face.b - 1];
			glm::vec4& c = vertPositionsBottom[face.c - 1];
			glm::vec4& d = vertPositionsTop[face.a - 1];
			glm::vec4& e = vertPositionsTop[face.b - 1];
			glm::vec4& f = vertPositionsTop[face.c - 1];

			u_int32_t iA = face.a;
			u_int32_t iB = face.b;
			u_int32_t iC = face.c;
			u_int32_t iD = face.a + vertexPerSize;
			u_int32_t iE = face.b + vertexPerSize;
			u_int32_t iF = face.c + vertexPerSize;

			addTetra(iA, iB, iC, iE, a, b, c, e, vertPositionsSide, indicesSide);
			addTetra(iA, iC, iD, iE, a, c, d, e, vertPositionsSide, indicesSide);
			addTetra(iC, iD, iE, iF, c, d, e, f, vertPositionsSide, indicesSide);
		}

		//Update output variables

		std::vector<glm::vec4> vertPositions{};
		vertPositions.insert(vertPositions.end(), vertPositionsBottom.begin(), vertPositionsBottom.end());
		vertPositions.insert(vertPositions.end(), vertPositionsTop.begin(), vertPositionsTop.end());
		vertPositions.insert(vertPositions.end(), vertPositionsSide.begin(), vertPositionsSide.end());

		std::vector<index_t> indices{};
		indices.insert(indices.end(), indicesBottom.begin(), indicesBottom.end());
		indices.insert(indices.end(), indicesTop.begin(), indicesTop.end());
		indices.insert(indices.end(), indicesSide.begin(), indicesSide.end());

		std::map<uint32_t, glm::vec4> uniquePositions{};

		for(auto& index : indices)
		{
			oShape->mesh.indeces.push_back(index);

			uint32_t vIndex = index.vertex_index - 1;
			
			if(uniquePositions.count(vIndex) == 0)
			{
				auto pos = vertPositions[vIndex];

				oAttrib->vertices.push_back(pos.x);
				oAttrib->vertices.push_back(pos.y);
				oAttrib->vertices.push_back(pos.z);
				oAttrib->vertices.push_back(pos.w);

				uniquePositions[vIndex] = pos;
			}
		}

		oAttrib->normals.push_back(0);
		oAttrib->normals.push_back(0);
		oAttrib->normals.push_back(0);
		oAttrib->normals.push_back(0);

		oAttrib->texcoords.push_back(0);
		oAttrib->texcoords.push_back(0);
		oAttrib->texcoords.push_back(0);

        return true;
    }
}