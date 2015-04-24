###########  common/mri/voxelizer/Makefile  ############
# Voxelizer is a library of GPU-accelerated voxelization routines for multi-
# component watertight triangle mesh models.
# 
# This library was initially written to be used in Jadrian's Blockhead project,
# project/brain/blockhead.
# 
# To enhance portability as a library, the .glsl shader source files are not
# actually themselves read from disk at run-time.  Instead, at compile-time a
# hexdump of each .glsl file is compiled into the binary.  This hexdump is
# stored in a corresponding .glsl.h file.
# 
# Written Spring 2013 by Jadrian Miles.

G_COMPILER  ?= gcc4

PROJECT    = mri/voxelizer
PROJECT_HOME = $(realpath ./)

LDLIB_VER += gl_compute
LDLIB_PROJECT += gl_compute

# GLX and GL --- can't use LDLIB_VER since it appends a compiler version.
LDLIB += -lX11 -lGL

LIBRARY = voxelizer
INCLUDE_H = voxelizer.h  geom_3d.h  watertight_component.h \
  gl_loadable_mesh.h  id_mesh.h  anonymesh.h  model_space.h \
  multi_component_slicemapper.h  multi_component_slicemap.h \
  multi_component_occmapper.h    multi_component_occmap.h
CXXSRC = voxelizer.cpp  geom_3d.cpp  id_mesh.cpp  anonymesh.cpp  \
  model_space.cpp \
  multi_component_slicemapper.cpp  multi_component_slicemap.cpp \
  multi_component_occmapper.cpp    multi_component_occmap.cpp
CXXFLAGS += -Wall -Wextra -Werror=return-type -Wno-sign-compare

# GLSL source files.
GLSL = multi_component_slicemapper.glsl   multi_component_occmapper.glsl

# Add the "glsl" target as a dependency for the "all" target --- this way we
# will automatically rebuild any missing .glsl.h files.
all : glsl

include  $(G)/common/build/make/directory.make
include  $(G)/common/build/make/build.multiple.make

# Source file dependencies on hexdumped GLSL files.
# If the associated .glsl.h file is new, force a rebuild of anything depending
# on the .cpp.
%mapper.cpp : %mapper.glsl.h
	touch $@

GLSL_H = $(addsuffix .h, $(GLSL))

.PHONY : glsl
glsl : $(GLSL_H)
%.glsl.h : %.glsl
	$(G)/common/build/kit/glsl_dump.sh $<

#
#########  end  common/mri/voxelizer/Makefile  #########
