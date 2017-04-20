#include <iostream>
#include <string>
#include <unistd.h>

#include "optparse.h"
#include "classyfier/Classyfier.h"
#include "LibraryParser.h"

int main(int argc, char** argv) {
	optparse::OptionParser parser = optparse::OptionParser().description("SymbolExporter v1.0");

	parser.add_option("-i", "--input").dest("input").help("ELF File").metavar("FILE");
	parser.add_option("-o", "--output").dest("output").set_default("output/").help("Folder where the output would be stored");

	const optparse::Values options = parser.parse_args(argc, argv);

	std::string inputFile = options["input"];
	if (inputFile.empty()) {
		std::cerr << "Failed! You have to specify an input file!\n";
		std::exit(1);
	}

	std::string outputFolder = options["output"];

    LibraryParser lib(inputFile);
	std::cout << "Sucessfuly extracted Symbols and VTables!\n";

	Classyfier fier(lib.getSymbols());
	std::cout << fier.parse() << " types have been skipped\n";
	
	std::cout << "Linking VTables\n";
	for (auto& vtable : lib.getVTables()) {
		for (const auto& object : fier.getObjects()) {
			const auto& child = object->getChild(vtable.getSignature());
			if (child) {
				child->setVTable(&vtable);
			}
		}
	}

	std::cout << "Starting to write " << fier.getObjects().size() << " classes\n";
	mkdir(outputFolder.c_str());

	for (const auto& object : fier.getObjects()) {
		std::ofstream clazz(outputFolder + object->getName() + ".h", std::ios_base::out | std::ios_base::trunc);
		clazz << "#pragma once\n\n";
		clazz << *object;
		clazz.close();
	}

	std::cout << "DONE!\n";
    std::cin.get();
    return 0;
}