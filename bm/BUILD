cc_library(
    name = "bm",
    hdrs = ["bm.h"],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "bm_test",
    srcs = ["bm_test.cc"],
    deps = [
        ":bm",
        "@gtest//:gtest_main",
    ],
)
