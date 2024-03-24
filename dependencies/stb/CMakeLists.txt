cpmaddpackage(
        NAME stb
        DOWNLOAD_ONLY
        GIT_REPOSITORY "git@github.com:nothings/stb.git"
        GIT_TAG master
)
add_library(stb_image STATIC src/image.cpp)
target_include_directories(stb_image
        PUBLIC include
        PRIVATE "${stb_SOURCE_DIR}"
)
sl_target_link_system_libraries(stb_image
        PRIVATE
        libassert::assert

        PUBLIC
        tl::optional
)
