cc_library(
    name = "matrix",
    hdrs = ["matrix.h"],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "matrix_test",
    srcs = ["matrix_test.cc"],
    deps = [
        ":matrix",
        "@gtest//:gtest_main",
    ],
)
