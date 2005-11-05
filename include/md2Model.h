/* md2Model.h - MD2 model rendering functions
 * Copyright (c) 2001-2005 Sam Steele
 * based on MD2 Loader for Sega Dreamcast by KamKa
 *
 * This file is part of DCBlap.
 *
 * DCBlap is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * DCBlap is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */
#ifndef __MD2_MODEL_H__
#define __MD2_MODEL_H__

#include <stdio.h>

#define MD2_MAGIC_NO	('I'+('D'<<8)+('P'<<16)+('2'<<24))

#define MAX_NO_SKIN		5
#define MAX_SKIN_NAME	64

#define FRAME_HEADER_SIZE	(sizeof(float)*6+16)

typedef unsigned char	byte;
typedef short*	pshort;

typedef struct _md2Header
{
	int		magic;
	int		version;
	int		skinWidth;
	int		skinHeight;
	int		frameSize;

	int		numSkins;
	int		numVertices;
	int		numTexCoords;
	int		numTriangles;
	int		numGlCommands;
	int		numFrames;

	int		offsetSkins;
	int		offsetTexCoord;
	int		offsetTriangles;
	int		offsetFrames;
	int		offsetGlCommands;
	int		offsetEnd;
}md2Header,*pmd2Header;


typedef struct _md2TriangleVertex
{
	byte	vertex[3];
	byte	lightNormalIndex;
}md2TriangleVertex,*pmd2TriangleVertex;

typedef struct _md2Frame
{
	float				scale[3];
	float				translate[3];
	char				name[16];
	pmd2TriangleVertex	pvertices;
}md2Frame,*pmd2Frame;

typedef struct _md2Triangle
{
	short	vertexIndices[3];
	short	textureIndices[3];
}md2Triangle,*pmd2Triangle;

typedef struct _md2TextureCoord
{
	short	s,t;
}md2TextureCoord,*pmd2TextureCoord;

typedef struct _md2GLCommand
{
	float	s,t;
	int		vertexIndex;
}md2GLCommand,*pmd2GLCommand;

typedef struct _md2GLVertex
{
	float	x,y,z;
}md2GLVertex,*pmd2GLVertex;

struct tris {
  _md2GLVertex v1;
  _md2GLVertex v2;
  _md2GLVertex v3;
};

class md2Model
{
	public:
		md2Model();
		~md2Model();

		int		Load(const char* fileName);
		int		GetNumFrames();
		void	Draw();
		void	SetFrame(int frame1, int frame2, int blendpos, int blendcount); // base zero
		int anim_start(char *name);
    int anim_end(char *name);

    void bound_min(float &x, float &y, float &z);
    void bound_max(float &x, float &y, float &z);
	private:
		void*	md2Malloc(size_t size);
		void	md2Free(void** p);
		int		md2ReadFile(const char* fileName);
		int		md2ReadHeader(byte *buffer,pmd2Header phead);
		long	md2FileSize(FILE *fp);
		void	md2DumpHeader(const pmd2Header phead);

		void	md2InitData();
		void	md2LoadData();
		void	md2LoadSkinNames();
		void	md2LoadTextureCoord();
		void	md2LoadFrames();
		void	md2LoadTriangles();
		void	md2LoadGLCommands();
		void	md2LoadTextures(const char* md2FileName);
		void	md2ProcessData();
		
		void	md2WriteLog(const char* fileName);

	private:
		byte*				m_buffer;
		md2Header			m_header;
		char				m_skinName[MAX_NO_SKIN][MAX_SKIN_NAME];
		pmd2TextureCoord	m_texData;
		pmd2Triangle		m_triData;
		pmd2TriangleVertex	m_vertData;
		pmd2Frame			m_frameData;

		pmd2GLVertex		m_glVertexData;
		pshort				m_glIndicesData;
   	tris *m_tris;

		unsigned int		m_listID;
};

#endif //__MD2_MODEL_H__