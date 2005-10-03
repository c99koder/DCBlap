/* md2Model.cc - MD2 model rendering functions
 * based on MD2 Loader for Sega Dreamcast by KamKa
 * Copyright (c) 2001-2003 Sam Steele
 *
 * This file is part of DCBlap.
 *
 * DCBlap is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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
#ifdef WIN32
#include <windows.h>
#include <GL\gl.h>
#endif
#ifdef DREAMCAST
#include <kos.h>
#include <GL/gl.h>
#endif
#ifdef LINUX
#include <GL/gl.h>
#endif
#ifdef MACOS
#include <OpenGL/gl.h>
#endif
#include <string.h>
#include <stdlib.h>
#include "md2Model.h"

#include <algorithm> //required for std::swap

#define ByteSwap5(x) ByteSwap((unsigned char *) &x,sizeof(x))

inline void ByteSwap(unsigned char * b, int n)
{
#ifdef MACOS
	register int i = 0;
	register int j = n-1;
	while (i<j)
	{
		std::swap(b[i], b[j]);
		i++, j--;
	}
#endif
}

extern char tex_theme_dir[100];

md2Model::md2Model()
{
	m_buffer	=NULL;
	m_texData	=NULL;
	m_triData	=NULL;
	m_vertData	=NULL;
	m_frameData	=NULL;
	m_glVertexData=NULL;
	m_glIndicesData=NULL;
}

md2Model::~md2Model()
{
	md2Free((void**)&m_buffer);
	md2Free((void**)&m_texData);
	md2Free((void**)&m_triData);
	md2Free((void**)&m_vertData);
	md2Free((void**)&m_glVertexData);
	md2Free((void**)&m_glIndicesData);
	for (int index=0;index<m_header.numFrames;index++)
		md2Free((void**)&m_frameData[index].pvertices);
	md2Free((void**)&m_frameData);
}

/*
	Name : Load
*/
int md2Model::Load(const char* fileName)
{
	int result=0;

	result=md2ReadFile(fileName);
	if (result!=0)
		return result;

	result=md2ReadHeader(m_buffer,&m_header);
	if (result!=0)
		return result;
	
	md2InitData();
	md2LoadData();
	md2ProcessData();
	SetFrame(0,0,0,1); // Initially set it to Point to First Frame

	return 0;
}

/*
	Name : md2Malloc
*/
void* md2Model::md2Malloc(size_t size)
{
	void* p=malloc(size);
	if (p==NULL)
		return p;
	memset(p,0,size);

	return p;
}

/*
	Name : md2Free
*/
void md2Model::md2Free(void** p)
{
	if (*p!=NULL)
	{
		free(*p);
		*p=NULL;
	}
}

/*
	Name : md2ReadFile
*/
int md2Model::md2ReadFile(const char* fileName)
{
	FILE	*fp=NULL;
	char fn2[200];

	if(tex_theme_dir[0]!='\0') {
		strcpy(fn2,tex_theme_dir);
		strcat(fn2,fileName);
		printf("Theme alternative: %s\n",fn2);
		fp=fopen(fn2,"rb");
	}
	if(!fp) fp=fopen(fileName,"rb");
	if (!fp) {
		return -1;
	}

	md2Free((void**)&m_buffer);

	long fileSize= md2FileSize(fp);
	if (fileSize<=0)
		return 1;

	m_buffer=(byte*)md2Malloc(fileSize);
	if (!m_buffer)
		return 1;

	if (fread(m_buffer,1,fileSize,fp)!=(size_t)fileSize)
		return 1;

	fclose(fp);
	return 0;
}

/*
	Name : md2FileSize
*/
long md2Model::md2FileSize(FILE *fp)
{
	long oldpos=ftell(fp);
	fseek(fp,0,SEEK_END);
	long curpos=ftell(fp);
	fseek(fp,oldpos,SEEK_SET);
	return curpos;
}




