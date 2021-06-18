cc_library(
    name = "timing",
    hdrs = ["timing.h"],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "timing_test",
    srcs = ["timing_test.cc"],
    deps = [
        ":timing",
        "@gtest//:gtest_main",
    ],
)
