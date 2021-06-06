cc_library(
    name = "serialization",
    hdrs = ["serialization.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@//crosslist",
    ],
)

cc_test(
    name = "serialization_test",
    srcs = ["serialization_test.cc"],
    deps = [
        ":serialization",
        "@gtest//:gtest_main",
    ],
)
