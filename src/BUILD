
# For //:lace_lib.
exports_files([
    "infile.c",
    "instream.c",
    "kve.c",
    "kve.h",
    "log.c",
    "outfile.c",
    "outstream.c",
    "string.c",
], visibility = [
    "//:__pkg__",
    "//test/fuzz:__pkg__",
    "//test/src:__pkg__",
])

exports_files([
    "godo.c",
    "time2sec.c",
    "transpose.c",
    "void.c",
], visibility = [
    "//test/builtin:__pkg__",
])

cc_binary(
    name = "lace",
    srcs = [
        "lace.c",
        "utilace.h",
        "best-match.c",
        "execfd.c",
        "godo.c",
        "ssh-all.c",
        "transpose.c",
        "ujoin.c",
        "waitdo.c",
        "xpipe.c",
        "//builtin:add.c",
        "//builtin:cmp.c",
        "//builtin:elastic_pthread.c",
        "//builtin:seq.c",
        "//builtin:time2sec.c",
        "//builtin:void.c",
        "//builtin:zec.c",
        "//include:lace_builtin.h",
        "//include:lace_posix_thread.h",
    ],
    defines = [
        "LACE_BUILTIN_LIBRARY",
    ],
    deps = [
        "//src/cx:cx_lib",
    ],
    visibility = ["//:__pkg__"],
)

cc_binary(
    name = "bestmatch",
    srcs = ["best-match.c"],
    deps = ["//:lace_lib"],
    visibility = ["//test/builtin:__pkg__"],
)

cc_binary(
    name = "chatty",
    srcs = ["chatty.c"],
    deps = ["//:lace_lib"],
    linkopts = select({
        "@platforms//os:linux": ["-lrt"],
        "//conditions:default": [],
    }),
    target_compatible_with = [
        "@platforms//os:linux",
    ],
    visibility = ["//test/src:__pkg__"],
)

cc_binary(
    name = "elastic_aio",
    srcs = ["elastic_aio.c"],
    deps = ["//src/cx:cx_lib"],
    linkopts = select({
        "@platforms//os:linux": ["-lrt"],
        "//conditions:default": [],
    }),
    target_compatible_with = [
        "@platforms//os:linux",
    ],
    visibility = ["//test/builtin:__pkg__"],
)

cc_binary(
    name = "elastic_poll",
    srcs = ["elastic_poll.c"],
    deps = ["//src/cx:cx_lib"],
    target_compatible_with = select({
        "@platforms//os:windows": ["@platforms//:incompatible"],
        "//conditions:default": [],
    }),
    visibility = ["//test/builtin:__pkg__"],
)

cc_binary(
    name = "sshall",
    srcs = ["ssh-all.c"],
    deps = ["//:lace_lib"],
    visibility = ["//test/builtin:__pkg__"],
)

cc_binary(
    name = "ujoin",
    srcs = ["ujoin.c"],
    deps = ["//src/cx:cx_lib"],
    visibility = ["//test/builtin:__pkg__"],
)

cc_binary(
    name = "waitdo",
    srcs = ["waitdo.c", "utilace.h"],
    deps = ["//src/cx:cx_lib"],
    visibility = ["//test/builtin:__pkg__"],
)

