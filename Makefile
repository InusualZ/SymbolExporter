#Project Name
PROJECT=SymbolExporter

#Sources Files
SOURCES_FILES=source\Main.cpp source\classyfier\Object.cpp source\classyfier\Classyfier.cpp source\VTable.cpp source\Symbol.cpp source\LibraryParser.cpp

#Objects File
OBJECTS_FILES=$(SOURCES_FILES:.cpp=.o)

#Compile Flags
CXXFLAGS=-std=c++11 -Ofast -Wall

#Link Flags
LFLAGS=-static-libgcc -static-libstdc++

#Build Directory
BUILD_DIRECTORY=build\\

#Objec Directory
OBJECT_DIRECTORY=$(BUILD_DIRECTORY)obj\\

$(PROJECT): before $(OBJECTS_FILES)
	@echo Linking $@
	@g++ $(addprefix $(OBJECT_DIRECTORY), $(notdir $(OBJECTS_FILES))) $(LFLAGS) -o $(BUILD_DIRECTORY)$@

%.o: %.cpp
	@echo Compiling: $<
	@g++ $< -c $(CXXFLAGS) -o $(OBJECT_DIRECTORY)$(notdir $@)

before:
	-@mkdir $(OBJECT_DIRECTORY)

.PHONY: clean before

clean:
	@rm -rf $(OBJECT_DIRECTORY)