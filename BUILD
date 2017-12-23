common_incs=[
    ".",
    "src",
]
common_cppflags = [
    "-std=c++14", "-O3", "-g", "-Wall", "-Wextra", "-Werror",
    "-Wno-unused-local-typedefs", "-Wno-unused-function", "-Wno-unused-parameter",
    "-Wno-unused-variable", "-Wno-comment"
]

common_deps = [
    "cpp3rdlib/gtest:1.6.0@//cpp3rdlib/gtest:gtest",
]

cc_library(
    name="parse",
    srcs=[
        "src/parse/*.cpp",
    ],
    incs=common_incs,
    deps=common_deps,
    extra_cppflags=common_cppflags,
    export_incs=["src"],
    link_all_symbols=True,
)

cc_test(
    name="parse_unit_test",
    srcs=[
        "test/parse/*.cpp",
    ],
    incs=common_incs,
    deps=common_deps+[
        ":parse",
    ],
    testdata=[
    ],
    extra_cppflags=common_cppflags,
    bundle_path="/opt/tiger/pps_parse_bin/lib",
)

