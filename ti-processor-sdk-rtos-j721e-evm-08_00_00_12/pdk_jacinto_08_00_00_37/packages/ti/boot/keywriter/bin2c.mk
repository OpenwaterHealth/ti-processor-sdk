$(KEYWRITER_APP_DIR)/main.c: ti-fs-keywriter.h

ti-fs-keywriter.h: $(BIN2C_EXE)
	$(BIN2C_EXE) $(SIGNED_SYSFW_BIN) ti-fs-keywriter.h TIFS_KEYWRITER_BIN > $(KEYWRITER_APP_DIR)/ti-fs-keywriter.h

$(BIN2C_EXE):
	gcc $(SCI_CLIENT_TOOLS_BIN2C_DIR)/bin2c.c -o $(BIN2C_EXE)

