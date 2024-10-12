#  Project Overview

The project consists of multiple C and header files, each serving a specific purpose in the program. It adheres to principles of clarity, readability, structured writing, and data abstraction.

 Assembler Stages

The assembler works in four main stages:

1. ## Pre-Processing ##
   The pre-assembler reads the source file line by line and identifies macro definitions. If a macro is identified, the lines of the macro are added to a macro table and replaced in the source file.
2. ## Front - Error Checking ##
   After pre-processing, the assembler checks the code for errors and notifies the user if any are found. This stage ensures that the source code adheres to the expected syntax and semantics.
3. ## First Pass ##
   The first pass identifies symbols (labels) and assigns them numerical values that represent their corresponding memory addresses. It also begins to generate the binary code that does not depend on the labels.
4. ## Second Pass ##
   In the second pass, the assembler generates the final machine code by replacing operation names with their binary equivalents and symbol names with their assigned memory locations.
