BIN       = MacMuPDF

TOOLCHAIN = /sdb/Retro68/Retro68-build/toolchain
ARCH      = m68k-apple-macos

CC        = $(TOOLCHAIN)/bin/$(ARCH)-gcc
CXX       = $(TOOLCHAIN)/bin/$(ARCH)-g++
LD        = $(TOOLCHAIN)/bin/$(ARCH)-ld
AS        = $(TOOLCHAIN)/bin/$(ARCH)-as
AR        = $(TOOLCHAIN)/bin/$(ARCH)-ar
REZ       = $(TOOLCHAIN)/bin/Rez
MAKE_APPL = $(TOOLCHAIN)/bin/MakeAPPL
LAUNCH_APPL = $(TOOLCHAIN)/bin/LaunchAPPL

CSRC      = $(wildcard source/*.c source/**/*.c)
INC       = $(wildcard source/*.h source/**/*.h)
OBJ       = $(CSRC:.c=.o)
SHAREDIR  = Shared
MUPDF_DIR = dep/mupdf-1.12.0-source
MUPDF_LIB_DIR = $(MUPDF_DIR)/build/release

CFLAGS += -O2 -std=c11 -ffunction-sections
CFLAGS += -Wno-multichar -Wno-attributes #-Werror
CFLAGS += -I$(MUPDF_DIR)/include -I$(TOOLCHAIN)/$(ARCH)/include
LDFLAGS = -lretrocrt -lm -Wl,-gc-sections

RINCLUDES = $(TOOLCHAIN)/RIncludes
REZFLAGS  = -I$(RINCLUDES)

MINI_VMAC_DIR=~/Mac/Emulation/Mini\ vMac
MINI_VMAC=$(MINI_VMAC_DIR)/Mini\ vMac
MINI_VMAC_LAUNCHER_DISK=$(MINI_VMAC_DIR)/launcher-sys.dsk

all: $(BIN).dsk

$(BIN).dsk: $(BIN).code.bin
	$(REZ) $(RINCLUDES)/Retro68APPL.r -I$(RINCLUDES) --copy $(BIN).code.bin \
		-o $(BIN).bin --cc $(BIN).dsk --cc $(BIN).APPL --cc %$(BIN).ad \
		-t 'APPL' -c '????'

$(BIN).code.bin: $(OBJ) $(MUPDF_LIB_DIR)/libmupdf.a $(MUPDF_LIB_DIR)/libmupdfthird.a
	$(CC) -o $(BIN).code.bin $(OBJ) $(MUPDF_LIB_DIR)/libmupdf.a $(MUPDF_LIB_DIR)/libmupdfthird.a $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(MUPDF_LIB_DIR)/libmupdf.a: $(MUPDF_DIR)/generated
	  cd $(MUPDF_DIR) && make CC=$(CC) CXX=$(CXX) AR=$(AR) CROSSCOMPILE=yes XCFLAGS="-DNOCJK -DTOFU -DTOFU_CJK -DNO_CJK -DNOTO_SMALL  -DFZ_PLOTTERS_RGB=1 -DFZ_PLOTTERS_G=1 -DFZ_PLOTTERS_CMYK=0 -DFZ_PLOTTERS_N=0  -DFZ_ENABLE_PDF=1 -DFZ_ENABLE_XPS=0 -DFZ_ENABLE_SVG=0 -DFZ_ENABLE_CBZ=0 -DFZ_ENABLE_IMG=0 -DFZ_ENABLE_TIFF=0 -DFZ_ENABLE_HTML=0 -DFZ_ENABLE_EPUB=0 -DFZ_ENABLE_JPX=1  -DFZ_ENABLE_JS=0 -DNO_ICC" libs

$(MUPDF_LIB_DIR)/libmupdfthird.a: $(MUPDF_LIB_DIR)/libmupdf.a

# This is run on the native platform before cross-compiling
$(MUPDF_DIR)/generated: $(MUPDF_DIR)
	cd $< && make generate

$(MUPDF_DIR): $(DEP_DIR)
	curl https://mupdf.com/downloads/mupdf-1.12.0-source.tar.gz | tar xz

$(DEP_DIR):
	mkdir -p $@

MINI_VMAC_DIR=../vMac
MINI_VMAC=$(MINI_VMAC_DIR)/Mini\ vMac
MINI_VMAC_LAUNCHER_DISK=$(MINI_VMAC_DIR)/6.0.8.dsk

MINI_VMAC_FLAGS=--emulator minivmac --minivmac-dir /sdb/Retro68/vMac --minivmac-path=/sdb/Retro68/vMac/Mini\ vMac --system-image /sdb/Retro68/vMac/6.0.8.dsk --autoquit-image /sdb/Retro68/vMac/autoquit-1.1.1_data.dsk

run: $(BIN).dsk
	$(MINI_VMAC) $(MINI_VMAC_LAUNCHER_DISK) $(BIN).dsk
	#$(LAUNCH_APPL) $(MINI_VMAC_FLAGS) $(BIN).APPL

clean:
	rm -f $(BIN) $(BIN).dsk $(BIN).bin $(BIN).code.bin $(BIN).ad $(BIN).code.bin.gdb $(BIN).APPL "%$(BIN).ad" \
		$(MUPDF_LIB_DIR)/libmupdf.a $(MUPDF_LIB_DIR)/libmupdfthird.a \
		$(OBJ) $(CDEP)
	rm -rf .finf .rsrc


.PHONY: clean run