/*
	Name : md2ReadHeader
*/
int md2Model::md2ReadHeader(byte *buffer,pmd2Header phead)
{
	memcpy(phead,buffer,sizeof(*phead));

	ByteSwap5(phead->magic);
	ByteSwap5(phead->version);
	ByteSwap5(phead->skinWidth);
	ByteSwap5(phead->skinHeight);
	ByteSwap5(phead->frameSize);
		
	ByteSwap5(phead->numSkins);
	ByteSwap5(phead->numVertices);
	ByteSwap5(phead->numTexCoords);
	ByteSwap5(phead->numTriangles);
	ByteSwap5(phead->numGlCommands);
	ByteSwap5(phead->numFrames);
		
	ByteSwap5(phead->offsetSkins);
	ByteSwap5(phead->offsetTexCoord);
	ByteSwap5(phead->offsetTriangles);
	ByteSwap5(phead->offsetFrames);
	ByteSwap5(phead->offsetGlCommands);
	ByteSwap5(phead->offsetEnd);
		
		
		if (phead->magic != MD2_MAGIC_NO)
		return 1;

	return 0;
}

/*
	Name : md2InitData
*/
void md2Model::md2InitData()
{
	int index=0;

	for (index=0;index<MAX_NO_SKIN;index++)
		memset(this->m_skinName[index],0,MAX_SKIN_NAME);

	md2Free((void**)&m_texData);
	md2Free((void**)&m_triData);
	md2Free((void**)&m_vertData);
	md2Free((void**)&m_frameData);
	if (m_frameData!=NULL)
	{
		for (int index=0;index<m_header.numFrames;index++)
			md2Free((void**)&m_frameData[index].pvertices);
		md2Free((void**)&m_frameData);
	}
}

/*
	Name : md2LoadData
*/
void md2Model::md2LoadData()
{
	md2LoadSkinNames();
	md2LoadTextureCoord();
	md2LoadTriangles();
	md2LoadFrames();
	md2LoadGLCommands();
}

/*
	Name : md2LoadFrames
*/
void md2Model::md2LoadFrames()
{
	int index=0;
	byte	*buf_t		= m_buffer+ m_header.offsetFrames;
	long	frameHeaderSize	= m_header.numFrames * sizeof(md2Frame);
	long	frameVertSize	= m_header.numVertices * sizeof(md2TriangleVertex); 

	m_frameData =(pmd2Frame)md2Malloc(frameHeaderSize);
	if (!m_frameData)
		return;

	for (index=0;index< m_header.numFrames;index++)
	{
		m_frameData[index].pvertices = (pmd2TriangleVertex)md2Malloc(frameVertSize);
		if (!m_frameData[index].pvertices)
			return;
	}

	for (index=0;index< m_header.numFrames;index++)
	{
		memcpy(&m_frameData[index],buf_t, FRAME_HEADER_SIZE);
		buf_t+= FRAME_HEADER_SIZE;
		memcpy(m_frameData[index].pvertices, buf_t, frameVertSize);
		buf_t+=frameVertSize;
		for(int index2=0; index2<3; index2++) {
			ByteSwap5(m_frameData[index].scale[index2]);
			ByteSwap5(m_frameData[index].translate[index2]);
		}
		for(int index2=0; index2<m_header.numVertices; index2++) {
			for(int index3=0; index3<3; index3++) {
				ByteSwap5(m_frameData[index].pvertices[index2].vertex[index3]);
			}
		}
	}
}

/*
	Name : md2LoadTriangles
*/
void md2Model::md2LoadTriangles()
{
	byte	*buf_t		= m_buffer+ m_header.offsetTriangles;
	long	totalsize	= m_header.numTriangles * sizeof(md2Triangle);
	
	m_triData = (pmd2Triangle)md2Malloc(totalsize);
	if (!m_triData)
		return;

	memcpy(m_triData, buf_t, totalsize);

	for(int index=0; index<m_header.numTriangles; index++) {
		for(int index2=0; index2<3; index2++) {
			ByteSwap5(m_triData[index].vertexIndices[index2]);
			ByteSwap5(m_triData[index].textureIndices[index2]);
		}
	}
}

/*
	Name : md2LoadTextureCoord
*/
void md2Model::md2LoadTextureCoord()
{
	int		index=0;
	byte	*buf_t=m_buffer+m_header.offsetTexCoord;
	long	totalsize = m_header.numTexCoords*sizeof(md2TextureCoord);

	m_texData=(pmd2TextureCoord)md2Malloc(totalsize);
	if (!m_texData)
		return;

	memcpy(m_texData,buf_t,totalsize);
	
	for(index=0; index<m_header.numTexCoords;index++) {
		ByteSwap5(m_texData[index].s);
		ByteSwap5(m_texData[index].t);
	}
}

