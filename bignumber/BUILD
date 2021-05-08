cc_library(
    name = "bignumber",
    hdrs = ["bignumber.h"],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "bignumber_test",
    srcs = ["bignumber_test.cc"],
    deps = [
        ":bignumber",
        "@gtest//:gtest_main",
    ],
)
