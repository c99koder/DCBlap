#!/usr/bin/python

import struct
import math
import sys

materials = []
vCount = 1

def read_string(file):
  s = ""
  c = 1;
  
  while c != '\0':
    c=file.read(1)
    if c != '\0': s += c

  return str(s)

def load_textures(file):
  out = open('materials.mtl', 'wb')
  nTextures = struct.unpack("<l",file.read(4))[0]
  print "Loading %i textures" % (nTextures)
  
  for i in range(0, nTextures):
    texName = read_string(file)  
    print texName
    out.write('newmtl ')
    out.write(texName)
    out.write('\n')
    out.write('Kd 1.000 1.000 1.000\n')
    out.write('map_Kd ' + texName.lower() + '.png\n')
    out.write('d 1.0\nillum 2\n\n')
    
    materials.append(texName)

  out.close()

def load_params(file):
  nParams = struct.unpack("<l",file.read(4))[0]
  print "Loading %i params" % (nParams)
  
  for j in range(0, nParams):
    pName = read_string(file)
    pVal = read_string(file)
    
    print "%s = %s" % (pName, pVal)

def load_polys(file, out):
  nPolys = struct.unpack("<l",file.read(4))[0]
  print "Loading %i polys" % (nPolys)
  global vCount

  if nPolys > 0:
    for i in range(0, nPolys):
      tex = struct.unpack("<l",file.read(4))[0]
      nVerts = struct.unpack("<l",file.read(4))[0]

      for i in range(0, nVerts):
        v = struct.unpack("<5f",file.read(4*5))
        out.write('v ')
        out.write(str(v[0]/100))
        out.write(' ')
        out.write(str(v[1]/100))
        out.write(' ')
        out.write(str(v[2]/100))
        out.write('\n')

        out.write('vt ')
        out.write(str(v[3] * -1))
        out.write(' ')
        out.write(str(v[4] * -1))
        out.write('\n')

      out.write('\nusemtl ')
      out.write(materials[tex])
      out.write('\nf ')
      for i in range(0, nVerts):
        out.write(str(vCount + i))
        out.write('/')
        out.write(str(vCount + i))
        out.write(' ')
      out.write('\n\n')
      vCount += nVerts

def load_ents(file):
  nEnts = struct.unpack("<l",file.read(4))[0]
  print "Loading %i entities" % (nEnts)
  obj_count = 0
  
  out = open('out.obj', 'wb')
  out.write('mtllib materials.mtl\n')

  for i in range(0, nEnts):
    entType = read_string(file)
    
    print "Type: %s" % (entType)
      
    load_params(file)
    out.write("o " + entType + '.' + str(obj_count) + "\n")
    obj_count = obj_count + 1
    load_polys(file,out)
  out.close()
  
def import_wld(path):
  file = open(path, 'rb')

  load_textures(file)
  load_ents(file)

import_wld(sys.argv[1])
