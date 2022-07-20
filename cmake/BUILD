package(default_visibility = ["//visibility:public"])

load("//tools/rules_pkg:pkg.bzl", "pkg_deb", "pkg_tar")

genrule(
    name = "download_model",
    outs = [
        "data",
    ],
    cmd = "wget ftp://172.16.11.50/software/pretrained/pb/resnet50_v1.5.pb -P $(location data); wget ftp://172.16.11.50/software/pretrained/onnx/resnet50_v1.5.onnx -P $(location data)",
)

filegroup(
    name = "TopsInference_pysamples_group",
    srcs = glob([
        "py-samples/common/**",
        "py-samples/resnet/**",
        "py-samples/resnetasync/**",
        "py-samples/resnetautobatch/**",
        "py-samples/resnetonnxfp16/**",
        "data/**",
    ]),
)

pkg_tar(
    name = "TopsInference_pysample_codes",
    srcs = [":TopsInference_pysamples_group", ":download_model"],
    mode = "0644",
    remap_paths = select({
        "@bazel_tools//src/conditions:linux_x86_64": {
            "tests/TopsInference/sample_code/py-samples/" : "py-samples/",
            "tests/TopsInference/sample_code/data/" : "data/",
            "bazel-out/k8-opt/bin/tests/TopsInference/sample_code/data": "data/",
        },
        "@bazel_tools//src/conditions:linux_aarch64": {
            "tests/TopsInference/sample_code/py-samples/" : "py-samples/",
            "tests/TopsInference/sample_code/data/" : "data/",
            "bazel-out/aarch64-opt/bin/tests/TopsInference/sample_code/data": "data/",
        },
    }),
    visibility = ["//visibility:public"],
)

filegroup(
    name = "TopsInference_cppsamples_group",
    srcs = glob([
        "cpp-samples/common/**",
        "cpp-samples/sampleAsyncResNet/**",
        "cpp-samples/sampleAutoBatchResNet/**",
        "cpp-samples/sampleResNet/**",
        "cpp-samples/sampleONNXFP16ResNet/**",
        "cpp-samples/*.txt",
        "data/**"
    ]),
    visibility = ["//visibility:public"],
)

pkg_tar(
    name = "TopsInference_cppsample_codes",
    srcs = [":TopsInference_cppsamples_group", ":download_model"],
    mode = "0644",
    package_dir = "/usr/local/dtu/sample/TopsInference",
    remap_paths = select({
        "@bazel_tools//src/conditions:linux_x86_64": {
            "tests/TopsInference/sample_code/cpp-samples/" : "cpp-samples/",
            "tests/TopsInference/sample_code/data/" : "data/",
            "bazel-out/k8-opt/bin/tests/TopsInference/sample_code/data": "data/",
        },
        "@bazel_tools//src/conditions:linux_aarch64": {
            "tests/TopsInference/sample_code/cpp-samples/" : "cpp-samples/",
            "tests/TopsInference/sample_code/data/" : "data/",
            "bazel-out/aarch64-opt/bin/tests/TopsInference/sample_code/data": "data/",
        },
    }),
    visibility = ["//visibility:public"],
)
