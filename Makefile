#=============================================================
# Toolchain definitions
#=============================================================
#ARC_TOOLCHAIN ?= mwdt
ARC_TOOLCHAIN ?= gnu

ROOT_PATH = ./../../..
HIMAX_PATH = $(ROOT_PATH)/himax_tflm
INC_PATH = ./inc
SRC_PATH = ./src
TFL_PATH = $(HIMAX_PATH)/tensorflow/lite
TFLM_PATH = $(HIMAX_PATH)/tensorflow/lite/micro
PROJECT_PATH = $(shell pwd)

ifeq ($(ARC_TOOLCHAIN), mwdt)
	CC = ccac
	AR = arac
	CXX = ccac
	LD = ccac
else ifeq ($(ARC_TOOLCHAIN), gnu)
	CC := arc-elf32-gcc
	AR := arc-elf32-ar
	CXX := arc-elf32-g++
	LD := arc-elf32-ld
endif

GEN_TOOL_DIR = $(HIMAX_PATH)/image_gen_linux
ifeq ($(ARC_TOOLCHAIN), mwdt)
GEN_TOOL_NAME = image_gen
else ifeq ($(ARC_TOOLCHAIN), gnu)
GEN_TOOL_NAME = image_gen_gnu
endif



#=============================================================
# Files and directories
#=============================================================

