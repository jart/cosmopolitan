/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=8 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╠──────────────────────────────────────────────────────────────────────────────╣
│░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░█▀█░█▀█░▀█▀░█░█░█▀█░█░░░█░░░█░█░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░█▀█░█░▄░░█░░█░█░█▀█░█░░░█░░░▀█▀░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░▀░▀░▀▀▀░░▀░░▀▀▀░▀░▀░▀▀▀░▀▀▀░░▀░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░█▀█░█▀█░█▀█░▀█▀░█▀█░█▀█░█░░░█▀▀░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░█▀▀░█ █░██▀░░█░░█▀█░█▀█░█░░░█▀▀░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░▀░░░▀▀▀░▀░▀░░▀░░▀░▀░▀▀▀░▀▀▀░▀▀▀░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░█▀▀░█░█░█▀▀░█▀█░█░█░▀█▀░█▀█░█▀█░█░░█▀▀░░░░░░░░░░░░░░░░░░░░░░░░▄▄░░░▐█░░│
│░░░░░░░█▀▀░▄▀▄░█▀▀░█░▄░█░█░░█░░█▀█░█▀█░█░░█▀▀░░░░░░░░░░░░▄▄▄░░░▄██▄░░█▀░░░█░▄░│
│░░░░░░░▀▀▀░▀░▀░▀▀▀░▀▀▀░▀▀▀░░▀░░▀░▀░▀▀▀░▀▀░▀▀▀░░░░░░░░░░▄██▀█▌░██▄▄░░▐█▀▄░▐█▀░░│
│░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░▐█▀▀▌░░░▄▀▌░▌░█░▌░░▌░▌░░│
╠──────────────────────────────────────────────────────▌▀▄─▐──▀▄─▐▄─▐▄▐▄─▐▄─▐▄─│
│ αcτµαlly pδrταblε εxεcµταblε § telecommunications                            │
╚─────────────────────────────────────────────────────────────────────────────*/
#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_UART_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_UART_H_

/**
 * @fileoverview PC Serial Line Helpers (8250 UART 16550+).
 *
 * <p>These functions provide the baseline of PC telecommunications
 * support that doesn't require switching context or cpu mode.
 *
 * <p>Example use cases:
 * <ul>
 * <li>Qemu stdio
 * <li>Bare metal stdio
 * <li>FIPS-170-2 SSH alternative
 * <li>NIC for key-signing servers
 * </ul>
 *
 * @see www.lammertbies.nl/comm/info/serial-uart.html
 */

#define COM1 0x0 /* offset in pc bios data area with port number (0x400) */
#define COM2 0x2
#define COM3 0x4
#define COM4 0x6
#define IRQ3 0x0b            /* com2 interrupt number (irq3) */
#define IRQ4 0x0c            /* com1 interrupt number (irq4) */
#define UART_DLAB (1 << 7)   /* serial line conf mode bit */
#define UART_DLL 0           /* divisor latch register */
#define UART_DLM 1           /* divisor latch register */
#define UART_IIR 2           /* interrupt identification register */
#define UART_LCR 3           /* line control register */
#define UART_LSR 5           /* line status register */
#define UART_TTYDA (1 << 0)  /* data available (rx ready) */
#define UART_TTYOE (1 << 1)  /* overrun error */
#define UART_TTYPE (1 << 2)  /* parity error */
#define UART_TTYFE (1 << 3)  /* framing error */
#define UART_TTYBSR (1 << 4) /* break signal received */
#define UART_TTYTXR (1 << 5) /* serial thr empty (tx ready) */
#define UART_TTYIDL (1 << 6) /* serial thr empty and line idle */
#define UART_TTYEDF (1 << 7) /* erroneous data in fifo */

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void sinit(unsigned short *ports, unsigned count, const char *config);
void sputc(unsigned char ch, int port);
int sgetc(int port);
void sputs(const char *s, int port);
void sflush(unsigned short *ports, unsigned count);
unsigned char slsr(int port);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_UART_H_ */
