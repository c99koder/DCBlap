#!BPY

"""
Name: 'DCBlap'
Blender: 237
Group: 'Export'
Tooltip: 'DCBlap world exporter'
"""

import Blender
import struct
import os
import math

textures = []

def write_string(file, s):
  for n in s:
    file.write(n)
    
  file.write('\0')

def write_textures(file):
  nTextures = 0
  for tex in Blender.Texture.Get():
    if tex.type == Blender.Texture.Types.IMAGE: nTextures = nTextures + 1
  
  print "Writing %i textures" % (nTextures)
  file.write(struct.pack("<l",nTextures)) 
  
  for tex in Blender.Texture.Get():
    if tex.type == Blender.Texture.Types.IMAGE:
      textures.append(tex)
      fn = os.path.basename(tex.image.getFilename())
      write_string(file,os.path.splitext(fn)[0])
  

def write_params(file, ent):
  nParams = len(ent.getAllProperties())
  if ent.getType() != 'Mesh':
    nParams = nParams + 1
    if ent.getType() == 'Lamp':
      nParams = nParams + 4
  print "Writing %i params" % (nParams)
  file.write(struct.pack("<l",nParams))
  
  for j in ent.getAllProperties():
    write_string(file,j.name)
    write_string(file,j.data)
    
    print "%s = %s" % (j.name, j.data)

  if ent.getType() != 'Mesh':  
    origin = ""  
    origin += str(ent.getLocation()[0] * 100)
    origin += " "
    origin += str(ent.getLocation()[1] * 100)
    origin += " "
    origin += str(ent.getLocation()[2] * 100)
    write_string(file,"origin")
    write_string(file,origin)
    
  if ent.getType() == "Lamp":
   	l = ent.getData()
   	
   	write_string(file,"red")
   	write_string(file, str(l.col[0] * l.energy * 100.0))
   	write_string(file,"green")
   	write_string(file, str(l.col[1] * l.energy * 100.0))
   	write_string(file,"blue")
   	write_string(file, str(l.col[2] * l.energy * 100.0))
   	write_string(file,"radius")
   	write_string(file, str(l.dist*2))

def write_verts(file, face, mat, pos):
  nVerts = len(face.v)
  print "Writing %i verts" % (nVerts)
  file.write(struct.pack("<l",nVerts))

  for i in range(0, nVerts):
    vert = Blender.Mathutils.Vector(pos) + (mat * face.v[i].co)
    uv = face.uv[i]
    file.write(struct.pack("<3f",vert[0]*100.0, vert[2]*100.0, vert[1]*100.0))
    file.write(struct.pack("<2f",uv[0], 1-uv[1]))
      
def write_polys(file, ent):
  if ent.getType() == 'Mesh':
    nPolys = len(ent.getData().faces)
  else:
    nPolys = 0
  
  print "Writing %i polys" % (nPolys)
  file.write(struct.pack("<l",nPolys))

  if nPolys > 0:
    mesh = ent.getData()
  
    for f in mesh.faces:
      tex = textures.index(mesh.materials[f.mat].getTextures()[0].tex)
      print "Texture index: %i" % (tex)
      file.write(struct.pack("<l",tex))
            
      write_verts(file, f, ent.getMatrix(), ent.getLocation())

def write_ents(file):
  nEnts = len(Blender.Object.Get())
  print "Writing %i entities" % (nEnts)
  file.write(struct.pack("<l",nEnts))
  
  for ent in Blender.Object.Get():
    entType = os.path.splitext(ent.getName())[0]
    write_string(file,entType)
    
    print "Type: %s" % (entType)
      
    write_params(file, ent)    
    write_polys(file, ent)
    
def export_wld(path):
  # parse the file
  file = open(path, 'w')

  write_textures(file)
  write_ents(file)
  
  Blender.Window.RedrawAll()
    
Blender.Window.FileSelector(export_wld, 'Export')
