set(LIBRARY_NAME ariadne-io)

add_library(${LIBRARY_NAME} OBJECT
    figure.cpp
    cairo.cpp
    gnuplot.cpp
    graphics_manager.cpp
    drawer.cpp
    command_line_interface.cpp
)

file(GLOB HEADER_FILES
	LIST_DIRECTORIES FALSE
	*.hpp
)

add_custom_target("${LIBRARY_NAME}_headers" SOURCES
	"${HEADER_FILES}"
)

foreach(WARN ${LIBRARY_EXCLUSIVE_WARN})
    target_compile_options(${LIBRARY_NAME} PUBLIC -W${WARN})
endforeach()

if(COVERAGE)
    include(CodeCoverage)
    append_coverage_compiler_flags()
endif()

