#!/usr/bin/make -f
# Makefile for DPF-Widgets #
# ------------------------ #
# Created by falkTX
#

DPF_DIR = ../../DPF
include $(DPF_DIR)/Makefile.base.mk

# ---------------------------------------------------------------------------------------------------------------------

BUILD_CXX_FLAGS += $(DGL_FLAGS) -DDONT_SET_USING_DGL_NAMESPACE
BUILD_CXX_FLAGS += -I$(DPF_DIR)/dgl

# ---------------------------------------------------------------------------------------------------------------------

all: OpenGL$(APP_EXT)

clean:
	rm -f *.d *.o
	rm -f OpenGL$(APP_EXT)

# ---------------------------------------------------------------------------------------------------------------------

$(DPF_DIR)/build/libdgl-cairo.a:
	$(MAKE) -C $(DPF_DIR)/dgl ../build/libdgl-cairo.a

$(DPF_DIR)/build/libdgl-opengl.a:
	$(MAKE) -C $(DPF_DIR)/dgl ../build/libdgl-opengl.a

$(DPF_DIR)/build/libdgl-stub.a:
	$(MAKE) -C $(DPF_DIR)/dgl ../build/libdgl-stub.a

$(DPF_DIR)/build/libdgl-vulkan.a:
	$(MAKE) -C $(DPF_DIR)/dgl ../build/libdgl-vulkan.a

# ---------------------------------------------------------------------------------------------------------------------

OpenGL$(APP_EXT): OpenGL.cpp.o $(DPF_DIR)/build/libdgl-opengl.a
	@echo "Linking $<"
	$(SILENT)$(CXX) $^ $(LINK_FLAGS) $(DGL_SYSTEM_LIBS) $(OPENGL_LIBS) -o $@

OpenGL.cpp.o: OpenGL.cpp
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) $(OPENGL_FLAGS) -DDGL_OPENGL -c -o $@

# ---------------------------------------------------------------------------------------------------------------------

-include Cairo.cpp.d
-include OpenGL.cpp.d

# ---------------------------------------------------------------------------------------------------------------------
