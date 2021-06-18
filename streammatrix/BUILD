cc_library(
    name = "streammatrix",
    hdrs = ["streammatrix.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@//crosslist",
        "@//serialization",
    ],
)

cc_test(
    name = "streammatrix_test",
    srcs = ["streammatrix_test.cc"],
    deps = [
        ":streammatrix",
        "@gtest//:gtest_main",
    ],
)
