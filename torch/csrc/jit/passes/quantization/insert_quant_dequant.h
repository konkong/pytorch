#pragma once

#include <torch/csrc/jit/api/module.h>
#include <torch/csrc/jit/ir/ir.h>

namespace torch {
namespace jit {

/** Swap functional linear CallFunctions to aten::linear
 *  so that it can survive inline, since quant fusion need to
 *  recognize linear as one op instead of a complicated if block
 */
TORCH_API void SwapFunctionalLinear(std::shared_ptr<Graph>& graph);
/** Swap all functional linear CallFunctions in module
 */
TORCH_API void SwapFunctionalLinear(Module& module);

/** Replicate quantize node for prim::If blocks, so that we can match
 *  quantization patterns in prim::If blocks
 */
TORCH_API void ReplicateQuant(std::shared_ptr<Graph>& graph);

/** Replicate dequantize node for each use, so that we can match
 *  quantization patterns
 */
TORCH_API void ReplicateDeQuant(std::shared_ptr<Graph>& graph);

/** Quantizes two types of general ops(ops that works both for floating point
 *  and quantized Tensors) in this pass
 *  for ops that only manipulates shape, e.g. flatten, quantization
 *  is done by swapping with previous dequantize op
 *  for ops that manipulates values of Tensor, e.g. average pool, quantization
 *  is done by inserting quant/dequant ops after the op
 */
TORCH_API void PropagateQuantizationOps(std::shared_ptr<Graph>& graph);

/** \brief Insert quantize - dequantize calls to the Tensors
 *  that are observed in insert_observers pass
 *
 * For each Tensor that is observed, get the observer module and call
 * calculate_qparam on the observer module to get quantization parameters
 * and add quantize - int_repr - dequantize function calls using these
 * parameters we also have special handling for quantizing "bias" right now.
 *
 * \param module the input module
 * \param method_name the method we want to insert quantization calls for
 */
TORCH_API Module InsertQuantDeQuant(
    Module& module,
    const std::string& method_name,
    bool inplace = false,
    bool is_dynamic = false);

} // namespace jit
} // namespace torch
