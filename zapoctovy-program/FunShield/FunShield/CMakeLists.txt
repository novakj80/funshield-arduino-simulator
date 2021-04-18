# CMakeList.txt: Projekt CMake pro FunShield, sem přidejte logiku zdrojového
# kódu a definic specifickou pro projekt.
#
cmake_minimum_required (VERSION 3.8)

# Přidejte zdrojový kód ke spustitelnému souboru tohoto projektu.
if(WIN32)
    set(exe_type WIN32 MACOSX_BUNDLE)
    add_compile_definitions(WXUSINGDLL)
    #set(wxBUILD_SHARED OFF)
endif()

add_subdirectory("libs/wxWidgets")

add_executable(App ${exe_type} "App.cpp" "Funshield.cpp" "novak.cpp")

target_link_libraries(App wx::base wx::core)
#if(wxBUILD_SHARED)
#    target_compile_definitions(App PRIVATE WXUSINGDLL)
#endif()
# TODO: V případě potřeby přidejte testy a cíle instalace.
