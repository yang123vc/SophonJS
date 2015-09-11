TARGET_SUFFIX:=$(TARGET)$(HOST_EXE_SUFFIX)
TARGET_FULL:=$(HOST_OUT)/$(TARGET_SUFFIX)

all: $(TARGET_FULL)

OBJS:=$(foreach src,$(SRCS),$(HOST_OUT)/$(dir $(src))$(TARGET_SUFFIX)-$(basename $(notdir $(src))).o)

$(foreach src,$(SRCS),$(eval $(call build_host_obj,$(src),$(TARGET_SUFFIX),$(CFLAGS))))

$(TARGET_FULL): BUILD_LDFLAGS:=$(HOST_LDFLAGS)  $(GLOBAL_HOST_LDFLAGS)
$(TARGET_FULL): $(OBJS)
	$(Q)$(INFO) HOST CC '->' $@
	$(Q)$(HOST_CC) -o $@ $^ $(BUILD_LDFLAGS)

CLEAN_TARGETS+=$(TARGET_FULL) $(OBJS)

include build/clear_env.mk
