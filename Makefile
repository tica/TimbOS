
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

BOOT-CONFIG			= bootmenu.cfg
KERNEL-BIN			= $(BIN-DIR)/kernel.bin
KERNEL-ELF			= $(BIN-DIR)/kernel.elf
FLOPPY-IMG			= floppy.img
FLOPPY-1440k-IMG	= floppy.1440k.img

ASM-OBJECTS	:= $(patsubst $(SRC-DIR)/%.s,$(OBJ-DIR)/%.o,$(ASM-SOURCES))
C-OBJECTS	:= $(patsubst $(SRC-DIR)/%.c,$(OBJ-DIR)/%.o,$(C-SOURCES))
CXX-OBJECTS	:= $(patsubst $(SRC-DIR)/%.cpp,$(OBJ-DIR)/%.o,$(CXX-SOURCES))

C-DEPS		:= $(patsubst $(SRC-DIR)/%.c,$(OBJ-DIR)/%.d,$(C-SOURCES))
CXX-DEPS	:= $(patsubst $(SRC-DIR)/%.cpp,$(OBJ-DIR)/%.d,$(CXX-SOURCES))

COMMON-FLAGS = -g -MD -Wall -Wextra -Werror -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -Wno-deprecated-declarations -Wno-unused-local-typedefs -Wno-unused-function
COMMON-INCLUDES = -I./src/lib/klibc/

C-FLAGS = $(COMMON-FLAGS) $(COMMON-INCLUDES)


CXX-FLAGS = $(COMMON-FLAGS)
CXX-FLAGS += -std=c++17 -Wall -fno-rtti -fno-exceptions
CXX-FLAGS += -I./src/lib/libc++/include -I./src/lib/klibc++/include
CXX-FLAGS += -D_LIBCPP_HAS_THREAD_API_EXTERNAL
#CXX-FLAGS += -D_LIBCPP_HAS_NO_THREADS
CXX-FLAGS += -D_LIBCPP_BUILDING_LIBRARY
CXX-FLAGS += -D_LIBCPP_HAS_NO_LONG_LONG -D_LIBCPP_HAS_NO_GLOBAL_FILESYSTEM_NAMESPACE
CXX-FLAGS += -D_LIBCPP_HAS_NO_STDIN -D_LIBCPP_HAS_NO_STDOUT -D_LIBCPP_HAS_NO_THREAD_UNSAFE_C_FUNCTIONS -D_LIBCPP_HAS_NO_COROUTINES -D_LIBCPP_HAS_NO_UNICODE_CHARS
CXX-FLAGS += -D_LIBCPP_HAS_NO_BUILTIN_OPERATOR_NEW_DELETE -D_LIBCPP_NO_EXCEPTIONS

# Add c includes last, so that #include_next works
CXX-FLAGS += $(COMMON-INCLUDES)




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

$(KERNEL-BIN) $(KERNEL-ELF): $(LINKER-LD) $(ASM-OBJECTS) $(C-OBJECTS) $(CXX-OBJECTS) Makefile
	@echo "[ld]    $@ <= (linker.ld $(ASM-OBJECTS) $(C-OBJECTS) $(CXX-OBJECTS))"
	@mkdir -p $(BIN-DIR)
	@$(TOOLS-PREFIX)ld -T $(LINKER-LD) -o $(KERNEL-ELF) $(ASM-OBJECTS) $(C-OBJECTS) $(CXX-OBJECTS)
	@cp $(KERNEL-ELF) $(KERNEL-BIN)
	@strip $(KERNEL-BIN)
	@$(TOOLS-PREFIX)objdump -S $(KERNEL-ELF) > objdump.txt

$(FLOPPY-1440k-IMG): $(KERNEL-BIN) $(BOOT-CONFIG) build/template.img build/create-image.sh build/mtools.cfg Makefile
	@echo "[create-image]	$@ <= ($<)"
	@build/create-image.sh $(KERNEL-BIN) $(BOOT-CONFIG) $(FLOPPY-1440k-IMG)	
	
debug: $(KERNEL-ELF) $(FLOPPY-1440k-IMG)
	/mnt/c/Program\ Files/qemu/qemu-system-i386.exe -s -drive file=$(FLOPPY-1440k-IMG),format=raw,if=floppy -serial file:serial.log &
	gdb -ex "target remote localhost:1234" -ex "symbol-file $(KERNEL-ELF)" -ex "c"
		
-include $(C-DEPS)
-include $(CXX-DEPS)
