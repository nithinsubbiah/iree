// Copyright 2022 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "torch-iree/InputConversion/Passes.h"

#include "mlir/Dialect/MemRef/Transforms/Passes.h"
#include "mlir/Pass/PassManager.h"
#include "mlir/Transforms/Passes.h"
#include "torch-mlir/Conversion/TorchConversionToMLProgram/TorchConversionToMLProgram.h"
#include "torch-mlir/Conversion/TorchToArith/TorchToArith.h"
#include "torch-mlir/Conversion/TorchToLinalg/TorchToLinalg.h"
#include "torch-mlir/Conversion/TorchToSCF/TorchToSCF.h"
#include "torch-mlir/Conversion/TorchToTMTensor/TorchToTMTensor.h"
#include "torch-mlir/Dialect/Torch/Transforms/Passes.h"
#include "torch-mlir/Dialect/TorchConversion/Transforms/Passes.h"

namespace mlir {
namespace iree_compiler {
namespace TorchInput {

namespace {
#define GEN_PASS_REGISTRATION
#include "torch-iree/InputConversion/Passes.h.inc" // IWYU pragma: export
} // namespace

void createTorchToIREEPipeline(OpPassManager &pm) {
  // This pipeline adapted from
  // createTorchBackendToLinalgOnTensorsBackendPipeline. Keep in sync with
  // additions there. Lower to linalg + guards which is the input to codegen
  // backends. We do this first as it tends to involve pattern-matching against
  // constants, (e.g. dimensions which must be constant in a ranked programming
  // model) and those constants get somewhat obscured by TorchToArith.
  pm.addNestedPass<func::FuncOp>(torch::createConvertTorchToTMTensorPass());
  pm.addNestedPass<func::FuncOp>(torch::createConvertTorchToLinalgPass());
  pm.addNestedPass<func::FuncOp>(torch::createConvertTorchToSCFPass());
  pm.addNestedPass<func::FuncOp>(torch::createConvertTorchToArithPass());
  pm.addPass(torch::createConvertTorchConversionToMLProgramPass());
  pm.addNestedPass<func::FuncOp>(memref::createExpandOpsPass());

  // Clean up any non-canonical code introduced above..
  pm.addNestedPass<func::FuncOp>(createCanonicalizerPass());
  // Resolve `dim` ops on tensors (which currently live in the `memref`
  // dialect for some reason -- we don't have memrefs at this level).
  pm.addNestedPass<func::FuncOp>(
      memref::createResolveShapedTypeResultDimsPass());
  // The resolution of `dim` ops tends to create identical ops. CSE them.
  pm.addNestedPass<func::FuncOp>(createCSEPass());

  // Finish the type conversion from `torch` types to the types of the
  // linalg-on-tensors backend contract.
  pm.addPass(torch::TorchConversion::createFuncBackendTypeConversionPass());
  pm.addNestedPass<func::FuncOp>(createCanonicalizerPass());
  pm.addNestedPass<func::FuncOp>(
      torch::TorchConversion::createFinalizingBackendTypeConversionPass());

  // TODO: Add validation pass.
  pm.addPass(createSymbolDCEPass());
}

void registerTMTensorConversionPasses() {
  // Generated.
  registerPasses();

  mlir::PassPipelineRegistration<>(
      "torch-to-iree",
      "Pipeline to lower from the Torch backend contract to legal IREE input.",
      createTorchToIREEPipeline);
}

} // namespace TorchInput
} // namespace iree_compiler
} // namespace mlir