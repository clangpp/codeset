cc_library(
    name = "logging",
    hdrs = ["logging.h"],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "logging_test",
    srcs = ["logging_test.cc"],
    deps = [
        ":logging",
        "@gtest//:gtest_main",
    ],
)
