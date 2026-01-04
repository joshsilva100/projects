add_executable(t3 t3.c t3main.c)

#add_compile_options("-O3" "-fopt-info-vec-optimized") 

target_compile_options(t3 PRIVATE -O3 -fopt-info-vec-optimized)

target_include_directories(t3 PUBLIC
	${CMAKE_SOURCE_DIR}/include
)
