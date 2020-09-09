#include "iap_patch.h"
#include "port.h"
#include "usart.h"

typedef int (*Func)(int);

static int MyAdd(int i) {
	return i + 8;
}

static int MyHook(int i) {
	return i + 5;
}

static void jumpExe(uint32_t addr) {
	__asm {
		//ldr pc, addr
	};
}

static void write_val1(uint32_t addr) {
	printf("write_val1\n");
	uint8_t buf[] = {'a', 1, 2, 3, 4, 5, 6, 7, 'z'};
	int ret = flash_port_write(addr, buf, sizeof(buf));
	uint8_t rbuf[20] = {0};
	flash_port_read(addr, rbuf, sizeof(buf));
	printf("ret:%d data: %s\n", ret, rbuf);
}

static void write_val2(uint32_t addr) {
	printf("write_val2\n");
	uint8_t buf[] = {'x', 7, 6, 5, 'y'};
	flash_port_write(addr, buf, sizeof(buf));
	uint8_t rbuf[20] = {0};
	flash_port_read(addr, rbuf, sizeof(buf));
	for (int i = 0; i < sizeof(buf); i++) {
		if (buf[i] != rbuf[i]) {
			printf("unequal at: %d\n", i);
		}
	}
	printf("data: %s\n", rbuf);
}

static uint8_t data[2200]; // 2k
static void write_more_bytes() {
	// wirte buffer lager than 2k to flash
	uint32_t base_addr = 0x08020000;
	int sz = 2100;
	int offset = 5;
	// verify
	// 2k = 8 * 16 * 16
	for (int i = 0; i < sz; i++) {
		data[i] = i % 256;
	}
	int ret = flash_port_write(base_addr + offset, data, sz);
	printf("write res: %d\n", ret);
	// 
	for (int i = 0; i < sz; i++) {
		uint8_t val = *(vu8*)(base_addr + offset + i);
		if (val != (i % 256)) {
			printf("Bug: val is not equal at: 0x%p %d\n", (void *) (base_addr + offset + i), i);
		}
	}
}

void doHook(void *func, void *newAddr) {
	char *f = (char *)func;
	*(long *)&f[0] = 0xe51ff004; // ldr pc, addr
	*(long *)&f[4] = (long)newAddr;
}

static void write_patch_data() {
	uint32_t base_addr = 0x08020000;
	// 
	uint32_t to_read = (uint32_t) (MyAdd) - 1;
	uint8_t rb[50];
	flash_port_read(to_read, rb, sizeof(rb));
	flash_port_write(base_addr, rb, sizeof(rb));
	
	Func exp = (Func) MyAdd;
	int r1 = exp(0);
	Func raw = (Func) (base_addr + 1);  // enter thumb mode
	int r2 = raw(0);
	printf("call before: %d\n", r2);
	
	union {
		uint8_t c[4];
		uint32_t addr;
	} jpaddr;
	
	uint32_t addr = (uint32_t) MyHook;
	jpaddr.addr = addr;
	uint8_t abuf[4] = {0};
	abuf[0] = jpaddr.c[0];
	abuf[1] = jpaddr.c[1];
	abuf[2] = jpaddr.c[2];
	abuf[3] = jpaddr.c[3];
	// https://github.com/liuyx/inline-hook/blob/master/hooker/HookerThumb.cpp
	// http://shell-storm.org/online/Online-Assembler-and-Disassembler/?inst=ldr+pc%2C+%3D0x12345678&arch=arm&as_format=inline#assembly
	// ldr lr 0xe51ff004 \xe5\x1f\xf0\x04
	// 小端系统 thumb 指令
	uint8_t code [] = {
		 0xdf, 0xf8, 0x00, 0xf0, 
		abuf[0], abuf[1], abuf[2], abuf[3]
	};
	uint32_t cb[2];
	cb[0] = 0xf000f8df; //小端系统，存储的时候就会反转Byte，写的时候是按照顺序写的
	cb[1] = addr;
	// modify
	//flash_port_write(base_addr, (uint8_t*)cb, sizeof(cb));
	flash_port_write(base_addr, code, sizeof(code));
	//raw = (Func) (base_addr);
	// thumb指令
	int r3 = raw(0);
	printf("call after: %d\n", r3);
}

void test_flash_iap_write(int i) {
	// 0x20000 = 2 * (16 ^ 4) = 2 * 64 * 1024 = 128k
	
	printf("test_flash_iap_write: %d\n", i);
	uint32_t write_addr = 0x08020000;
	
	if (i == 1) {
		//write_more_bytes();
		write_patch_data();
		//write_val1(write_addr);
	} else if (i == 2) {
		
		//write_val2(write_addr);
	}
	
}
