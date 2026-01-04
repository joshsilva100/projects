add_executable(t7 t7.c t7main.c)

#add_compile_options("-O3" "-fopt-info-vec-optimized") 

target_compile_options(t7 PRIVATE -O3 -fopt-info-vec-optimized)

target_include_directories(t7 PUBLIC
	${CMAKE_SOURCE_DIR}/include
)
