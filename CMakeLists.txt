cmake_minimum_required(VERSION 3.16)
project(KSesh VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(ksesh_macos_deployment_target "11.0")
set(ksesh_apple_development_team_id "XM695N4A7T")

if (MSVC)
  add_definitions(
    /DUNICODE
    /D_UNICODE
    /source-charset:utf-8
    /we4828
    /wd4100 # Suppress "unreferenced formal parameter" warnings
  )
  add_compile_options(
    $<$<CONFIG:Debug>:/MTd>
    $<$<CONFIG:Release>:/MT>
    $<$<CONFIG:RelWithDebInfo>:/MT>
    $<$<CONFIG:MinSizeRel>:/MT>
    /MP
  )
elseif(APPLE)
  set(CMAKE_OSX_DEPLOYMENT_TARGET "${ksesh_macos_deployment_target}")
  set(CMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "${ksesh_apple_development_team_id}")
  set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
  add_compile_options(-Wno-shorten-64-to-32)
endif()

add_subdirectory(deps/JUCE)

juce_add_gui_app(ksesh
  PRODUCT_NAME "KSesh"
  APP_SANDBOX_ENABLED TRUE
  APP_SANDBOX_INHERIT FALSE
  APP_SANDBOX_OPTIONS "com.apple.security.files.user-selected.read-write;com.apple.security.files.bookmarks.app-scope"
  VERSION "${CMAKE_PROJECT_VERSION}"
  BUILD_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}"
  BUNDLE_ID "com.github.kbinani.ksesh"
  HARDENED_RUNTIME_ENABLED TRUE
  PLIST_TO_MERGE "
    <plist>
      <dict>
        <key>LSApplicationCategoryType</key>
        <string>public.app-category.productivity</string>
        <key>ITSAppUsesNonExemptEncryption</key>
        <false/>
        <key>LSMinimumSystemVersion</key>
        <string>${ksesh_macos_deployment_target}</string>
      </dict>
    </plist>"
  COMPANY_COPYRIGHT "Copyright © 2024 kbinani"
)

list(APPEND ksesh_files
  src/desktop/MainWindow.hpp
  src/desktop/MainComponent.hpp
  src/desktop/SignListComponent.hpp
  src/desktop/TextEditorComponent.hpp
  src/desktop/HieroglyphComponent.hpp
  src/desktop/SplitterComponent.hpp
  src/desktop/Content.hpp
  src/desktop/SignList.hpp
  src/desktop/Harfbuzz.hpp
  src/desktop/String.hpp
)
if (APPLE)
  list(APPEND ksesh_files src/desktop/Application.mm)
else()
  list(APPEND ksesh_files src/desktop/Application.cpp)
endif()

target_sources(ksesh PRIVATE ${ksesh_files})

juce_add_binary_data(ksesh-binary-data
  NAMESPACE ksesh::BinaryData
  HEADER_NAME BinaryData.hpp
  SOURCES
    deps/font-tools/EgyptianOpenType/font/eot.ttf
)

target_compile_definitions(ksesh
  PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:ksesh,JUCE_PRODUCT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:ksesh,JUCE_VERSION>"
)

if (MSVC)
  target_compile_definitions(ksesh
    PRIVATE
      NOMINMAX
      WIN32_LEAN_AND_MEAN
  )
  target_compile_options(ksesh PRIVATE $<$<CONFIG:Release>:/Zi>)
  target_link_options(ksesh PRIVATE $<$<CONFIG:Release>:/DEBUG>)
elseif(APPLE)
  foreach (target IN ITEMS ksesh ksesh-binary-data)
    set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH $<IF:$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>,YES,NO>)
  endforeach()
endif()

target_link_libraries(ksesh
  PRIVATE
    ksesh-binary-data
    juce::juce_gui_extra
    juce::juce_opengl
  PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
)

target_include_directories(ksesh
  PRIVATE
    deps/JUCE/modules/juce_graphics/image_formats
)

source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${ksesh_files})
