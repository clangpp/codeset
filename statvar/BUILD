cc_library(
    name = "statvar",
    hdrs = ["statvar.h"],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "statvar_test",
    srcs = ["statvar_test.cc"],
    deps = [
        ":statvar",
        "@gtest//:gtest_main",
    ],
)
