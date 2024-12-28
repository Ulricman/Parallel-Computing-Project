file(GLOB ExperimentsSources CONFIGURE_DEPENDS "./*.cpp")

foreach(ExperimentsSource ${ExperimentsSources})
    get_filename_component(ExecName ${ExperimentsSource} NAME_WE)
    add_executable(${ExecName} ${ExperimentsSource} ${ProjectSources})
    target_include_directories(${ExecName} PUBLIC ${CMAKE_SOURCE_DIR}/include)
endforeach()