SRCS := \
$(TFLM_PATH)/himax_we1_evb/debug_log.cc \
$(TFLM_PATH)/all_ops_resolver.cc \
$(TFLM_PATH)/flatbuffer_utils.cc \
$(TFLM_PATH)/memory_helpers.cc \
$(TFLM_PATH)/micro_allocator.cc \
$(TFLM_PATH)/micro_error_reporter.cc \
$(TFLM_PATH)/micro_graph.cc \
$(TFLM_PATH)/micro_interpreter.cc \
$(TFLM_PATH)/micro_profiler.cc \
$(TFLM_PATH)/micro_resource_variable.cc \
$(TFLM_PATH)/micro_string.cc \
$(TFLM_PATH)/micro_time.cc \
$(TFLM_PATH)/micro_utils.cc \
$(TFLM_PATH)/mock_micro_graph.cc \
$(TFLM_PATH)/recording_micro_allocator.cc \
$(TFLM_PATH)/recording_simple_memory_allocator.cc \
$(TFLM_PATH)/simple_memory_allocator.cc \
$(TFLM_PATH)/system_setup.cc \
$(TFLM_PATH)/test_helpers.cc \
$(TFL_PATH)/schema/schema_utils.cc \
$(TFLM_PATH)/memory_planner/linear_memory_planner.cc \
$(TFLM_PATH)/memory_planner/greedy_memory_planner.cc \
$(TFLM_PATH)/memory_planner/non_persistent_buffer_planner_shim.cc \
$(TFL_PATH)/c/common.c \
$(TFL_PATH)/core/api/error_reporter.cc \
$(TFL_PATH)/core/api/flatbuffer_conversions.cc \
$(TFL_PATH)/core/api/op_resolver.cc \
$(TFL_PATH)/core/api/tensor_utils.cc \
$(TFL_PATH)/kernels/internal/reference/portable_tensor_utils.cc \
$(TFL_PATH)/kernels/internal/quantization_util.cc \
$(TFL_PATH)/kernels/kernel_util.cc \
$(TFLM_PATH)/kernels/arc_mli/add.cc \
$(TFLM_PATH)/kernels/arc_mli/conv.cc \
$(TFLM_PATH)/kernels/arc_mli/depthwise_conv.cc \
$(TFLM_PATH)/kernels/arc_mli/fully_connected.cc \
$(TFLM_PATH)/kernels/arc_mli/mli_interface.cc \
$(TFLM_PATH)/kernels/arc_mli/pooling.cc \
$(TFLM_PATH)/kernels/activations_common.cc \
$(TFLM_PATH)/kernels/activations.cc \
$(TFLM_PATH)/kernels/add_common.cc \
$(TFLM_PATH)/kernels/add_n.cc \
$(TFLM_PATH)/kernels/arg_min_max.cc \
$(TFLM_PATH)/kernels/assign_variable.cc \
$(TFLM_PATH)/kernels/batch_to_space_nd.cc \
$(TFLM_PATH)/kernels/call_once.cc \
$(TFLM_PATH)/kernels/cast.cc \
$(TFLM_PATH)/kernels/ceil.cc \
$(TFLM_PATH)/kernels/circular_buffer_common.cc \
$(TFLM_PATH)/kernels/circular_buffer.cc \
$(TFLM_PATH)/kernels/comparisons.cc \
$(TFLM_PATH)/kernels/concatenation.cc \
$(TFLM_PATH)/kernels/conv_common.cc \
$(TFLM_PATH)/kernels/cumsum.cc \
$(TFLM_PATH)/kernels/depth_to_space.cc \
$(TFLM_PATH)/kernels/depthwise_conv_common.cc \
$(TFLM_PATH)/kernels/dequantize_common.cc \
$(TFLM_PATH)/kernels/dequantize.cc \
$(TFLM_PATH)/kernels/detection_postprocess.cc \
$(TFLM_PATH)/kernels/elementwise.cc \
$(TFLM_PATH)/kernels/elu.cc \
$(TFLM_PATH)/kernels/ethosu.cc \
$(TFLM_PATH)/kernels/exp.cc \
$(TFLM_PATH)/kernels/expand_dims.cc \
$(TFLM_PATH)/kernels/fill.cc \
$(TFLM_PATH)/kernels/floor.cc \
$(TFLM_PATH)/kernels/floor_div.cc \
$(TFLM_PATH)/kernels/floor_mod.cc \
$(TFLM_PATH)/kernels/fully_connected_common.cc \
$(TFLM_PATH)/kernels/gather.cc \
$(TFLM_PATH)/kernels/gather_nd.cc \
$(TFLM_PATH)/kernels/hard_swish_common.cc \
$(TFLM_PATH)/kernels/hard_swish.cc \
$(TFLM_PATH)/kernels/if.cc \
$(TFLM_PATH)/kernels/kernel_runner.cc \
$(TFLM_PATH)/kernels/kernel_util.cc \
$(TFLM_PATH)/kernels/l2norm.cc \
$(TFLM_PATH)/kernels/l2_pool_2d.cc \
$(TFLM_PATH)/kernels/leaky_relu_common.cc \
$(TFLM_PATH)/kernels/leaky_relu.cc \
$(TFLM_PATH)/kernels/log_softmax.cc \
$(TFLM_PATH)/kernels/logical.cc \
$(TFLM_PATH)/kernels/logical_common.cc \
$(TFLM_PATH)/kernels/logistic.cc \
$(TFLM_PATH)/kernels/logistic_common.cc \
$(TFLM_PATH)/kernels/maximum_minimum.cc \
$(TFLM_PATH)/kernels/mul.cc \
$(TFLM_PATH)/kernels/mul_common.cc \
$(TFLM_PATH)/kernels/neg.cc \
$(TFLM_PATH)/kernels/pack.cc \
$(TFLM_PATH)/kernels/pad.cc \
$(TFLM_PATH)/kernels/pooling_common.cc \
$(TFLM_PATH)/kernels/prelu.cc \
$(TFLM_PATH)/kernels/prelu_common.cc \
$(TFLM_PATH)/kernels/quantize.cc \
$(TFLM_PATH)/kernels/quantize_common.cc \
$(TFLM_PATH)/kernels/read_variable.cc \
$(TFLM_PATH)/kernels/reduce.cc \
$(TFLM_PATH)/kernels/reshape.cc \
$(TFLM_PATH)/kernels/resize_bilinear.cc \
$(TFLM_PATH)/kernels/resize_nearest_neighbor.cc \
$(TFLM_PATH)/kernels/round.cc \
$(TFLM_PATH)/kernels/shape.cc \
$(TFLM_PATH)/kernels/slice.cc \
$(TFLM_PATH)/kernels/softmax.cc \
$(TFLM_PATH)/kernels/softmax_common.cc \
$(TFLM_PATH)/kernels/space_to_batch_nd.cc \
$(TFLM_PATH)/kernels/space_to_depth.cc \
$(TFLM_PATH)/kernels/split.cc \
$(TFLM_PATH)/kernels/split_v.cc \
$(TFLM_PATH)/kernels/squeeze.cc \
$(TFLM_PATH)/kernels/strided_slice.cc \
$(TFLM_PATH)/kernels/sub.cc \
$(TFLM_PATH)/kernels/sub_common.cc \
$(TFLM_PATH)/kernels/svdf.cc \
$(TFLM_PATH)/kernels/svdf_common.cc \
$(TFLM_PATH)/kernels/tanh.cc \
$(TFLM_PATH)/kernels/transpose.cc \
$(TFLM_PATH)/kernels/transpose_conv.cc \
$(TFLM_PATH)/kernels/unpack.cc \
$(TFLM_PATH)/kernels/var_handle.cc \
$(TFLM_PATH)/kernels/zeros_like.cc \
$(TFLM_PATH)/kernels/arc_mli/scratch_buffers.cc \
$(TFLM_PATH)/kernels/arc_mli/scratch_buf_mgr.cc \
$(TFLM_PATH)/kernels/arc_mli/mli_slicers.cc \
$(TFL_PATH)/experimental/microfrontend/lib/fft_util.cc \
$(TFL_PATH)/experimental/microfrontend/lib/fft.cc \
$(TFL_PATH)/experimental/microfrontend/lib/filterbank_util.c \
$(TFL_PATH)/experimental/microfrontend/lib/filterbank.c \
$(TFL_PATH)/experimental/microfrontend/lib/frontend_util.c \
$(TFL_PATH)/experimental/microfrontend/lib/frontend.c \
$(TFL_PATH)/experimental/microfrontend/lib/log_lut.c \
$(TFL_PATH)/experimental/microfrontend/lib/log_scale_util.c \
$(TFL_PATH)/experimental/microfrontend/lib/log_scale.c \
$(TFL_PATH)/experimental/microfrontend/lib/noise_reduction_util.c \
$(TFL_PATH)/experimental/microfrontend/lib/noise_reduction.c \
$(TFL_PATH)/experimental/microfrontend/lib/pcan_gain_control_util.c \
$(TFL_PATH)/experimental/microfrontend/lib/pcan_gain_control.c \
$(TFL_PATH)/experimental/microfrontend/lib/window_util.c \
$(TFL_PATH)/experimental/microfrontend/lib/window.c \
$(TFL_PATH)/experimental/microfrontend/lib/kiss_fft_int16.cc

