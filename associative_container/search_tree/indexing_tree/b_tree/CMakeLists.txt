find_package(Boost REQUIRED COMPONENTS filesystem system)

add_subdirectory(tests)

set(BTREE_TARGET mp_os_assctv_cntnr_srch_tr_indxng_tr_b_tr)

add_library(
        ${BTREE_TARGET}
        include/b_tree.h
        src/hhh.cpp
)

target_include_directories(
        ${BTREE_TARGET}
        PUBLIC
        ./include
        ${Boost_INCLUDE_DIRS}
)

target_link_libraries(
        ${BTREE_TARGET}
        PUBLIC
        mp_os_lggr_clnt_lggr
        mp_os_assctv_cntnr_srch_tr
        ${Boost_LIBRARIES}
)