/*
	Name : md2LoadSkinNames
*/
void md2Model::md2LoadSkinNames()
{
	int		index=0;
	byte*	buf_t=m_buffer+m_header.offsetSkins;

	for (index=0;index<m_header.numSkins;index++,buf_t+=MAX_SKIN_NAME)
		strcpy(m_skinName[index],(const char*)buf_t);
}

/*
	Name : md2LoadGLCommands
*/
void md2Model::md2LoadGLCommands()
{
	return;
}

/*
	Name : md2ProcessData
*/
void md2Model::md2ProcessData()
{
	md2Free((void**)&m_glVertexData);
	m_glVertexData = (pmd2GLVertex) md2Malloc(m_header.numVertices*sizeof(md2GLVertex));
	if (!m_glVertexData)
	{
		printf("Error at md2ProcessData\n");
		return;
	}
}

void md2Model::SetFrame(int frame1, int frame2, int blendpos, int blendcount)
{
  float x1,y1,z1,x2,y2,z2;
	int index=0;

	//printf("+++md2Model::SetFrame(frame1=%i frame2=%i blendpos=%i)\n",frame1,frame2,blendpos);
	if (m_glVertexData!=NULL)
	{
		//printf("memset()\n");
		memset(m_glVertexData,0,sizeof(md2GLVertex)*m_header.numVertices);

		for (index=0;index<m_header.numVertices;index++)
		{
			//printf("interpolating points...\n");
      //interpolate
 			x1=(m_frameData[frame1].pvertices[index].vertex[0] * m_frameData[frame1].scale[0])+m_frameData[frame1].translate[0];
			x2=(m_frameData[frame2].pvertices[index].vertex[0] * m_frameData[frame2].scale[0])+m_frameData[frame2].translate[0];
			y1=(m_frameData[frame1].pvertices[index].vertex[1] * m_frameData[frame1].scale[1])+m_frameData[frame1].translate[1];
			y2=(m_frameData[frame2].pvertices[index].vertex[1] * m_frameData[frame2].scale[1])+m_frameData[frame2].translate[1];
			z1=(m_frameData[frame1].pvertices[index].vertex[2] * m_frameData[frame1].scale[2])+m_frameData[frame1].translate[2];
			z2=(m_frameData[frame2].pvertices[index].vertex[2] * m_frameData[frame2].scale[2])+m_frameData[frame2].translate[2];

			//printf("Scaling...\n");
			// Every vertex of frame is multiplied by it's respective scale and then the translation is added.
			m_glVertexData[index].x = x1-((x1-x2)/blendcount)*blendpos;
			m_glVertexData[index].y = y1-((y1-y2)/blendcount)*blendpos;
			m_glVertexData[index].z = z1-((z1-z2)/blendcount)*blendpos;
		}
	}
	//printf("---md2Model::SetFrame()\n");
}

void md2Model::bound_min(float &x, float &z, float &y) {
  x=m_glVertexData[0].x;
  y=m_glVertexData[0].y;
  z=m_glVertexData[0].z;
  int i;

  for(i=0;i<m_header.numVertices;i++) {
    if(x>m_glVertexData[i].x) {
      x=m_glVertexData[i].x;
    }
    if(y>m_glVertexData[i].y) {
      y=m_glVertexData[i].y;
    }
    if(z>m_glVertexData[i].z) {
      z=m_glVertexData[i].z;
    }
  }
  x/=50.0f;
  y/=50.0f;
  z/=50.0f;
}

void md2Model::bound_max(float &x, float &z, float &y) {
  x=m_glVertexData[0].x;
  y=m_glVertexData[0].y;
  z=m_glVertexData[0].z;
  int i;

  for(i=0;i<m_header.numVertices;i++) {
    if(x<m_glVertexData[i].x) {
      x=m_glVertexData[i].x;
    }
    if(y<m_glVertexData[i].y) {
      y=m_glVertexData[i].y;
    }
    if(z<m_glVertexData[i].z) {
      z=m_glVertexData[i].z;
    }
  }
  x/=50.0f;
  y/=50.0f;
  z/=50.0f;
}

