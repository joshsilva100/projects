add_executable(t5 t5.c t5main.c)

#add_compile_options("-O3" "-ffast-math" "-fopt-info-vec-optimized") 

target_compile_options(t5 PRIVATE -O3 -ffast-math -fopt-info-vec-optimized)

#target_link_libraries(t5 PRIVATE m)

target_include_directories(t5 PUBLIC
	${CMAKE_SOURCE_DIR}/include
)
