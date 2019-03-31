set(TARGET_NAME dotnet-wrapper)

link_directories(${CMAKE_BINARY_DIR}/thirdparty/SatisfactoryModLoader/satisfactory-mod-loader/thirdparty/detours/src)
link_directories(${CMAKE_BINARY_DIR}/thirdparty/SatisfactoryModLoader/satisfactory-mod-loader)

add_library(${TARGET_NAME} SHARED DotNetWrapper.cpp)

add_dependencies(${TARGET_NAME} detours)
add_dependencies(${TARGET_NAME} satisfactory-mod-loader)

set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME "${TARGET_NAME}")

if(MSVC)
    set(DETOURS_LIB detours)
    set(SML_LIB satisfactory-mod-loader)
    set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS "/WHOLEARCHIVE:detours /WHOLEARCHIVE:satisfactory-mod-loader")
else()
    set(DETOURS_LIB "-Wl,-whole-archive -l detours -Wl,-no-whole-archive")
    set(SML_LIB "-Wl,-whole-archive -l satisfactory-mod-loader -Wl,-no-whole-archive")
endif(MSVC)

add_definitions(-DUNICODE -D_UNICODE)

target_include_directories(${TARGET_NAME} PRIVATE ../include)
target_include_directories(${TARGET_NAME} PRIVATE ../thirdparty/SatisfactoryModLoader/satisfactory-mod-loader/include)
target_include_directories(${TARGET_NAME} PRIVATE ../thirdparty/SatisfactoryModLoader/satisfactory-mod-loader/thirdparty/detours/src)

target_link_libraries(${TARGET_NAME} ${DETOURS_LIB})
target_link_libraries(${TARGET_NAME} ${SML_LIB})

install(FILES ${CMAKE_BINARY_DIR}/${TARGET_NAME}/src/${TARGET_NAME}.dll DESTINATION "${CMAKE_SOURCE_DIR}/out/mods")