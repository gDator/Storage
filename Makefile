# Alternative GNU Make workspace makefile autogenerated by Premake

ifndef config
  config=debug_win64
endif

ifndef verbose
  SILENT = @
endif

ifeq ($(config),debug_win64)
  STORAGE_config = debug_win64

else ifeq ($(config),debug_static)
  STORAGE_config = debug_static

else ifeq ($(config),debug_shared)
  STORAGE_config = debug_shared

else ifeq ($(config),debug_linux)
  STORAGE_config = debug_linux

else ifeq ($(config),release_win64)
  STORAGE_config = release_win64

else ifeq ($(config),release_static)
  STORAGE_config = release_static

else ifeq ($(config),release_shared)
  STORAGE_config = release_shared

else ifeq ($(config),release_linux)
  STORAGE_config = release_linux

else
  $(error "invalid configuration $(config)")
endif

PROJECTS := STORAGE

.PHONY: all clean help $(PROJECTS) 

all: $(PROJECTS)

STORAGE:
ifneq (,$(STORAGE_config))
	@echo "==== Building STORAGE ($(STORAGE_config)) ===="
	@${MAKE} --no-print-directory -C . -f STORAGE.make config=$(STORAGE_config)
endif

clean:
	@${MAKE} --no-print-directory -C . -f STORAGE.make clean

help:
	@echo "Usage: make [config=name] [target]"
	@echo ""
	@echo "CONFIGURATIONS:"
	@echo "  debug_win64"
	@echo "  debug_static"
	@echo "  debug_shared"
	@echo "  debug_linux"
	@echo "  release_win64"
	@echo "  release_static"
	@echo "  release_shared"
	@echo "  release_linux"
	@echo ""
	@echo "TARGETS:"
	@echo "   all (default)"
	@echo "   clean"
	@echo "   STORAGE"
	@echo ""
	@echo "For more information, see https://github.com/premake/premake-core/wiki"