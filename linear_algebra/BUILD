cc_library(
    name = "linear_algebra",
    hdrs = ["linear_algebra.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//matrix",
    ],
)

cc_test(
    name = "linear_algebra_test",
    srcs = ["linear_algebra_test.cc"],
    deps = [
        ":linear_algebra",
        "@gtest//:gtest_main",
    ],
)
