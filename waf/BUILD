cc_library(
    name = "waf_core",
    hdrs = ["waf_core.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@//crosslist",
        "@//serialization",
        "@//sparsematrix",
    ],
)

cc_test(
    name = "waf_core_test",
    srcs = ["waf_core_test.cc"],
    deps = [
        ":waf_core",
        ":waf_facility",
        "@gtest//:gtest_main",
    ],
)

cc_library(
    name = "waf_facility",
    srcs = ["waf_facility.cc"],
    hdrs = ["waf_facility.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":waf_core",
        "@//ac",
        "@//serialization",
    ],
)

cc_test(
    name = "waf_facility_test",
    srcs = ["waf_facility_test.cc"],
    deps = [
        ":waf_facility",
        "@gtest//:gtest_main",
    ],
)

cc_library(
    name = "waf_shell",
    srcs = ["waf_shell.cc"],
    hdrs = ["waf_shell.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":waf_core",
        ":waf_facility",
        "@//configure",
        "@//logging",
        "@//serialization",
        "@//timing",
    ],
)

cc_test(
    name = "waf_shell_test",
    srcs = ["waf_shell_test.cc"],
    deps = [
        ":waf_shell",
        "@gtest//:gtest_main",
    ],
)

cc_binary(
    name = "main",
    srcs = ["main.cc"],
    deps = [":waf_shell"],
)
