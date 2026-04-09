cmake_minimum_required(VERSION 4.0)
project(M3OEP_nlippiat)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

add_executable(M3OEP_nlippiat
	main.cpp
	Project.cpp
	Task.cpp
	CountdownTimer.cpp
	Persistence.cpp
	AnalysisBridge.cpp)

target_link_libraries(M3OEP_nlippiat PRIVATE nlohmann_json::nlohmann_json)
