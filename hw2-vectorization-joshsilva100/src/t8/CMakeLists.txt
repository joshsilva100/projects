add_executable(t8 t8.c t8main.c)

#add_compile_options("-O3" "-fopt-info-vec-optimized") 

target_compile_options(t8 PRIVATE -O3 -fopt-info-vec-optimized)

target_include_directories(t8 PUBLIC
	${CMAKE_SOURCE_DIR}/include
)
