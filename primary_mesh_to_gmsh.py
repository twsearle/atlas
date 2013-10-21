#! /usr/bin/env python
"""
Convert the primary mesh generated by program primarymesh.f90
to gmsh readable mesh
Note that many elements are duplicated, but for visualisation purpose
this does not matter.

Usage:
    python primary_mesh_to_gmsh.py [option] mesh.d

Options:
    --sphere      : Create a 3D spherical mesh:     mesh_sphere.msh
                    instead of a 2D latlon mesh:    mesh_latlon.msh

    --stereo      : Create a 2D stereographic mesh: mesh_stereo.msh
                    instead of a 2D latlon mesh:    mesh_latlon.msh

    --latlon      : Create a 2D latlon mesh: mesh_latlon.msh
                    This is default behaviour if above are not selected

Then, gmsh can be used as follows:
    gmsh mesh_latlon.msh fields*.msh &

where fields*.msh are regular outputs from the shallow_water.F90 program

Gmsh Tip: righ-click on any field and select "Combine Time Steps > By View Name"
"""

from argparse import ArgumentParser
from math import pi, sin, cos

parser = ArgumentParser()
parser.add_argument('--sphere',  help='spherical view',     action="store_true")
parser.add_argument('--stereo',  help='stereographic view', action="store_true")
parser.add_argument('--latlon',  help='latlon view',        action="store_true")
parser.add_argument("primary_mesh", type=str, help="modules")
args = parser.parse_args()

print( 'Reading primary mesh file "'+args.primary_mesh+'"' )
with open(args.primary_mesh,'r') as inputfile :
    header = [ int(n) for n in inputfile.readline().split() ]
    [nb_nodes,nb_edges,nb_bdry_edges,nb_coin,nb_edge,nb_before_plus_1] = header
    nodes=[]
    for n in range(nb_nodes):
        coords = [float(x) for x in inputfile.readline().split()]
        coords.append(0.)
        if args.sphere or args.stereo:
            r     = 6371.
            phi   = coords[0]
            theta = coords[1]+pi/2.
            x = r*sin(theta)*cos(phi)
            y = r*sin(theta)*sin(phi)
            z = r*cos(theta)
            if args.stereo:
                if z>0: z=1.
                else: z = -1.
            coords = [x,y,z]
        else:
            coords[0] *= 180./pi
            coords[1] *= 180./pi
        nodes.append(coords)
    
    etype = []
    elems = []
    for n in range(nb_edges):
        node_ids = [int(nid) for nid in inputfile.readline().split()]
        if node_ids[6] == 0:   # triangle
            etype.append(2)
            elems.append( [node_ids[1], node_ids[2], node_ids[4]] )
        elif node_ids[5] == 0: # hybrid_triangle
            etype.append( 2 )
            elems.append( [node_ids[1], node_ids[2], node_ids[6]] )
        else:                  # quad
            etype.append( 3 )
            elems.append( [node_ids[1], node_ids[2], node_ids[6], node_ids[5]] )

    for n in range(nb_bdry_edges):
        node_ids = [int(nid) for nid in inputfile.readline().split()]
        if node_ids[4] == 0:   # triangle
            etype.append(2)
            elems.append( [node_ids[1], node_ids[2], node_ids[3]] )
        else:                  # quad
            etype.append( 3 )
            elems.append( [node_ids[1], node_ids[2], node_ids[4], node_ids[3]] )


if args.sphere:   outputfilename = 'mesh_sphere.msh'
elif args.stereo: outputfilename = 'mesh_stereo.msh'
else:             outputfilename = 'mesh_latlon.msh'

print( 'Writing Gmsh file "'+outputfilename+'"' )
with open(outputfilename,'w') as outputfile:
    outputfile.write("$MeshFormat\n")
    outputfile.write("2.2 0 8\n")
    outputfile.write("$EndMeshFormat\n")
    outputfile.write("$Nodes\n")
    outputfile.write(str(len(nodes))+'\n')
    for n, coord in enumerate(nodes):
        outputfile.write(str(n+1)+' '+' '.join([str(x) for x in coord])+'\n')
    outputfile.write("$EndNodes\n")
    outputfile.write("$Elements\n")
    outputfile.write(str(len(elems))+'\n')
    for e, conn in enumerate(elems):
      outputfile.write(str(e+1)+' '+str(etype[e])+' 2 1 1 '+' '.join([str(n) for n in conn])+'\n')
    outputfile.write("$EndElements\n")


""" ======================================================================
The primary mesh was constructed with following node numbering conventions
( r=right vertex, l=left vertex for dual cell)

Triangles
                       4
                     /   \
                   /   l   \ 
                  1---->----2
                   \   r   /
                     \   /
                       3
 
Hybrid 
                       6
                     /   \    
                   /   l   \
                  1---->----2
                  |    r    |
                  3_________4
 
Quads 
                  5_________6
                  |    l    |
                  1---->----2
                  |    r    |
                  3_________4


Boundary faces   (orientation of boundary is such that 
 the normal is pointing into the computational domain

Triangles: 
                       3
                     /   \
                   /   l   \ 
                  1---->----2
 
Quads:
                  3_________4
                  |    l    |
                  1---->----2
 
====================================================================== """