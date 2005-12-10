/*
 Tiki
 
 ShadowBox.h
 
 Copyright (C)2005 Sam Steele
 */

#ifndef __TIKI_DRW_SHADOW_BOX_H
#define __TIKI_DRW_SHADOW_BOX_H

#include "Tiki/drawable.h"
#include "Tiki/color.h"

namespace Tiki {
	namespace GL {
		
		/* ShadowBox is a box with a shadow and a border */
		class ShadowBox : public Drawable {
public:
			ShadowBox(float w, float h);
			~ShadowBox();
			
			void drawBox(float x, float y, float z, float w, float h, Color c);
			void setSize(float w, float h);
			
			//Overloaded functions
			void draw(ObjType list);
private:
				float m_w, m_h;
		};
		
	};
};
#endif //__TIKI_DRW_SHADOW_BOX_H