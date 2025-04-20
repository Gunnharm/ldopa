add_library(ldopa-lib-module-pn OBJECT
    # algos/generate/alpha/alpha_miner.cpp
    algos/generate/alpha/maximal_set_constructor.cpp
)

target_include_directories(ldopa-lib-module-pn PRIVATE ${CMAKE_SOURCE_DIR}/src) 