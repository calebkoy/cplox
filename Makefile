# Run pub get on tool directory. 
# On non-Windows OS's try replacing 'pub.bat' with 
# 'pub' if this doesn't work. 
get:
	@ cd ./tool; pub.bat get
  
test: debug 
	@ dart tool/bin/test.dart

# Compile a debug build of cplox.
debug:
	@ $(MAKE) -f cplox.make NAME=cploxd MODE=debug SOURCE_DIR=src
  
# Compile cplox.
cplox:
	@ $(MAKE) -f cplox.make NAME=cplox MODE=release SOURCE_DIR=src
	@ cp build/cplox cplox # Copy cplox to the top level for convenience.
  
.PHONY: cplox