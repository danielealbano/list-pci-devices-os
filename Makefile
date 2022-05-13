CFLAGS=-ffreestanding -O2 -Wall -Wextra
LDFLAGS=-ffreestanding -O2 -nostdlib -lgcc
TARGET=myos

SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

all: $(TARGET)

boot.o: 
	as -32 boot.S -o boot.o

%.o: %.c
	gcc -m32 -o $@ -c $^ -std=gnu99 $(CFLAGS)

$(TARGET): boot.o $(OBJS)
	gcc -m32 -T linker.ld -o $(TARGET) boot.o $(OBJS) $(LDFLAGS)

$(TARGET).iso: $(TARGET) .phony
	mkdir isodir || true
	mkdir isodir/boot || true
	cp $(TARGET) isodir/boot
	grub-mkrescue -o $(TARGET).iso isodir

$(TARGET).img: $(TARGET) .phony
	mkdir usbdir || true
	dd if=/dev/zero of=$(TARGET).img bs=20M count=1
	$(eval LOOPDEV=$(shell losetup -f))
	$(eval USBDIR=$(realpath usbdir))
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
	rm $(OBJS) $(TARGET) $(TARGET).iso $(TARGET).img || true

.phony:
