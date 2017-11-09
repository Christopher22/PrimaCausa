#include "hex.h"
#include "texture.h"
#include "material.h"

#include "Utils/linearscale.h"
#include "../Controller/engine.h"

#include <cmath>
#include <cassert>

static const float PI = 3.14159265358979f;

namespace View {
	const float Hex::Size = 0.1f;
	const float Hex::INNER_RATIO = 1.0f;

	Hex::Hex(Texture* tex, Material* mat) :
		Drawable(mat),
		m_pTex(tex),
		m_texD(std::sin(60.0f * 180.0f / PI) * 0.5f),
		m_texS(0.5f) 
	{
		assert(tex);
	}

	Hex::~Hex() {
		delete m_pTex;
		m_pTex = nullptr;
	}

	void Hex::Draw(unsigned short xIn, unsigned short yIn, unsigned short layer, bool selected) {
		glEnable(GL_DEPTH_TEST);
		m_pTex->Enable();

		Drawable::GetMaterial()->Apply(selected);
		glEnable(GL_BLEND);

		const float halfSize = Size / 2;
		const float innerSize = INNER_RATIO*Size;
		const float innerHalfSize = innerSize / 2;
		const float d = std::sqrtf(std::powf(halfSize, 2) + std::powf(Size, 2));
		const float innerD = std::sqrtf(std::powf(innerHalfSize, 2.0f) + std::powf(innerSize, 2.0f));
		const float x = LinearScale::Scale(0.0f, 2.0f / ( Size*1.5f ), -1.0f, +1.0f, xIn);
		const float z = Size * layer / 2;
		float y = LinearScale::Scale(0.0f, 2.0f / ( d*2.0f ), -1.0f, +1.0f, yIn);
		
		if ( ( xIn % 2 ) != 0 ) {
			y += d;
		}

		glPushMatrix();
		glTranslatef(x, y, z);
		glBegin(GL_TRIANGLE_FAN);
		glNormal3f(0, 0, 1.0f);
		glTexCoord2f(m_texS, m_texS);
		glVertex2f(0, 0);                     //center

		glNormal3f(0, 0, 1.0f);
		glTexCoord2f(m_texS - ( m_texS / 2 ), m_texS + m_texD);
		glVertex2f(-innerHalfSize, +innerD);   //A

		glNormal3f(0, 0, 1.0f);
		glTexCoord2f(m_texS + ( m_texS / 2 ), m_texS + m_texD);
		glVertex2f(+innerHalfSize, +innerD);   //B

		glNormal3f(0, 0, 1.0f);
		glTexCoord2f(m_texS + m_texS, m_texS);
		glVertex2f(+innerSize, +0);            //C

		glNormal3f(0, 0, 1.0f);
		glTexCoord2f(m_texS + ( m_texS / 2 ), m_texS - m_texD);
		glVertex2f(+innerHalfSize, -innerD);   //D

		glNormal3f(0, 0, 1.0f);
		glTexCoord2f(m_texS - ( m_texS / 2 ), m_texS - m_texD);
		glVertex2f(-innerHalfSize, -innerD);   //E

		glNormal3f(0, 0, 1.0f);
		glTexCoord2f(m_texS - m_texS, m_texS);
		glVertex2f(-innerSize, +0);            //F

		glNormal3f(0, 0, 1.0f);
		glTexCoord2f(m_texS - ( m_texS / 2 ), m_texS + m_texD);
		glVertex2f(-innerHalfSize, +innerD);   //A
		glEnd();


		glBegin(GL_QUADS);
		glNormal3f(0, -1.0f, 0);
		glTexCoord2f(0, 0);
		glVertex3f(+innerHalfSize, -innerD, 0); //D

		glTexCoord2f(0.5f, 0);
		glVertex3f(-innerHalfSize, -innerD, 0);   //E

		glTexCoord2f(0.5f, 0.5f);
		glVertex3f(-innerHalfSize, -innerD, -Size);   //E0.5f

		glTexCoord2f(0, 0.5f);
		glVertex3f(+innerHalfSize, -innerD, -Size); //D0.5f



		glNormal3f(1.0f, 1.0f, 0);
		glTexCoord2f(0, 0);
		glVertex3f(+innerSize, +0, 0);            //C

		glTexCoord2f(0.15f, 0);
		glVertex3f(+innerHalfSize, -innerD, 0);   //D

		glTexCoord2f(0.15f, 0.15f);
		glVertex3f(+innerHalfSize, -innerD, -Size);   //D0.5f

		glTexCoord2f(0, 0.15f);
		glVertex3f(+innerSize, +0, -Size);            //C0.5f


		glTexCoord2f(0, 0);
		glVertex3f(+innerHalfSize, +innerD, 0);   //B

		glTexCoord2f(0.15f, 0);
		glVertex3f(+innerSize, +0, 0);            //C

		glTexCoord2f(0.15f, 0.15f);
		glVertex3f(+innerSize, +0, -Size);            //C0.5f

		glTexCoord2f(0, 0.15f);
		glVertex3f(+innerHalfSize, +innerD, -Size);   //B0.5f


		glTexCoord2f(0, 0);
		glVertex3f(-innerHalfSize, +innerD, 0);   //A

		glTexCoord2f(0.15f, 0);
		glVertex3f(+innerHalfSize, +innerD, 0);   //B

		glTexCoord2f(0.15f, 0.15f);
		glVertex3f(+innerHalfSize, +innerD, -Size);   //B0.5f

		glTexCoord2f(0, 0.15f);
		glVertex3f(-innerHalfSize, +innerD, -Size);   //A0.5f


		glTexCoord2f(0, 0);
		glVertex3f(-innerHalfSize, +innerD, 0); //A

		glTexCoord2f(0.15f, 0);
		glVertex3f(-innerSize, +0, 0);            //F

		glTexCoord2f(0.15f, 0.15f);
		glVertex3f(-innerSize, +0, -Size);            //F0.5f

		glTexCoord2f(0, 0.15f);
		glVertex3f(-innerHalfSize, +innerD, -Size);   //A0.5f


		glNormal3f(-1.0f, -1.0f, 0);
		glTexCoord2f(0, 0);
		glVertex3f(-innerSize, +0, 0);            //F

		glNormal3f(-1.0f, -1.0f, 0);
		glTexCoord2f(0.15f, 0);
		glVertex3f(-innerSize, +0, -Size);            //F0.5f

		glNormal3f(-1.0f, -1.0f, 0);
		glTexCoord2f(0.15f, 0.15f);
		glVertex3f(-innerHalfSize, -innerD, -Size);   //E0.5f

		glNormal3f(-1.0f, -1.0f, 0);
		glTexCoord2f(0, 0.15f);
		glVertex3f(-innerHalfSize, -innerD, 0);   //E

		glEnd();
		m_pTex->Disable();

		if ( selected && layer == 0 ) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			glLineWidth(2.0f);
			glBegin(GL_POLYGON);

			glVertex2f(-innerHalfSize, +innerD);   //A
			glVertex2f(+innerHalfSize, +innerD);   //B
			glVertex2f(+innerSize, +0);            //C
			glVertex2f(+innerHalfSize, -innerD);   //D
			glVertex2f(-innerHalfSize, -innerD);   //E
			glVertex2f(-innerSize, +0);            //F
			glVertex2f(-innerHalfSize, +innerD);   //A

			glEnd();
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		glPopMatrix();
		glDisable(GL_BLEND);
		Drawable::GetMaterial()->Apply(false);
	}
}
