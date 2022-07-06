################################ Set env ################################
PREFIX       := /usr
CFLAGS       := -Iinclude/slab/bsp -Iinclude/ -Wno-pointer-arith
SHARED_FLAGS := -shared -fPIC $(CFLAGS)
OBJECTS      := src/pcam5c.cpp            \
                src/dma.cpp               \
                src/ps/PS_GPIO.cpp        \
                src/ps/PS_IIC.cpp         \
                src/video/VideoOutput.cpp \
                src/bsp/*.c               \

LIB_PATH     := $(PREFIX)/lib/slab
INCLUDE_PATH := $(PREFIX)/include/slab
LDCONF_PATH  := /etc/ld.so.conf.d
#PKGCONF_PATH := $(PREFIX)/lib/arm-linux-gnueabihf/pkgconfig
PKGCONF_PATH := $(PREFIX)/lib/pkgconfig

INSTALL_ALL  := $(LIB_PATH)/libslab_pcam.so  \
                $(INCLUDE_PATH)/pcam5c.hpp   \
                $(INCLUDE_PATH)/dma.hpp      \
                $(INCLUDE_PATH)/video        \
                $(INCLUDE_PATH)/ps           \
                $(INCLUDE_PATH)/bsp          \
                $(PKGCONF_PATH)/slab_pcam.pc \
                $(LDCONF_PATH)/slab.conf
#########################################################################

default: all

################################# Build #################################
all: lib/libslab_pcam.so

lib/libslab_pcam.so: $(OBJECTS)
	sudo rm -rf ./lib
	find . -name "*~" | xargs rm -f
	mkdir -p lib
	g++ $(SHARED_FLAGS) $(OBJECTS) -o lib/libslab_pcam.so
#########################################################################

################################ Install ################################
install: $(INSTALL_ALL)

uninstall:
	sudo rm -rf $(INSTALL_ALL)
	sudo ldconfig

$(LIB_PATH)/libslab_pcam.so: lib/libslab_pcam.so
	sudo mkdir -p $(LIB_PATH)
	sudo cp lib/libslab_pcam.so $(LIB_PATH)/libslab_pcam.so

$(INCLUDE_PATH)/pcam5c.hpp: include/slab/pcam5c.hpp
	sudo mkdir -p $(INCLUDE_PATH)
	sudo cp include/slab/pcam5c.hpp $(INCLUDE_PATH)/pcam5c.hpp

$(INCLUDE_PATH)/dma.hpp: include/slab/dma.hpp
	sudo mkdir -p $(INCLUDE_PATH)
	sudo cp include/slab/dma.hpp $(INCLUDE_PATH)/dma.hpp

$(INCLUDE_PATH)/video: include/slab/video
	sudo mkdir -p $(INCLUDE_PATH)
	sudo cp -r include/slab/video $(INCLUDE_PATH)/video

$(INCLUDE_PATH)/ps: include/slab/ps
	sudo mkdir -p $(INCLUDE_PATH)
	sudo cp -r include/slab/ps $(INCLUDE_PATH)/ps

$(INCLUDE_PATH)/bsp: include/slab/bsp
	sudo mkdir -p $(INCLUDE_PATH)
	sudo cp -r include/slab/bsp $(INCLUDE_PATH)/bsp

$(LDCONF_PATH)/slab.conf: config/slab.conf
	sudo mkdir -p $(LDCONF_PATH)
	sudo cp config/slab.conf $(LDCONF_PATH)/slab.conf
	sudo ldconfig

$(PKGCONF_PATH)/slab_pcam.pc: config/slab_pcam.pc
	sudo mkdir -p $(PKGCONF_PATH)
	sudo cp config/slab_pcam.pc $(PKGCONF_PATH)/slab_pcam.pc
#########################################################################

################################# Clean #################################
clean:
	rm -rf ./lib
	find . -name "*~" | xargs rm -f
#########################################################################
