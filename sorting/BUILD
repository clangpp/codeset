cc_library(
    name = "sorting",
    hdrs = ["sorting.h"],
    visibility = ["//visibility:public"],
    deps = ["@//tree"],
)

cc_test(
    name = "sorting_test",
    srcs = ["sorting_test.cc"],
    deps = [
        ":sorting",
        "@gtest//:gtest_main",
    ],
)
