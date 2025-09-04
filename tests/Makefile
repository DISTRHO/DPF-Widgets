#!/usr/bin/make -f
# Makefile for DPF-Widgets #
# ------------------------ #
# Created by falkTX
#

DPF_DIR = ../../DPF
ifeq (,$(wildcard $(DPF_DIR)))
DPF_DIR = ../../dpf
endif

include $(DPF_DIR)/Makefile.base.mk

# ---------------------------------------------------------------------------------------------------------------------

BUILD_CXX_FLAGS += $(DGL_FLAGS) -DDONT_SET_USING_DGL_NAMESPACE
BUILD_CXX_FLAGS += -I$(DPF_DIR)/dgl -I../opengl

CAIRO_FLAGS += -DDGL_CAIRO -I../cairo
OPENGL_FLAGS += -DDGL_OPENGL -I../opengl

ifeq ($(WASM),true)
LINK_FLAGS += --shell-file=./emscripten/shell.html
LINK_FLAGS += -sEXPORTED_RUNTIME_METHODS=FS,cwrap
endif

# ---------------------------------------------------------------------------------------------------------------------

all: breeze$(APP_EXT) imgui$(APP_EXT) opengl$(APP_EXT) textedit$(APP_EXT)

clean:
	rm -f *.d *.o *.js *.html *.wasm
	rm -f breeze$(APP_EXT)
	rm -f imgui$(APP_EXT)
	rm -f opengl$(APP_EXT)
	rm -f textedit$(APP_EXT)

# ---------------------------------------------------------------------------------------------------------------------

.PHONY: $(DPF_DIR)/build/libdgl-cairo.a
.PHONY: $(DPF_DIR)/build/libdgl-opengl.a
.PHONY: $(DPF_DIR)/build/libdgl-stub.a
.PHONY: $(DPF_DIR)/build/libdgl-vulkan.a

$(DPF_DIR)/build/libdgl-cairo.a:
	$(MAKE) -C $(DPF_DIR)/dgl ../build/libdgl-cairo.a

$(DPF_DIR)/build/libdgl-opengl.a:
	$(MAKE) -C $(DPF_DIR)/dgl ../build/libdgl-opengl.a

$(DPF_DIR)/build/libdgl-stub.a:
	$(MAKE) -C $(DPF_DIR)/dgl ../build/libdgl-stub.a

$(DPF_DIR)/build/libdgl-vulkan.a:
	$(MAKE) -C $(DPF_DIR)/dgl ../build/libdgl-vulkan.a

# ---------------------------------------------------------------------------------------------------------------------

breeze$(APP_EXT): breeze.cpp.o $(DPF_DIR)/build/libdgl-opengl.a
	@echo "Linking $@"
	$(SILENT)$(CXX) $^ $(LINK_FLAGS) $(DGL_SYSTEM_LIBS) $(OPENGL_LIBS) -o $@

imgui$(APP_EXT): imgui.cpp.o imgui-src.cpp.o $(DPF_DIR)/build/libdgl-opengl.a
	@echo "Linking $@"
	$(SILENT)$(CXX) $^ $(LINK_FLAGS) $(DGL_SYSTEM_LIBS) $(OPENGL_LIBS) -o $@

opengl$(APP_EXT): opengl.cpp.o imgui-src.cpp.o $(DPF_DIR)/build/libdgl-opengl.a
	@echo "Linking $@"
	$(SILENT)$(CXX) $^ $(LINK_FLAGS) $(DGL_SYSTEM_LIBS) $(OPENGL_LIBS) -o $@

textedit$(APP_EXT): textedit.cpp.o imgui-src.cpp.o $(DPF_DIR)/build/libdgl-opengl.a
	@echo "Linking $@"
	$(SILENT)$(CXX) $^ $(LINK_FLAGS) $(DGL_SYSTEM_LIBS) $(OPENGL_LIBS) -o $@

# ---------------------------------------------------------------------------------------------------------------------

breeze.cpp.o: breeze.cpp
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) $(OPENGL_FLAGS) -c -o $@

imgui.cpp.o: imgui.cpp
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) $(OPENGL_FLAGS) -c -o $@

imgui-src.cpp.o: imgui-src.cpp
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) $(OPENGL_FLAGS) -c -o $@

opengl.cpp.o: opengl.cpp
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) $(OPENGL_FLAGS) -c -o $@

textedit.cpp.o: textedit.cpp
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) $(OPENGL_FLAGS) -c -o $@

# ---------------------------------------------------------------------------------------------------------------------

-include breeze.cpp.d
-include cairo.cpp.d
-include imgui.cpp.d
-include imgui-src.cpp.d
-include opengl.cpp.d
-include textedit.cpp.d

# ---------------------------------------------------------------------------------------------------------------------
