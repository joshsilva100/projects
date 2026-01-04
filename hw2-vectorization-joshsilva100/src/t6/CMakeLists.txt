add_executable(t6 t6.c t6main.c)

#add_compile_options("-O3" "-fopt-info-vec-optimized") 

target_compile_options(t6 PRIVATE -O3 -fopt-info-vec-optimized)

target_include_directories(t6 PUBLIC
	${CMAKE_SOURCE_DIR}/include
)
