The Simple Netlist Viewer (SNV) allows a user to display the netlist (or electrical schematic) of a cell, either by building it in the main, or by importing it from an xml file.

Each cell in made of terminals (I/O), nets (or wires), and instances (representation of other cells used in the current one).
It also should provide a model, though that is not strictly necessary.

Controls in the main window are:
Ctrl +/- : change zoom level from 1 to max(int)
Alt +/- : adjust primary color (RGB) of selected element (Lines, Instances, Terminals, Background)
Shift up/down : change selected primary color
Shift L/I/T/B : Change selected element
Arrow keys : move the viewport

To build a cell from scratch in the main, the following steps must be followed in order : 
1. Call the CTOR of Cell : `Cell *pointer_to_cell = new Cell("name");` Note : the returned pointer is also stored in a static array of the class Cell.
2. Call the CTOR of Term as many times as needed to add all desired I/O : `new Term(owner_cell, "name", Term::direction);` Note : direction is one those : In, Out, Inout, Tristate, Transcv, Unknown
3. Call the CTOR of Net as many times as needed to add all desired wires : `Net *new_net = new Net(owner_cell, "name", Term::type)` Note : type is one of those : External (should be connected to the Cell's terminals), Internal (should be connected only to Instances' terminals)
4. Call the CTOR of Instance as many times as needed to add all desired sub modules : `Instance *new_instance = new Instance(owner_cell, model_cell, "name");` Note : the model_cell is used for the drawing
5. Call the connect() method on the cell and its instances to establish the link between terms and and nets : `pointer_to_cell -> connect("terminal's name", net);` Note : the terminal is found by its name

To save a cell, call the save method : `pointer_to_cell -> save("name", "path/to/cell");`
To load a cell from an xml file, call the load method : `Cell::load("name")` Note : the cell name should match exactly the xml file name (without the extension), and the file should be located in ../cells with respect to the running directory

If during load there is a missing or damaged cell from the dependancy tree, the missing models won't be loaded. Instead, a default cell shall be used and no drawing will occur apart from the name. An error message will be displayed in the terminal.

To rebuild the project, create the following directories : build, install, install/bin, install/include
Go into build, and run `cmake ../src`, then `make install`
You can then directly run `./SNV`
The graphical interface doesn't allow for the creation or modification of a cell.
