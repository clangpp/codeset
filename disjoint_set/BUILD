cc_library(
    name = "disjoint_set",
    hdrs = ["disjoint_set.h"],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "disjoint_set_test",
    srcs = ["disjoint_set_test.cc"],
    deps = [
        ":disjoint_set",
        "@gtest//:gtest_main",
    ],
)
