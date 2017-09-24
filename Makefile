
SRC-DIR = src
OBJ-DIR = obj
BIN-DIR	= bin
DEP-DIR = dep

ASM-SOURCES	= $(shell find $(SRC-DIR) -type f -name '*.s')
C-SOURCES	= $(shell find $(SRC-DIR) -type f -name '*.c')
CXX-SOURCES	= $(shell find $(SRC-DIR) -type f -name '*.cpp')
LINKER-LD	= $(SRC-DIR)/linker.ld

TARGET-PLATFORM = i686-elf
TOOLS-PREFIX = /usr/local/cross/bin/$(TARGET-PLATFORM)-

KERNEL-BIN			= $(BIN-DIR)/kernel.bin
FLOPPY-IMG			= floppy.img
FLOPPY-1440k-IMG	= floppy.1440k.img

ASM-OBJECTS	:= $(patsubst $(SRC-DIR)/%.s,$(OBJ-DIR)/%.o,$(ASM-SOURCES))
C-OBJECTS	:= $(patsubst $(SRC-DIR)/%.c,$(OBJ-DIR)/%.o,$(C-SOURCES))
CXX-OBJECTS	:= $(patsubst $(SRC-DIR)/%.cpp,$(OBJ-DIR)/%.o,$(CXX-SOURCES))

C-DEPS		:= $(patsubst $(SRC-DIR)/%.c,$(OBJ-DIR)/%.d,$(C-SOURCES))
CXX-DEPS	:= $(patsubst $(SRC-DIR)/%.cpp,$(OBJ-DIR)/%.d,$(CXX-SOURCES))

COMMON-FLAGS = -I./src/lib/minilibc/ -MD -Wall -Wextra -Werror -nostdlib -nostartfiles -nodefaultlibs -fno-builtin -Wno-deprecated-declarations -Wno-unused-local-typedefs -Wno-unused-function
C-FLAGS = $(COMMON-FLAGS)
CXX-FLAGS = $(COMMON-FLAGS) -I./src/lib/ministl -I./src/lib/ministl/config -I./src/lib/ministl/std -I./src/lib/ministl/c_std -I./src/lib/ministl/tr1 -I./src/lib/mystl -std=c++17 -Wall -fno-rtti -fno-exceptions


all: $(FLOPPY-1440k-IMG)

clean:
	@rm -rf $(DEP-DIR) $(OBJ-DIR) $(BIN-DIR)
	@rm -f $(FLOPPY-IMG)
	@rm -f $(FLOPPY-1440k-IMG)

dump:
	@echo C-DEPS = $(C-DEPS)
	@echo CXX-DEPS = $(CXX-DEPS)
	@echo C-OBJECTS = $(C-OBJECTS)
	@echo CXX-OBJECTS = $(CXX-OBJECTS)
	@echo C-SOURCES = $(C-SOURCES)
	@echo CXX-SOURCES = $(CXX-SOURCES)

$(OBJ-DIR)/%.o : $(SRC-DIR)/%.s Makefile
	@echo "[nasm]  $@ <= ($<)"
	@mkdir -p $(dir $@)
	@nasm -f elf -o $@ $<

$(OBJ-DIR)/%.o: $(SRC-DIR)/%.c Makefile
	@echo "[gcc]   $@ <= ($<)"
	@mkdir -p $(dir $@)
	$(TOOLS-PREFIX)gcc -o $@ -c $< $(C-FLAGS)
	
$(OBJ-DIR)/%.o : $(SRC-DIR)/%.cpp Makefile	
	@echo "[g++]	$@ <= ($<)"
	@mkdir -p $(dir $@)
	@$(TOOLS-PREFIX)g++ -o $@ -c $< $(CXX-FLAGS)

$(KERNEL-BIN): $(LINKER-LD) $(ASM-OBJECTS) $(C-OBJECTS) $(CXX-OBJECTS) Makefile
	@echo "[ld]    $@ <= (linker.ld $(ASM-OBJECTS) $(C-OBJECTS) $(CXX-OBJECTS))"
	@mkdir -p $(BIN-DIR)
	@$(TOOLS-PREFIX)ld -T $(LINKER-LD) -o $(KERNEL-BIN) $(ASM-OBJECTS) $(C-OBJECTS) $(CXX-OBJECTS)
	@$(TOOLS-PREFIX)objdump -S $(KERNEL-BIN) > objdump.txt

$(FLOPPY-1440k-IMG): $(KERNEL-BIN) build/template.img Makefile
	@echo "[create-image]	$@ <= ($<)"
	@build/create-image.sh $(KERNEL-BIN) $(FLOPPY-1440k-IMG)	
		
-include $(C-DEPS)
-include $(CXX-DEPS)
