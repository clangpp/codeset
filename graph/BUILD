cc_library(
    name = "graph",
    hdrs = ["graph.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//crosslist",
    ],
)

cc_test(
    name = "graph_test",
    srcs = ["graph_test.cc"],
    deps = [
        ":graph",
        "//crosslist",
        "@gtest//:gtest_main",
    ],
)
