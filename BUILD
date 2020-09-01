common_incs=[
    ".",
    "src",
]
common_cppflags = [
    "-std=c++14", "-O3", "-Wall", "-Wextra", "-Werror",
    "-Wno-unused-local-typedefs", "-Wno-unused-function", "-Wno-unused-parameter",
    "-Wno-unused-variable", "-Wno-comment"
]

common_deps = [
    "cpp3rdlib/gtest:1.6.0@//cpp3rdlib/gtest:gtest",
    #"cpp3rdlib/jemalloc:5.0.1.prof@//cpp3rdlib/jemalloc:jemalloc",
]

cc_library(
    name="libparse2",
    srcs=[
        "src/libparse2/*.cpp",
    ],
    incs=common_incs,
    deps=common_deps,
    extra_cppflags=common_cppflags,
    export_incs=["src"],
    link_all_symbols=True,
    bundle_path="lib",
)


cc_test(
    name="libparse2_unit_test",
    srcs=[
        "test/parse/*.cpp",
    ],
    incs=common_incs,
    deps=common_deps+[
        ":libparse2",
    ],
    testdata=[
    ],
    extra_cppflags=common_cppflags,
    bundle_path="lib",
)

