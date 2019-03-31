set(TARGET_NAME dotnet-wrapper)

link_directories(${CMAKE_BINARY_DIR}/thirdparty/SatisfactoryModLoader/satisfactory-mod-loader)

add_library(${TARGET_NAME} SHARED DotNetWrapper.cpp ClrHost.cpp ModInfo.cpp)

add_dependencies(${TARGET_NAME} satisfactory-mod-loader)

set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME "${TARGET_NAME}")

if(MSVC)
    set(SML_LIB satisfactory-mod-loader)
    set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS "/WHOLEARCHIVE:satisfactory-mod-loader")
else()
    set(SML_LIB "-Wl,-whole-archive -l satisfactory-mod-loader -Wl,-no-whole-archive")
endif(MSVC)

target_include_directories(${TARGET_NAME} PRIVATE ../include)
target_include_directories(${TARGET_NAME} PRIVATE ../thirdparty)
target_include_directories(${TARGET_NAME} PRIVATE ../thirdparty/SatisfactoryModLoader/satisfactory-mod-loader/include)

target_link_libraries(${TARGET_NAME} ${SML_LIB})

install(FILES ${CMAKE_BINARY_DIR}/src/${TARGET_NAME}.dll DESTINATION "${CMAKE_SOURCE_DIR}/../out/mods")