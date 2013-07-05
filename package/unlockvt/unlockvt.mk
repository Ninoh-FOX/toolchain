UNLOCKVT_DIR=$(BUILD_DIR)/unlockvt

$(UNLOCKVT_DIR)/unlockvt.c:
	rm -rf $(UNLOCKVT_DIR)
	mkdir $(UNLOCKVT_DIR)
	cp package/unlockvt/unlockvt.c $(UNLOCKVT_DIR)

$(UNLOCKVT_DIR)/unlockvt: $(UNLOCKVT_DIR)/unlockvt.c
	$(TARGET_CC) $(TARGET_CFLAGS) -s $^ -o $@

$(TARGET_DIR)/usr/sbin/unlockvt: $(UNLOCKVT_DIR)/unlockvt
	$(INSTALL) -m 755 $^ $@

unlockvt: $(TARGET_DIR)/usr/sbin/unlockvt

ifeq ($(BR2_PACKAGE_UNLOCKVT),y)
TARGETS += unlockvt
endif
