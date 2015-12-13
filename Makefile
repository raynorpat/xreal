default:
	@echo "WARNING: build system doesn't rely on GNU make. Spawning SCons"
	@echo "  use scons -h for help"
	scons --debug=time --jobs=2

all:
	scons --debug=time --jobs=2
	
clean:
	scons -c