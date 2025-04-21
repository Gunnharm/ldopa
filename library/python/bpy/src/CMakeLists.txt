# Настройка Python
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
find_package(Boost COMPONENTS python3 REQUIRED)

# Добавляем директории с исходным кодом
include_directories(
    ${CMAKE_SOURCE_DIR}/library/python/bpy/src
    ${CMAKE_SOURCE_DIR}/library/src
    ${Boost_INCLUDE_DIRS}
    ${Python3_INCLUDE_DIRS}
)

# Создаем библиотеку
add_library(eventlog_python SHARED
    main.cpp
    
    # Transition systems
    ts/eventlog_ts-bpw.cpp
    ts/ts_fbuilder-bpw.cpp
    ts/ts_reducer-bpw.cpp
    ts/ts_metrics_calc-bpw.cpp
    ts/evlog_ts_dotwritter-bpw.cpp
    
    # Event log
    eventlog/sqlitelog-bpw.cpp
    eventlog/csvlog-bpw.cpp
    
    # Petri nets
    pn/eventlog_pn-bpw.cpp
    pn/pn_regs_synth-bpw.cpp
    pn/evlog_ptnets_dotwritter-bpw.cpp
    pn/pn_alpha_miner-bpw.cpp
)

# Устанавливаем префикс для Python-модуля
set_target_properties(eventlog_python PROPERTIES
    PREFIX ""
    OUTPUT_NAME "eventlog"
)

set_target_properties(eventlog_python PROPERTIES
    BUILD_RPATH "/home/ganvas/.local/lib"
    INSTALL_RPATH "/home/ganvas/.local/lib"
    CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE
)

# Устанавливаем флаги линковки для проверки неразрешенных символов
if(NOT WIN32)
    target_link_options(eventlog_python PRIVATE
        "LINKER:--no-undefined"
    )
endif()

# Устанавливаем зависимости
target_link_libraries(eventlog_python PRIVATE
    ${Boost_LIBRARIES}
    ${Python3_LIBRARIES}
    xilib  # Основная библиотека
    ldopa-lib-shared
)

# Устанавливаем путь для установки модуля
install(TARGETS eventlog_python
    LIBRARY DESTINATION ${Python3_SITEARCH}/xi/ldopa
) 