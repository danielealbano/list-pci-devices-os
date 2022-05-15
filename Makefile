CFLAGS=-ffreestanding -O2 -Wall -Wextra
LDFLAGS=-ffreestanding -O2 -nostdlib -lgcc
TARGET=build/myos
SRC_DIR=src
OBJ_DIR=obj

SRCS=$(wildcard $(SRC_DIR)/*.c)
OBJS=$(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

all: $(TARGET)

$(OBJ_DIR):
	mkdir $(OBJ_DIR) || true

$(OBJ_DIR)/boot.o: $(OBJ_DIR)
	as -32 src/boot.S -o $(OBJ_DIR)/boot.o

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	gcc -m32 -o $@ -c $^ -std=gnu99 -I include $(CFLAGS)

$(TARGET): $(BUILD_DIR) $(OBJ_DIR)/boot.o $(OBJS)
	(mkdir $(shell dirname $(TARGET)) || true) 2>/dev/null
	gcc -m32 -T linker.ld -o $(TARGET) $(OBJ_DIR)/boot.o $(OBJS) $(LDFLAGS)

$(TARGET).iso: $(TARGET) .phony
	mkdir $(BUILD_DIR)isodir || true
	mkdir $(BUILD_DIR)isodir/boot || true
	cp $(TARGET) $(BUILD_DIR)isodir/boot
	grub-mkrescue -o $(TARGET).iso $(BUILD_DIR)isodir

$(TARGET).img: $(TARGET) .phony
	mkdir usbdir || true
	dd if=/dev/zero of=$(TARGET).img bs=20M count=1
	$(eval LOOPDEV=$(shell losetup -f))
	$(eval USBDIR=$(realpath $(BUILD_DIR)/usbdir))
	sudo losetup $(LOOPDEV) $(TARGET).img
	sudo mkfs.vfat $(LOOPDEV) -v
	sudo mount -t vfat $(LOOPDEV) $(USBDIR)
	sudo grub-install --no-floppy --force --root-directory=$(USBDIR) $(LOOPDEV)
	sudo cp $(TARGET) $(USBDIR)/boot/
	sudo cp grub-usb.cfg $(USBDIR)/boot/grub/grub.cfg
	sudo sync
	sudo umount $(USBDIR)
	sudo losetup -d $(LOOPDEV)

qemu-cd: $(TARGET).iso
	qemu-system-i386 -M q35 -cdrom $(TARGET).iso -serial stdio

qemu-hd: $(TARGET).img
	qemu-system-i386 -M q35 -hda $(TARGET).img -serial stdio

qemu: $(TARGET)
	qemu-system-i386 -M q35 -kernel $(TARGET) -serial stdio

clean:
	rm $(OBJ_DIR)/boot.o $(OBJS) $(TARGET) $(TARGET).iso $(TARGET).img || true

.phony:
