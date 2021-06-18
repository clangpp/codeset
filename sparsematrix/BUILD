cc_library(
    name = "sparsematrix",
    hdrs = ["sparsematrix.h"],
    visibility = ["//visibility:public"],
    deps = ["@//crosslist"],
)

cc_test(
    name = "sparsematrix_test",
    srcs = ["sparsematrix_test.cc"],
    deps = [
        ":sparsematrix",
        "@gtest//:gtest_main",
    ],
)
