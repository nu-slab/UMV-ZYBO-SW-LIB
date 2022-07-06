################################ Set env ################################
PREFIX       = /usr
PY_BOOST     = -lboost_python38 -lboost_numpy38 -I$(PREFIX)/include/python3.8/
PY_TARGET    = slab.so
PY_SO        = $(PREFIX)/lib/python3.8/$(PY_TARGET)
LIB          = $(PREFIX)/lib/slab
INCLUDE      = $(PREFIX)/include/slab
LDCONF       = /etc/ld.so.conf.d/slab.conf
#PKGCONF      = $(PREFIX)/lib/arm-linux-gnueabihf/pkgconfig/slab.pc
PKGCONF      = $(PREFIX)/lib/pkgconfig/slab.pc
CFLAGS       = -I`pwd`/include
SHARED_FLAGS = -shared -fPIC $(CFLAGS)
PY_FLAGS     = $(SHARED_FLAGS) $(PY_BOOST)
INSTALL_ALL  = $(LIB)/libslab_system.so $(INCLUDE)/system.hpp \
               $(INCLUDE)/pspl.hpp $(INCLUDE)/system.hpp \
               $(PKGCONF) $(LDCONF) $(PREFIX)/include/slab.hpp
#########################################################################

default: all

################################# Build #################################
all: lib/libslab_system.so # lib/$(PY_TARGET) lib/libslab_system.so

lib/$(PY_TARGET): src/pyslab.cpp lib/pspl.o lib/system.o
	g++ src/pyslab.cpp lib/pspl.o lib/system.o -o lib/$(PY_TARGET) $(PY_FLAGS)

lib/pspl.o: src/pspl.cpp
	mkdir -p lib
	g++ -c src/pspl.cpp -o lib/pspl.o $(CFLAGS)

lib/system.o: src/system.cpp
	mkdir -p lib
	g++ -c src/system.cpp -o lib/system.o $(CFLAGS)

lib/libslab_system.so: src/pspl.cpp src/system.cpp
	mkdir -p lib
	g++ $(SHARED_FLAGS) src/pspl.cpp src/system.cpp -o lib/libslab_system.so

#########################################################################


################################ Install ################################
install: clean $(INSTALL_ALL)

uninstall:
	rm -f $(INSTALL_ALL)
	ldconfig

$(PY_SO): lib/$(PY_TARGET)
	mkdir -p $(PREFIX)/lib/python3.8
	cp lib/$(PY_TARGET) $(PY_SO)

$(LIB)/libslab_system.so: lib/libslab_system.so
	mkdir -p $(LIB)
	cp lib/libslab_system.so $(LIB)/libslab_system.so

$(PREFIX)/include/slab.hpp:
	cp include/slab.hpp $(PREFIX)/include/slab.hpp

$(INCLUDE)/pspl.hpp: include/slab/pspl.hpp
	mkdir -p $(PREFIX)/include/slab
	cp include/slab/pspl.hpp $(INCLUDE)/pspl.hpp

$(INCLUDE)/system.hpp: include/slab/system.hpp
	mkdir -p $(PREFIX)/include/slab
	cp include/slab/system.hpp $(INCLUDE)/system.hpp

$(LDCONF): config/slab.conf
	mkdir -p /etc/ld.so.conf.d/
	cp config/slab.conf $(LDCONF)
	ldconfig

$(PKGCONF): config/slab.pc
	mkdir -p $(PREFIX)/lib/arm-linux-gnueabihf/pkgconfig/
	cp config/slab.pc $(PKGCONF)
#########################################################################


################################# Clean #################################
clean:
	rm -rf lib sample/*/main
	find . -name "*~" | xargs rm -rf 

#########################################################################
