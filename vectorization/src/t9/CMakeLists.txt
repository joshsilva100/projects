add_executable(t9 t9.c t9main.c)

#add_compile_options("-O3" "-fopt-info-vec-optimized") 

target_compile_options(t9 PRIVATE -O3 -fopt-info-vec-optimized)

target_include_directories(t9 PUBLIC
	${CMAKE_SOURCE_DIR}/include
)
