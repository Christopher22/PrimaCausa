#include "mesh.h"
#include "window.h"
#include "drawable.h"
#include "material.h"
#include "colorprofile.h"

#include "Utils/Vec3d.h"
#include "Utils/vec2d.h"
#include "Utils/tokenizer.h"
#include "Utils/linearscale.h"

#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

static const float PI = 3.14159265358979f;

namespace View {
	const float Mesh::SIZE = 0.1f;
	const float Mesh::INNER_RATIO = 1.0f;

	Mesh::Mesh(unsigned int size, Material* mat) : Drawable(mat), m_vertices(), m_vertexTextureCoords(), m_vertexNormals(), m_Tx(0), m_Ty(0), m_Tz(0) {
		m_vertices.reserve(size);
		m_vertexTextureCoords.reserve(size);
		m_vertexNormals.reserve(size);
	}

	Mesh* Mesh::Load(const char* filepath, Material* mat) {
		std::ifstream file(filepath);
		if ( !file ) {
			return nullptr;
		}

		std::vector<Vec3d<float>> vertices, vertexNormals;
		std::vector<Vec2d<float>> vertexTextureCoords;
		std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;

		std::string line;
		std::vector<std::string> lineElements, tmp;
		while ( std::getline(file, line) ) {
			Tokenizer::Tokenize(line, ' ', lineElements);
			if ( lineElements.front() == "#" ) {
				continue;
			} else if ( lineElements.front() == "v" ) {
				vertices.push_back(Vec3d<float>(std::stof(lineElements[1]), std::stof(lineElements[2]), std::stof(lineElements[3])));
			} else if ( lineElements.front() == "vn" ) {
				vertexNormals.push_back(Vec3d<float>(std::stof(lineElements[1]), std::stof(lineElements[2]), std::stof(lineElements[3])));
			} else if ( lineElements.front() == "vt" ) {
				vertexTextureCoords.push_back(Vec2d<float>(std::stof(lineElements[1]), std::stof(lineElements[2])));
			} else if ( lineElements.front() == "f" ) {
				std::for_each(lineElements.begin() + 1, lineElements.end(), [&] (const std::string &element) {
					Tokenizer::Tokenize(element, '/', tmp);
					vertexIndices.push_back(std::stoi(tmp[0]));
					normalIndices.push_back(std::stoi(tmp[1]));
				});
			}
		}

		Mesh* result = new Mesh(vertexIndices.size(), mat);
		for ( unsigned int i = 0; i < vertexIndices.size(); i++ ) {
			result->add(vertices[vertexIndices[i] - 1], vertexNormals[normalIndices[i] - 1]);
		}

		return result;
	}

	void Mesh::add(const Vec3d<float>& vertex, const Vec3d<float>& normal, const Vec2d<float>& uv) {
		m_vertices.push_back(vertex);
		m_vertexTextureCoords.push_back(uv);
		m_vertexNormals.push_back(normal);
	}

	void Mesh::add(const Vec3d<float>& vertex, const Vec3d<float>& normal) {
		m_vertices.push_back(vertex);
		m_vertexNormals.push_back(normal);
	}

	void Mesh::add(const Vec3d<float>& vertex) {
		m_vertices.push_back(vertex);
	}

	void Mesh::Draw(unsigned short xIn, unsigned short yIn, unsigned short layer, bool selected) {
		Drawable::GetMaterial()->Apply(selected);

		const float halfSize = SIZE / 2;
		const float innerSize = INNER_RATIO * SIZE;
		const float innerHalfSize = innerSize / 2;
		const float d = std::sqrtf(std::powf(halfSize, 2) + std::powf(SIZE, 2));
		const float innerD = std::sqrtf(std::powf(innerHalfSize, 2.0f) + std::powf(innerSize, 2.0f));
		const float x = LinearScale::Scale(0.0f, 2.0f / (SIZE*1.5f), -1.0f, +1.0f, xIn);
		const float z = SIZE*layer / 2;

		float y = LinearScale::Scale(0.0f, 2.0f / ( d*2.0f ), -1.0f, +1.0f, yIn);
		if ( ( xIn % 2 ) != 0 ) {                        //offset
			y += d;
		}

		glDisable(GL_DEPTH_TEST);
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
		Drawable::GetColor()->Apply();
		glPushMatrix();
		glTranslatef(x, y, z);
		glTranslatef(m_Tx, m_Ty, m_Tz);
		glBegin(GL_TRIANGLES);

		std::for_each(m_vertices.begin(), m_vertices.end(), [] (const Vec3d<float>& vertex) {
			glVertex3f(vertex.GetX(), vertex.GetY(), vertex.GetZ());
		});

		for ( unsigned int i = 0; i < m_vertices.size(); i++ ) {
			glNormal3f(m_vertexNormals[i].GetX(), m_vertexNormals[i].GetY(), m_vertexNormals[i].GetZ());
			glVertex3f(m_vertices[i].GetX(), m_vertices[i].GetY(), m_vertices[i].GetZ());
		}

		glEnd();

		Drawable::GetMaterial()->Apply(false);
		ColorProfile::WHITE.Apply();

		glDisable(GL_COLOR_MATERIAL);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glPopMatrix();
	}

	void Mesh::Translate(float x, float y, float z){
		m_Tx = x;
		m_Ty = y;
		m_Tz = z;
	}
}