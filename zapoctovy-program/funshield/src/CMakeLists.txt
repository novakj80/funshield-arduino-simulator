cmake_minimum_required (VERSION 3.8)

project("funshield")

# Specify name of the .ino file with your program here
set(ino_file "hello_world.ino") 


set_source_files_properties(${ino_file} PROPERTIES LANGUAGE CXX)
set_source_files_properties(${ino_file} PROPERTIES COMPILE_OPTIONS -xc++)
if(WIN32)
    set(exe_type WIN32 MACOSX_BUNDLE)
    #add_compile_definitions(WXUSINGDLL)
    set(wxBUILD_SHARED OFF)
endif()

add_subdirectory("libs/wxWidgets")


add_executable(simulator ${exe_type} "App.cpp" "funshield.cpp" ${ino_file})

target_link_libraries(simulator wx::base wx::core)

if(wxBUILD_SHARED)
    target_compile_definitions(simulator PRIVATE WXUSINGDLL)
endif()
