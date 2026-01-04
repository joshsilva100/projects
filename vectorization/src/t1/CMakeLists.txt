add_executable(t1 t1.c t1main.c)

target_compile_options(t1 PRIVATE -O3 -fopt-info-vec-optimized)

target_include_directories(t1 PUBLIC
	${CMAKE_SOURCE_DIR}/include
)