OBJS := \
$(patsubst %.cc,%.o,$(patsubst %.c,%.o,$(SRCS)))

PROJECT_SRCS := \
$(wildcard $(SRC_PATH)/*.cc) \
$(wildcard $(SRC_PATH)/*.c)  \
$(wildcard $(SRC_PATH)/model/*.cc)  \
$(wildcard $(SRC_PATH)/model/*.c) 

PROJECT_OBJS := \
$(patsubst %.cc,%.o,$(patsubst %.c,%.o,$(PROJECT_SRCS)))

#=============================================================
# Applications settings
#=============================================================
MLI_PATH = $(HIMAX_PATH)/third_party/arc_mli_package/bin/himax_arcem9d_r16/release
DEPEND_PATH = $(HIMAX_PATH)/third_party/mw_gnu_dependencies/gnu_depend_lib
SDK_PATH = $(HIMAX_PATH)/himax_we1_sdk

CXXFLAGS += \
-fno-rtti \
-fpermissive \
-fno-exceptions \
-fno-threadsafe-statics \
-fno-unwind-tables \
-ffunction-sections \
-fdata-sections \
-fmessage-length=0 \
-DTF_LITE_STATIC_MEMORY \
-DTF_LITE_DISABLE_X86_NEON \
-O3 \
-Wsign-compare \
-Wdouble-promotion \
-Wshadow \
-Wunused-variable \
-Wmissing-field-initializers \
-Wunused-function \
-Wswitch \
-Wvla \
-Wall \
-Wextra \
-Wstrict-aliasing \
-Wno-unused-parameter \
-DREDUCE_CODESIZE \
-mxy \
-include $(SDK_PATH)/core_config.h \
-mcpu=em4_fpus \
-mlittle-endian \
-mcode-density \
-mdiv-rem \
-mswap \
-mnorm \
-mmpy-option=6 \
-mbarrel-shifter \
-mfpu=fpus_all \
-I $(HIMAX_PATH) \
-I $(INC_PATH) \
-I $(INC_PATH)/model \
-I $(SDK_PATH) \
-I $(HIMAX_PATH)/third_party/gemmlowp \
-I $(HIMAX_PATH)/third_party/flatbuffers/include \
-I $(HIMAX_PATH)/third_party/ruy \
-I $(HIMAX_PATH)/third_party/arc_mli_package/include \
-I $(HIMAX_PATH)/third_party/arc_mli_package/include/api \
-I $(HIMAX_PATH)/third_party/kissfft \
-I $(TFLM_PATH)/tools/make/downloads/kissfft


CCFLAGS+= \
-mcpu=em4_fpus \
-mlittle-endian \
-mcode-density \
-mdiv-rem \
-mswap \
-mnorm \
-mmpy-option=6 \
-mbarrel-shifter \
-mfpu=fpus_all \
-fno-unwind-tables \
-ffunction-sections \
-fdata-sections \
-fmessage-length=0 \
-DTF_LITE_STATIC_MEMORY \
-DTF_LITE_DISABLE_X86_NEON \
-O3 \
-DREDUCE_CODESIZE \
-mxy \
-include $(SDK_PATH)/core_config.h \
-I $(HIMAX_PATH) \
-I $(INC_PATH) \
-I $(SDK_PATH) \
-I $(HIMAX_PATH)/third_party/gemmlowp \
-I $(HIMAX_PATH)/third_party/flatbuffers/include \
-I $(HIMAX_PATH)/third_party/ruy \
-I $(HIMAX_PATH)/third_party/arc_mli_package/include \
-I $(HIMAX_PATH)/third_party/arc_mli_package/include/api \
-I $(TFLM_PATH)/tools/make/downloads/kissfft

LDFLAGS +=  -Wl,-lmli -Wl,-lmwdepend -Wl,-marcv2elfx -Wl,-Map=memory.map -Wl,--strip-debug -Wl,--stats,--gc-sections -Wl,--cref \
-L$(MLI_PATH) \
-L$(DEPEND_PATH) \
-L .\
-Wl,--start-group \
$(SDK_PATH)/libcpuarc.a \
$(SDK_PATH)/libbss.a \
$(SDK_PATH)/libboard_socket.a \
$(SDK_PATH)/libboard_open_socket.a \
$(SDK_PATH)/liblibcommon.a \
$(SDK_PATH)/liblibaudio.a \
$(SDK_PATH)/liblibsecurity.a \
$(SDK_PATH)/liblibsensordp.a \
$(SDK_PATH)/liblibtflm.a \
-Wl,--end-group \


#=============================================================
# Common rules
#=============================================================
.PHONY: project

%.o: %.cc
	$(CXX) $(CXXFLAGS) $(EXT_CFLAGS) $(INCLUDES) -c $< -o $@

%.o: %.c
	$(CC) $(CCFLAGS) $(EXT_CFLAGS) $(INCLUDES) -c $< -o $@


#=================================================================
# Global rules
#=================================================================
output: MAP_NAME = output
output: output.elf

output.elf : $(OBJS) $(PROJECT_OBJS) 
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(PROJECT_OBJS) $(LDFLAGS)

clean:
	@echo 'cleaning'
	-@$(RM) $(PROJECT_OBJS)
	-@$(RM) *.elf
	-@$(RM) *.map
	-@$(RM) *.img
clean_all:
	@echo 'cleaning'
	-@$(RM) $(OBJS) $(PROJECT_OBJS)
	-@$(RM) *.elf
	-@$(RM) *.map
	-@$(RM) *.img
	
ifeq ($(ARC_TOOLCHAIN), mwdt)
flash:
	@export PATH=$(shell pwd)/$(GEN_TOOL_DIR)/:$$PATH && \
	cp output.elf output.map $(GEN_TOOL_DIR) && \
	cd $(GEN_TOOL_DIR) && \
	$(GEN_TOOL_NAME) -e output.elf -m output.map -o output_mwdt.img -s 1024 && \
	cp output_mwdt.img $(PROJECT_PATH) && \
	rm output.elf output.map output_mwdt.img

else ifeq ($(ARC_TOOLCHAIN), gnu)
flash:
	@export PATH=$(shell pwd)/$(GEN_TOOL_DIR)/:$$PATH && \
	cp output.elf $(GEN_TOOL_DIR) && \
	cd $(GEN_TOOL_DIR) && \
	$(GEN_TOOL_NAME) -e output.elf -s 1024 -o output_gnu.img &&\
	cp output_gnu.img $(PROJECT_PATH) && \
	rm output.elf output_gnu.img
endif 	
