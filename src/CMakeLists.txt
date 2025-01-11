# -*- explicit-buffer-name: "CMakeLists.txt<M1-MOBJ/8-10>" -*-
#
# To display executed commands, use:
#     > cmake -DCMAKE_VERBOSE_MAKEFILE:STRING=YES ../src

cmake_minimum_required(VERSION 3.5.0)
project(SCHEMATIC)

include_directories(shapes)
include_directories(graphicInterface)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "-Wall -g" CACHE STRING "C++ Compiler Release options." FORCE)
set(CMAKE_INSTALL_PREFIX "../install")

find_package(LibXml2 REQUIRED)
find_package(Qt6 COMPONENTS Widgets Core REQUIRED)

set(includes)
set(cpps)

set(CMAKE_AUTOMOC ON)


include_directories(${SCHEMATIC_SOURCE_DIR}
    ${LIBXML2_INCLUDE_DIR}
)

set(includes Indentation.h
    XmlUtil.h
    Point.h
    Box.h
    Term.h
    Net.h
    Instance.h
    Node.h
    Line.h
    Shape.h
    Symbol.h
    Cell.h
    CellWidget.h
    CellViewer.h
    SaveCellDialog.h
    OpenCellDialog.h
    InstancesModel.h
    InstancesWidget.h
    CellsLib.h
    CellsModel.h
)

set(cpps Indentation.cpp
    XmlUtil.cpp
    Point.cpp
    Box.cpp
    Term.cpp
    Net.cpp
    Instance.cpp
    Node.cpp
    Line.cpp
    Shape.cpp
    Symbol.cpp
    Cell.cpp
    SaveCellDialog.cpp
    CellWidget.cpp
    CellViewer.cpp
    Main.cpp
    OpenCellDialog.cpp
    InstancesModel.cpp
    InstancesWidget.cpp
    CellsLib.cpp
    CellsModel.cpp
)

add_executable(SNW ${cpps} ${mocCpps})
target_link_libraries(SNW Qt6::Widgets Qt6::Core ${LIBXML2_LIBRARIES})
install(TARGETS SNW DESTINATION bin)
install(FILES ${includes} ${mocIncludes} DESTINATION include)
