cmake_minimum_required(VERSION 4.0)
project(project_manager_plus)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

add_executable(project_manager_plus
	main.cpp
	Project.cpp
	Task.cpp
	CountdownTimer.cpp
	Persistence.cpp
	AnalysisBridge.cpp)

target_link_libraries(project_manager_plus PRIVATE nlohmann_json::nlohmann_json)
