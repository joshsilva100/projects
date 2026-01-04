add_executable(t2 t2.c t2main.c)

#add_compile_options("-O3" "-fopt-info-vec-optimized") 

target_compile_options(t2 PRIVATE -O3 -fopt-info-vec-optimized)

target_include_directories(t2 PUBLIC
	${CMAKE_SOURCE_DIR}/include
)
