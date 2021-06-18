cc_library(
    name = "facility",
    srcs = ["facility.cc"],
    hdrs = ["facility.h"],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "facility_test",
    srcs = ["facility_test.cc"],
    deps = [
        ":facility",
        "@gtest//:gtest_main",
    ],
)