/*
	Name : md2WriteLog
*/
void md2Model::md2WriteLog(const char* fileName)
{
	char	data[512]={0};
	char	logfname[255]={0};
	FILE	*fp;
	int		index=0;
	pmd2Header	phead= &m_header;

	strcpy(logfname,fileName);
	strcpy(logfname+strlen(fileName)-3,"log");

	fp=fopen(logfname,"w");
	if (!fp)
		return;

	sprintf(data,"File Name       : %s\n", fileName);fwrite(data,strlen(data),1,fp);
	sprintf(data,"magic           : %c%c%c%c\n", (phead->magic&0xFF),((phead->magic>>8)&0xFF),((phead->magic>>16)&0xFF),((phead->magic>>24)&0xFF));fwrite(data,strlen(data),1,fp);
	sprintf(data,"version         : %d\n", phead->version);fwrite(data,strlen(data),1,fp);
	sprintf(data,"skinWidth       : %d\n", phead->skinWidth);fwrite(data,strlen(data),1,fp);
	sprintf(data,"skinHeight      : %d\n", phead->skinHeight);fwrite(data,strlen(data),1,fp);
	sprintf(data,"frameSize       : %d\n", phead->frameSize);fwrite(data,strlen(data),1,fp);
	sprintf(data,"numSkins        : %d\n", phead->numSkins);fwrite(data,strlen(data),1,fp);
	sprintf(data,"numVertices     : %d\n", phead->numVertices);fwrite(data,strlen(data),1,fp);
	sprintf(data,"numTexCoords    : %d\n", phead->numTexCoords);fwrite(data,strlen(data),1,fp);
	sprintf(data,"numTriangles    : %d\n", phead->numTriangles);fwrite(data,strlen(data),1,fp);
	sprintf(data,"numGlCommands   : %d\n", phead->numGlCommands);fwrite(data,strlen(data),1,fp);
	sprintf(data,"numFrames       : %d\n", phead->numFrames);fwrite(data,strlen(data),1,fp);
	sprintf(data,"---------------------\n");fwrite(data,strlen(data),1,fp);
	sprintf(data,"offsetSkins     : %d\n", phead->offsetSkins);fwrite(data,strlen(data),1,fp);
	sprintf(data,"offsetTexCoord  : %d\n", phead->offsetTexCoord);fwrite(data,strlen(data),1,fp);
	sprintf(data,"offsetTriangles : %d\n", phead->offsetTriangles);fwrite(data,strlen(data),1,fp);
	sprintf(data,"offsetFrames    : %d\n", phead->offsetFrames);fwrite(data,strlen(data),1,fp);
	sprintf(data,"offsetGlCommands: %d\n", phead->offsetGlCommands);fwrite(data,strlen(data),1,fp);
	sprintf(data,"offsetEnd       : %d\n", phead->offsetEnd);fwrite(data,strlen(data),1,fp);

	sprintf(data,"Total Frames  : %.3d\n----------------------\n", m_header.numFrames);fwrite(data,strlen(data),1,fp);
	for (index=0;index< m_header.numFrames;index++)
	{
		sprintf(data, "[%3d] %s\n", (index+1), m_frameData[index].name);
		fwrite(data,strlen(data),1,fp);
	}

	fclose(fp);
}

/*
	Name : GetNumFrames
*/
int	md2Model::GetNumFrames() 
{ 
	return this->m_header.numFrames; 
}

