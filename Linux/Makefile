# makefile for building Dagon

# Packages required:
# libxxf86vm-dev
# libopenal-dev
# libogg-dev
# libvorbis-dev
# libtheora-dev
# lua5.1-dev
# libfreetype6-dev
# libglew1.6-dev

BIN_DIR:= build
OBJS_DIR:= build/obj
OTHERS_DIR:= otherSources
SRC_DIR:= ../Dagon

CC:= gcc
INCLUDE:= -I $(SRC_DIR) -I $(OTHERS_DIR) -I /usr/include/freetype2 -I /usr/include/lua5.1
LIBS:= -lXxf86vm -lGL -lGLU -lGLEW -lopenal -llua5.1 -lfreetype -logg -lvorbis -lvorbisfile -ltheora -lm  -lX11 -lstdc++
CFLAGS:= -O2 -Wall -static $(INCLUDE)

MODULES:= DGAudio DGAudioManager DGButton DGCameraManager \
 DGConfig DGConsole DGControl DGCursorManager DGDustData DGEffectsManager \
 DGFeedManager DGFont DGFontData DGFontManager DGImage \
 DGInterface DGLog DGNode DGObject DGOverlay DGRenderManager \
 DGRoom DGScene	DGScript DGShaderData DGSplashData DGSpot DGState \
 DGSystemUnix DGTexture DGTextureManager DGTimerManager DGVideo \
 DGVideoManager

EXTLIBS:= stb_image
OTHERS:= main

SRC:= $(patsubst %, $(SRC_DIR)/%.cpp, $(MODULES))

OBJS:= $(patsubst %, $(OBJS_DIR)/%.o, $(MODULES)) \
 $(patsubst %, $(OBJS_DIR)/%.o, $(EXTLIBS)) \
 $(patsubst %, $(OBJS_DIR)/%.o, $(OTHERS))

DAGON:= $(BIN_DIR)/dagon

build: $(OBJS_DIR) $(DAGON)

#--- Make object dir
$(OBJS_DIR):
	mkdir -p $(OBJS_DIR)

#--- C link : use gcc
$(DAGON): $(OBJS)
	$(CC) $^ -o $(DAGON) $(LIBS)

#--- C compiling : use gcc
$(OBJS_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

#--- Special cases
$(OBJS_DIR)/DGDustData.o: $(SRC_DIR)/DGDustData.c
	$(CC) $(CFLAGS) -c $< -o $@
$(OBJS_DIR)/DGFontData.o: $(SRC_DIR)/DGFontData.c
	$(CC) $(CFLAGS) -c $< -o $@
$(OBJS_DIR)/DGShaderData.o: $(SRC_DIR)/DGShaderData.c
	$(CC) $(CFLAGS) -c $< -o $@
$(OBJS_DIR)/DGSplashData.o: $(SRC_DIR)/DGSplashData.c
	$(CC) $(CFLAGS) -c $< -o $@
$(OBJS_DIR)/stb_image.o: $(OTHERS_DIR)/stb_image.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-rm -f $(DAGON) $(OBJS)

rebuild:
	clean build

