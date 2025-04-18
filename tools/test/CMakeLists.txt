################################################################################
# Autogenerated by build_tools/bazel_to_cmake/bazel_to_cmake.py from           #
# tools/test/BUILD.bazel                                                       #
#                                                                              #
# Use iree_cmake_extra_content from iree/build_defs.oss.bzl to add arbitrary   #
# CMake-only content.                                                          #
#                                                                              #
# To disable autogeneration for this file entirely, delete this header.        #
################################################################################

iree_add_all_subdirs()

iree_lit_test_suite(
  NAME
    lit
  SRCS
    "benchmark_flags.txt"
    "compile_flags.mlir"
    "compile_pipelines.mlir"
    "compile_to_continuation.mlir"
    "compile_to_phase.mlir"
    "executable_benchmarks.mlir"
    "executable_configurations.mlir"
    "executable_sources.mlir"
    "iree-benchmark-executable.mlir"
    "iree-benchmark-module.mlir"
    "iree-compile-help.txt"
    "iree-convert-parameters.txt"
    "iree-dump-parameters.txt"
    "iree-run-mlir.mlir"
    "iree-run-module-expected.mlir"
    "iree-run-module-inputs.mlir"
    "iree-run-module-multi.mlir"
    "iree-run-module-outputs.mlir"
    "iree-run-module.mlir"
    "multiple_args.mlir"
    "multiple_exported_functions.mlir"
    "null_values.mlir"
    "parameters_scoped.mlir"
    "parameters_unscoped.mlir"
    "repeated_return.mlir"
    "scalars.mlir"
  TOOLS
    ${IREE_LLD_TARGET}
    FileCheck
    iree-benchmark-executable
    iree-benchmark-module
    iree-compile
    iree-convert-parameters
    iree-dump-parameters
    iree-opt
    iree-run-mlir
    iree-run-module
    not
  DATA
    echo_npy.py
    parameters_a.safetensors
    parameters_b.safetensors
  LABELS
    "driver=local-task"
    "hostonly"
  TIMEOUT
    300
)

### BAZEL_TO_CMAKE_PRESERVES_ALL_CONTENT_BELOW_THIS_LINE ###
