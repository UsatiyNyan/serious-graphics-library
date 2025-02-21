cpmaddpackage(
        NAME implot
        DOWNLOAD_ONLY
        GIT_REPOSITORY "git@github.com:epezent/implot.git"
        GIT_TAG v0.16
)

add_library(implot STATIC
        "${implot_SOURCE_DIR}/implot.cpp"
        "${implot_SOURCE_DIR}/implot_items.cpp"

)
target_include_directories(implot
        PUBLIC "${implot_SOURCE_DIR}"
)
sl_target_link_system_libraries(implot
        PUBLIC
        imgui
)

add_library(implot_demo STATIC
        "${implot_SOURCE_DIR}/implot_demo.cpp"
)
sl_target_link_system_libraries(implot_demo
        PUBLIC
        implot
)
