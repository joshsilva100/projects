add_executable(t4 t4.c t4main.c)

#add_compile_options("-O3" "-fopt-info-vec-optimized") 

target_compile_options(t4 PRIVATE -O3 -fopt-info-vec-optimized)

target_include_directories(t4 PUBLIC
	${CMAKE_SOURCE_DIR}/include
)
