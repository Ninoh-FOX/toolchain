LOWPOWD_DIR := $(BUILD_DIR)/lowpowd

$(LOWPOWD_DIR)/lowpowd.c:
	mkdir -p $(LOWPOWD_DIR)
	cp package/lowpowd/lowpowd.c $@

$(LOWPOWD_DIR)/lowpowd: $(LOWPOWD_DIR)/lowpowd.c
	$(TARGET_CC) $(TARGET_CFLAGS) $^ -o $@

$(TARGET_DIR)/usr/sbin/lowpowd: $(LOWPOWD_DIR)/lowpowd
	$(INSTALL) -m 755 $^ $@

$(TARGET_DIR)/etc/init.d/S99lowpowd.sh: package/lowpowd/S99lowpowd.sh
	$(INSTALL) -m 755 $^ $@

.PHONY: lowpowd
lowpowd: $(TARGET_DIR)/usr/sbin/lowpowd $(TARGET_DIR)/etc/init.d/S99lowpowd.sh

ifeq ($(BR2_PACKAGE_LOWPOWD),y)
TARGETS += lowpowd
endif
