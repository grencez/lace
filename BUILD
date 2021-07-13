
alias(
    name = "lace",
    actual = "//src:lace",
    visibility = ["//visibility:public"],
)

cc_library(
    name = "lace_compat_lib",
    srcs = [
        "//compat:errno.c",
        "//compat:fd.c",
        "//compat:fd_exclusive.h",
        "//compat:file.c",
        "//compat:random.c",
        "//compat:sh.c",
        "//compat:string.c",
        "//include:lace_compat_errno.h",
        "//include:lace_compat_fd.h",
        "//include:lace_compat_file.h",
        "//include:lace_compat_random.h",
        "//include:lace_compat_sh.h",
        "//include:lace_compat_string.h",
    ],
    includes = ["include"],
    linkopts = select({
        "@platforms//os:windows": [
            "-DEFAULTLIB:advapi32.lib",  # For RtlGenRandom().
        ],
        "//conditions:default": ["-pthread"],
    }),
    visibility = ["//visibility:public"],
)

cc_library(
    name = "lace_lib",
    srcs = [
        "//include:lace.h",
        "//src:infile.c",
        "//src:instream.c",
        "//src:kve.c",
        "//src:kve.h",
        "//src:log.c",
        "//src:outfile.c",
        "//src:outstream.c",
        "//src:string.c",
    ],
    includes = ["include"],
    deps = [":lace_compat_lib"],
    visibility = ["//visibility:public"],
)


cc_library(
    name = "lace_tool_lib",
    srcs = [
        "//include:lace_posix_thread.h",
        "//include:lace_tool.h",
        "//tool:pipem.c",
    ],
    defines = ["LACE_TOOL_LIBRARY"],
    includes = ["include"],
    deps = [":lace_compat_lib"],
    visibility = ["//visibility:public"],
)