/*
	Name : Draw()
	// frame : is base zero index
*/
void md2Model::Draw()
{
	//GLfloat	v1[3],v2[3],v3[3];
	

	/*
	glCallList(m_listID);

	GLenum ret=glGetError();
	if (ret!=GL_NO_ERROR)
		printf("An error has occured\n");
		*/
	int index=0;
	glBegin(GL_TRIANGLES);
		for (index=0;index< m_header.numTriangles;index++)
		{
			// Calculate the two vertex out of 3 by [v1 - v2] & [v2 - v3] (Read Bernard Koleman Elementary Linear Algebra P-193)
			/*v1[0] = m_glVertexData[m_triData[index].vertexIndices[2]].x - m_glVertexData[m_triData[index].vertexIndices[1]].x;
			v1[1] = m_glVertexData[m_triData[index].vertexIndices[2]].y - m_glVertexData[m_triData[index].vertexIndices[1]].y;
			v1[2] = m_glVertexData[m_triData[index].vertexIndices[2]].z - m_glVertexData[m_triData[index].vertexIndices[1]].z;

			v2[0] = m_glVertexData[m_triData[index].vertexIndices[1]].x - m_glVertexData[m_triData[index].vertexIndices[0]].x;
			v2[1] = m_glVertexData[m_triData[index].vertexIndices[1]].y - m_glVertexData[m_triData[index].vertexIndices[0]].y;
			v2[2] = m_glVertexData[m_triData[index].vertexIndices[1]].z - m_glVertexData[m_triData[index].vertexIndices[0]].z;

			// Take the Cross Product of v1 x v2
			v3[0] = v1[1]*v2[2] - v1[2]*v2[1];
			v3[1] = v1[2]*v2[0] - v1[0]*v2[2];
			v3[2] = v1[0]*v2[1] - v1[1]*v2[0];*/
		
			//glNormal3fv(v3);
#ifdef WIN32
      glTexCoord2f((float)m_texData[m_triData[index].textureIndices[2]].s/(float)m_header.skinWidth,1-((float)m_texData[m_triData[index].textureIndices[2]].t/(float)m_header.skinHeight));
#else
      glTexCoord2f((float)m_texData[m_triData[index].textureIndices[2]].s/(float)m_header.skinWidth,((float)m_texData[m_triData[index].textureIndices[2]].t/(float)m_header.skinHeight));
#endif
			glVertex3f( m_glVertexData[m_triData[index].vertexIndices[2]].x/50.0f,
						m_glVertexData[m_triData[index].vertexIndices[2]].y/50.0f,
						m_glVertexData[m_triData[index].vertexIndices[2]].z/50.0f);
#ifdef WIN32
      glTexCoord2f((float)m_texData[m_triData[index].textureIndices[1]].s/(float)m_header.skinWidth,1-((float)m_texData[m_triData[index].textureIndices[1]].t/(float)m_header.skinHeight));
#else
      glTexCoord2f((float)m_texData[m_triData[index].textureIndices[1]].s/(float)m_header.skinWidth,((float)m_texData[m_triData[index].textureIndices[1]].t/(float)m_header.skinHeight));
#endif
			glVertex3f( m_glVertexData[m_triData[index].vertexIndices[1]].x/50.0f,
						m_glVertexData[m_triData[index].vertexIndices[1]].y/50.0f,
						m_glVertexData[m_triData[index].vertexIndices[1]].z/50.0f);
#ifdef WIN32
      glTexCoord2f((float)m_texData[m_triData[index].textureIndices[0]].s/(float)m_header.skinWidth,1-((float)m_texData[m_triData[index].textureIndices[0]].t/(float)m_header.skinHeight));
#else
      glTexCoord2f((float)m_texData[m_triData[index].textureIndices[0]].s/(float)m_header.skinWidth,((float)m_texData[m_triData[index].textureIndices[0]].t/(float)m_header.skinHeight));
#endif
      glVertex3f( m_glVertexData[m_triData[index].vertexIndices[0]].x/50.0f,
						m_glVertexData[m_triData[index].vertexIndices[0]].y/50.0f,
						m_glVertexData[m_triData[index].vertexIndices[0]].z/50.0f);
		}
	glEnd();
}

int md2Model::anim_start(char *name) {
  int x;
  for (x=0;x<GetNumFrames();x++) {
    if(!strncmp(name,m_frameData[x].name,strlen(name))) {
      return x;
    }
  }
  return -1;
}

int md2Model::anim_end(char *name) {
	int i;
	for(i=0;i<GetNumFrames();i++) {
		if(!strncmp(name,m_frameData[i].name,strlen(name))) {
      if(i==GetNumFrames()) return i;
			while(i<GetNumFrames() && !strncmp(name,m_frameData[i].name,strlen(name))) {
                          i++;
                        }
                        return i-1;
		}
	}
	return -1;
}
