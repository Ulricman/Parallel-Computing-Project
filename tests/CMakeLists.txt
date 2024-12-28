file(GLOB TestSources CONFIGURE_DEPENDS "./*.cpp")

foreach(TestSource ${TestSources})
    get_filename_component(ExecName ${TestSource} NAME_WE)
    add_executable(${ExecName} ${TestSource} ${ProjectSources})
    target_include_directories(${ExecName} PUBLIC ${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(${ExecName} PUBLIC GTest::GTest GTest::Main)
endforeach()