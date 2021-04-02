cmake_minimum_required(VERSION 3.18)
project(InformedSearch LANGUAGES CXX)

add_executable(a
	main.cpp
	TrackingAllocator.cpp
	GraphInfo.cpp
	Vertex.cpp
	PathResult.cpp
	InformedBase.cpp
	BestFirst.cpp
	AStar.cpp
)
set_target_properties(a PROPERTIES CXX_STANDARD 14 CXX_STANDARD_REQUIRED ON)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT a)

if (MSVC)
	target_compile_definitions(a PRIVATE _CRT_SECURE_NO_WARNINGS)
endif ()
