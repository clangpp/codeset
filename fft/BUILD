cc_library(
    name = "fft",
    hdrs = ["fft.h"],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "fft_test",
    srcs = ["fft_test.cc"],
    deps = [
        ":fft",
        "@gtest//:gtest_main",
    ],
)
