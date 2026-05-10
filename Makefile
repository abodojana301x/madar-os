# Layer 7 Build System (Apps added)

# Purpose: تجميع النواة مع Drivers (Layer 3)

# Try to use cross-compiler, fallback to native
CC = x86_64-elf-gcc
AS = x86_64-elf-as
LD = x86_64-elf-ld

# Check if cross-compiler exists
ifeq (,$(shell which $(CC)))
    CC = gcc
    AS = as
    LD = ld
endif

CFLAGS = -ffreestanding -fno-stack-protector -fno-pic \
         -mno-red-zone -mgeneral-regs-only \
         -Wall -Wextra -Werror -std=gnu11 \
         -Iinclude -I drivers -I compat -I gui -I. -fno-builtin

ASFLAGS = -g --64

LDFLAGS = -T linker.ld -nostdlib -z max-page-size=0x1000

SRCS = kernel/boot.s kernel/main.c kernel/memory.c kernel/vga.c kernel/serial.c kernel/idt.c kernel/pic.c kernel/kprintf.c kernel/string.c \
       kernel/heap.c kernel/spinlock.c kernel/process.c kernel/stack.c kernel/thread.c kernel/panic.c kernel/kernel_diag.c \
       kernel/scheduler.c kernel/timer.c kernel/syscall.c \
       kernel/sync.c kernel/atomic.c kernel/stress_test.c kernel/acpi.c kernel/layer2_report.c kernel/layer4_report.c \
       kernel/boot_diag.c kernel/crash_handler.c kernel/vmm.c \
       drivers/pci.c drivers/fb.c drivers/kbd.c drivers/mouse.c drivers/ata.c drivers/rtl8139.c \
       drivers/irq_manager.c drivers/drivers.c \
       services/ring_buffer.c services/vfs.c services/ramdisk.c services/ipc.c services/signal.c \
       services/security.c services/uid_table.c services/disk_io.c services/fat32.c services/fat_dir.c \
       services/net_utils.c services/arp.c services/ip.c services/icmp.c services/tcp.c services/udp.c \
       services/init.c services/service_mgr.c services/config_parser.c services/syscall_layer4.c \
       compat/pe_parser.c compat/pe_loader.c compat/pe_validator.c compat/mem_mapper.c compat/stack_init.c \
       compat/api_log.c compat/win32_stubs.c compat/syscall_bridge.c compat/seh_bridge.c compat/exception_mapper.c \
       compat/safe_exit.c compat/pe_threads.c compat/tls_manager.c compat/sync_mapper.c compat/exe_sandbox.c \
       compat/compatd.c compat/test_runner.c compat/compat_process.c compat/import_resolver.c compat/module_registry.c \
  	gui/gfx_backend.c gui/double_buffer.c gui/vsync_timer.c gui/font_engine.c gui/utf8_parser.c gui/glyph_cache.c \
	gui/wm_core.c gui/window_list.c gui/wm_events.c gui/event_loop.c gui/input_router.c gui/focus_manager.c \
	gui/keyboard_system.c gui/input_system.c gui/input_poller.c \
 	gui/widgets.c gui/taskbar.c gui/style.c gui/gui_daemon.c gui/perf_monitor.c gui/stress_test.c \
 	gui/ui_toolkit.c gui/theme.c gui/gfx_utils.c gui/rtl_support.c gui/animation.c gui/widgets_modern.c \
	desktop/shell.c desktop/panel.c desktop/start_menu.c desktop/app_registry.c desktop/session.c \
	apps/shell.c apps/lang_detect.c apps/builtins.c \
	apps/fileman.c apps/vfs_browser.c apps/clipboard_mgr.c apps/path_bar.c apps/icon_renderer.c \
	apps/settings.c apps/config_parser.c apps/resolution_mgr.c apps/net_config.c apps/exe_toggle.c \
	apps/http_client.c apps/tcp_socket.c apps/html_min_parser.c apps/web_view.c \
	apps/taskmgr.c apps/process_list.c apps/resource_monitor.c apps/launcher.c apps/start_menu.c apps/kill_handler.c \
	system/boot_sequence.c system/login_screen.c system/init_gui.c system/stress_suite.c system/release_validator.c
OBJS = $(SRCS:.c=.o)
OBJS := $(OBJS:.s=.o)

TARGET = kernel.bin
BUILD_DIR = build
ISO_ROOT = $(BUILD_DIR)/iso_root
ISO = $(BUILD_DIR)/madar-os.iso
MAX_KERNEL_SIZE ?= 204800

.PHONY: all clean run run-gui run-vnc debug test

all: $(TARGET)

$(TARGET): $(OBJS)
	@echo "🔗 Linking $(TARGET)..."
	$(LD) $(LDFLAGS) -o $@ $^
	@echo "✅ Build complete. Size: $$(stat -c%s $(TARGET)) bytes"

%.o: %.c
	@echo "📝 Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	@echo "🔧 Assembling $<..."
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) kernel.elf
	rm -rf $(BUILD_DIR)

$(ISO): $(TARGET) boot/grub/grub.cfg
	@mkdir -p $(ISO_ROOT)/boot/grub
	@cp $(TARGET) $(ISO_ROOT)/boot/kernel.bin
	@cp boot/grub/grub.cfg $(ISO_ROOT)/boot/grub/grub.cfg
	@echo "📦 Creating bootable ISO..."
	@grub-mkrescue -o $(ISO) $(ISO_ROOT) >/dev/null

run: $(ISO)
	qemu-system-x86_64 -machine pc -m 256M -serial stdio -display none \
		-cdrom $(ISO) -boot d -no-reboot -no-shutdown

run-gui: $(ISO)
	@if [ -n "$$DISPLAY" ]; then \
		echo "🖥️  Launching GUI with GTK display..."; \
		qemu-system-x86_64 -machine pc -m 256M \
			-cdrom $(ISO) -boot d -no-reboot -no-shutdown -display gtk; \
	else \
		echo "⚠️  No DISPLAY detected. Falling back to VNC on localhost:5901 (:1)."; \
		echo "   In Codespaces: forward port 5901 and open with a VNC client."; \
		qemu-system-x86_64 -machine pc -m 256M \
			-cdrom $(ISO) -boot d -no-reboot -no-shutdown -display vnc=127.0.0.1:1; \
	fi

run-vnc: $(ISO)
	@echo "🖥️  Starting QEMU with VNC on localhost:5901 (:1)"
	@echo "   In Codespaces: forward port 5901 and connect using a VNC client."
	qemu-system-x86_64 -machine pc -m 256M \
		-cdrom $(ISO) -boot d -no-reboot -no-shutdown -display vnc=127.0.0.1:1

debug: $(ISO)
	qemu-system-x86_64 -machine pc -m 256M -serial stdio -display none \
		-cdrom $(ISO) -boot d -s -S -no-reboot -no-shutdown

test: $(TARGET)
	@echo "🧪 Running basic tests..."
	@bytes=$$(stat -c%s $(TARGET)); \
		if [ $$bytes -lt $(MAX_KERNEL_SIZE) ]; then \
			echo "✅ Size OK (<$$(($(MAX_KERNEL_SIZE)/1024))KB)"; \
		else \
			echo "❌ Size too large: $$bytes bytes"; \
			exit 1; \
		fi
	@echo "✅ All build tests passed"

layer3_test: $(TARGET)
	@echo "🧪 Layer 3 Driver Tests"
	@echo "Run: make run"
