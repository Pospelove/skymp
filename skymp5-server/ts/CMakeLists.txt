include(${CMAKE_SOURCE_DIR}/cmake/npm.cmake)

npm_execute_command(
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  COMMAND install
)

file(GLOB_RECURSE sources ${PROJECT_SOURCE_DIR}/ts/*)

add_custom_target(skymp5-server-ts ALL
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  SOURCES ${sources}
  COMMAND npm run build-ts
)