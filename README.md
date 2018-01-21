# SymbolExporter
This software would let you export symbols and vtables from an ELF File to it's counterpart in C++

# Options
* `-i` or `--input` - It's the ELF File to be parse. Obligatory
* `-o` or `--output` - Where you want to store the output. Default `output/`
* `-m` or `--mangled` - Would write mangled symbol on comment form before the demangled symbol

# Compiling Requirement
* GCC v5.4 or MinGW (Might work but you may need to tweak some file first. If you do please make an PR, It would be greatly appreciated)
* CMake v3.5
* Git (Optional: You could download the source directly from the repo)

# Compiling
* `git clone https://github.com/InusualZ/SymbolExporter`
* `cd SymbolExporter`
* `mkdir build && cd build`
* `cmake ..`
* `make`

Yay! You have compiled the program! Enjoy!

# Disclaimer
```
This SymbolExporter is provided by InusualZ (Wesley Moret) "as is" and "with all faults." 
InusualZ (Wesley Moret) makes no representations or warranties of any kind concerning the safety, suitability, lack of viruses, inaccuracies, typographical errors, or other harmful components of this SymbolExporter. 
There are inherent dangers in the use of any software, and you are solely responsible for determining whether this SymbolExporter is compatible with your equipment and other software installed on your equipment. 
You are also solely responsible for the protection of your equipment and backup of your data, and InusualZ (Wesley Moret) will not be liable for any damages you may suffer in connection with using, modifying, or distributing this SymbolExporter.
```
