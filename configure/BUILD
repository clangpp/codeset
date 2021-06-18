cc_library(
    name = "configure",
    srcs = ["configure.cc"],
    hdrs = ["configure.h"],
    visibility = ["//visibility:public"],
    deps = ["@//facility"],
)

cc_test(
    name = "configure_test",
    srcs = ["configure_test.cc"],
    deps = [
        ":configure",
        "@gtest//:gtest_main",
    ],
)
