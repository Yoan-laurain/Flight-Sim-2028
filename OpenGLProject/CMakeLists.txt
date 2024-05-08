set (SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set (INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(PROJECT_NAME OpenGLProject)

file(GLOB_RECURSE HEADERS
    "${INCLUDE_DIR}/*.h"
    "${INCLUDE_DIR}/*.hxx"
)

file(GLOB_RECURSE SOURCES
    "${SRC_DIR}/*.cpp"
    "${SRC_DIR}/*.shader"
)

add_executable(${PROJECT_NAME}
    ${HEADERS}
    ${SOURCES}
)

# Headers
foreach(_header IN ITEMS ${HEADERS})
    get_filename_component(_header_path "${_header}" PATH)
    string(REPLACE "${CMAKE_SOURCE_DIR}/${PROJECT_NAME}" "" _header_path "${_header_path}")
    string(REPLACE "/" "\\" _header_path "${_header_path}")
    source_group("${_header_path}" FILES "${_header}")
endforeach()

# Sources
foreach(_source IN ITEMS ${SOURCES})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "${CMAKE_SOURCE_DIR}/${PROJECT_NAME}" "" _group_path "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_group_path}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${INCLUDE_DIR}>
    PRIVATE
        ${SRC_DIR}
)

target_link_libraries(${PROJECT_NAME} 
    PUBLIC
        OpenGL::GL
	glfw
	GLEW::GLEW
	imgui::imgui
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/res
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/res
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/src/shaders
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
)

file(COPY ${CMAKE_SOURCE_DIR}/res DESTINATION ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
file(COPY ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/src/Shaders DESTINATION ${CMAKE_BINARY_DIR}/${PROJECT_NAME})