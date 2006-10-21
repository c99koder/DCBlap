#!BPY

"""
Name: 'DCBlap'
Blender: 237
Group: 'Import'
Tooltip: 'DCBlap world importer'
"""

import Blender
import struct
import math

materials = []

if Blender.Get("texturesdir") == None:
  Blender.Draw.PupMenu("Texture path not defined%t|Please specify a texture path in Blender's preferences.");
  raise RuntimeError,"Texture path not defined"

def read_string(file):
  s = ""
  c = 1;
  
  while c != '\0':
    c=file.read(1)
    if c != '\0': s += c

  return str(s)

def load_textures(file):
  nTextures = struct.unpack("<l",file.read(4))[0]
  print "Loading %i textures" % (nTextures)
  
  for i in range(0, nTextures):
    Blender.Window.DrawProgressBar(i / nTextures, "Loading textures")
    texName = read_string(file)  
    print texName
    tex = Blender.Texture.New(texName)
    tex.setType('Image')
    print Blender.Get("texturesdir") + texName + ".png"
    img = Blender.Image.Load(Blender.Get("texturesdir") + texName + ".png")
    tex.image = img
    
    mat = Blender.Material.New(texName)
    mat.setTexture(0,tex)
    mat.getTextures()[0].texco = Blender.Texture.TexCo['UV']
    mat.mode |= Blender.Material.Modes.SHADELESS
    
    materials.append(mat)

def load_params(file, ent):
  nParams = struct.unpack("<l",file.read(4))[0]
  print "Loading %i params" % (nParams)
  
  for j in range(0, nParams):
    pName = read_string(file)
    pVal = read_string(file)
    
    print "%s = %s" % (pName, pVal)
    if pName == "origin":
      words = pVal.split()
      ent.setLocation(float(words[0])/100, float(words[1])/100, float(words[2])/100)
    if pName == "angles":
      words = pVal.split()
      ent.RotX = math.radians(float(words[0]))
      ent.RotY = math.radians(float(words[2]) + 90)
      ent.RotZ = math.radians(float(words[1]))      
    else:
      ent.addProperty(pName, pVal)

def load_verts(file, mesh, tex):
  nVerts = struct.unpack("<l",file.read(4))[0]
  face = Blender.NMesh.Face()
  
  for i in range(0, nVerts):
    v = struct.unpack("<5f",file.read(4*5))
    if nVerts == 3 or nVerts == 4:
      vert = Blender.NMesh.Vert(float(v[0]/100),float(v[2]/100),float(v[1]/100))
      mesh.verts.append(vert)
      face.append(mesh.verts[len(mesh.verts)-1])
      face.uv.append(tuple([float(v[3]), float(1-v[4])]))
      face.mat = tex

  mesh.addFace(face)
  
def load_polys(file, ent):
  nPolys = struct.unpack("<l",file.read(4))[0]
  print "Loading %i polys" % (nPolys)

  if nPolys > 0:
    mesh = Blender.NMesh.New()
  
    for i in range(0, nPolys):
      tex = struct.unpack("<l",file.read(4))[0]
      
      load_verts(file, mesh, tex)
      
    mesh.materials = materials 
    ent.link(mesh)

def load_ents(file):
  scene = Blender.Scene.getCurrent()
  nEnts = struct.unpack("<l",file.read(4))[0]
  print "Loading %i entities" % (nEnts)
  
  for i in range(0, nEnts):
    Blender.Window.DrawProgressBar(i / nEnts, "Loading entities")
    entType = read_string(file)
    
    print "Type: %s" % (entType)
    if entType == "camera":
      ent = Blender.Object.New('Camera', entType)
    else:
      ent = Blender.Object.New('Mesh', entType)
      
    load_params(file, ent)    
    load_polys(file,ent)
    
    scene.link(ent)
    ent.sel = 1;
    ent.Layers = scene.Layers
    
  Blender.Window.DrawProgressBar(1.0, "Finished")
  
def import_wld(path):
  file = open(path, 'r')

  load_textures(file)
  load_ents(file)
  
  Blender.Window.RedrawAll()
    
Blender.Window.FileSelector(import_wld, 'Import')
