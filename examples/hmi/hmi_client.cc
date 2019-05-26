#include <iostream>
#include <cstdint>
#include <cstring>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string.hpp>
#include <vector>

extern "C"
{

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <modbus.h>   
}

const uint16_t UT_REGISTERS_ADDRESS = 10;
const uint16_t UT_REGISTERS_NB = 0x3;
const uint16_t UT_REGISTERS_NB_MAX = 0x20;
const uint16_t UT_REGISTERS_TAB[] = { 0x022B, 0x0001, 0x0064 };

#define BUG_REPORT(_cond, _format, _args ...) \
    printf("\nLine %d: assertion error for '%s': " _format "\n", __LINE__, # _cond, ## _args)

#define ASSERT_TRUE(_cond, _format, __args...) {  \
    if (_cond) {                                  \
        printf("OK\n");                           \
    } else {                                      \
        BUG_REPORT(_cond, _format, ## __args);    \
        goto close;                               \
    }                                             \
};

int equal_dword(uint16_t *tab_reg, const uint32_t value) {
    return ((tab_reg[0] == (value >> 16)) && (tab_reg[1] == (value & 0xFFFF)));
}


int main(int argc, char **argv) {
    modbus_mapping_t mb_mapping;
    int s, rc, header_length, i;
    uint8_t *tab_rp_bits = NULL;
    uint16_t *tab_rp_registers = NULL;

    string ip_address = "127.0.0.1";
    int port_no = 1502;
    int nb_points = 10000;

    tab_rp_bits = (uint8_t *) malloc(nb_points * sizeof(uint8_t));
    memset(tab_rp_bits, 0, nb_points * sizeof(uint8_t));
    tab_rp_registers = (uint16_t *) malloc((nb_points /4) * sizeof(uint16_t));
    memset(tab_rp_registers, 0, (nb_points/4) * sizeof(uint16_t));

    ctx = modbus_new_tcp(ip_address.c_str(), port_no);
    modbus_set_debug(ctx, TRUE);
    modbus_set_error_recovery(ctx,
                              MODBUS_ERROR_RECOVERY_LINK |
                              MODBUS_ERROR_RECOVERY_PROTOCOL);

    if (modbus_connect(ctx) == -1) {
        fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
        modbus_free(ctx);
        return 0;
    }

    std::cout << "\nTEST WRITE/READ:\n";

    /** COIL BITS **/

    /* Single */
    rc = modbus_write_bit(ctx, UT_BITS_ADDRESS, ON);
    std::cout << "1/2 modbus_write_bit: " << std::endl;
    ASSERT_TRUE(rc == 1, "");

    rc = modbus_read_bits(ctx, UT_BITS_ADDRESS, 1, tab_rp_bits);
    std::cout << "2/2 modbus_read_bits: " << std::endl;
    ASSERT_TRUE(rc == 1, "FAILED (nb points %d)\n", rc);
    ASSERT_TRUE(tab_rp_bits[0] == ON, "FAILED (%0X != %0X)\n",
                tab_rp_bits[0], ON);

    /* Single register */
    rc = modbus_write_register(ctx, UT_REGISTERS_ADDRESS, 0x1234);
    printf("1/2 modbus_write_register: ");
    ASSERT_TRUE(rc == 1, "");

    rc = modbus_read_registers(ctx, UT_REGISTERS_ADDRESS,
                               1, tab_rp_registers);
    printf("2/2 modbus_read_registers: ");
    ASSERT_TRUE(rc == 1, "FAILED (nb points %d)\n", rc);
    ASSERT_TRUE(tab_rp_registers[0] == 0x1234, "FAILED (%0X != %0X)\n",
                tab_rp_registers[0], 0x1234);
    /* End of single register */

    /* Many registers */
    rc = modbus_write_registers(ctx, UT_REGISTERS_ADDRESS,
                                UT_REGISTERS_NB, UT_REGISTERS_TAB);
    printf("1/5 modbus_write_registers: ");
    ASSERT_TRUE(rc == UT_REGISTERS_NB, "");

    rc = modbus_read_registers(ctx, UT_REGISTERS_ADDRESS,
                               UT_REGISTERS_NB, tab_rp_registers);
    printf("2/5 modbus_read_registers: ");
    ASSERT_TRUE(rc == UT_REGISTERS_NB, "FAILED (nb points %d)\n", rc);

    for (i=0; i < UT_REGISTERS_NB; i++) {
        ASSERT_TRUE(tab_rp_registers[i] == UT_REGISTERS_TAB[i],
                    "FAILED (%0X != %0X)\n",
                    tab_rp_registers[i], UT_REGISTERS_TAB[i]);
    }

    rc = modbus_read_registers(ctx, UT_REGISTERS_ADDRESS,
                               0, tab_rp_registers);
    printf("3/5 modbus_read_registers (0): ");
    ASSERT_TRUE(rc == -1, "FAILED (nb_points %d)\n", rc);
    memset(tab_rp_registers, 0, (nb_points/4) * sizeof(uint16_t));

    /* Write registers to zero from tab_rp_registers and store read registers
       into tab_rp_registers. So the read registers must set to 0, except the
       first one because there is an offset of 1 register on write. */
    rc = modbus_write_and_read_registers(ctx,
                                         UT_REGISTERS_ADDRESS + 1,
                                         UT_REGISTERS_NB - 1,
                                         tab_rp_registers,
                                         UT_REGISTERS_ADDRESS,
                                         UT_REGISTERS_NB,
                                         tab_rp_registers);
    printf("4/5 modbus_write_and_read_registers: ");
    ASSERT_TRUE(rc == UT_REGISTERS_NB, "FAILED (nb points %d != %d)\n",
                rc, UT_REGISTERS_NB);

    ASSERT_TRUE(tab_rp_registers[0] == UT_REGISTERS_TAB[0],
                "FAILED (%0X != %0X)\n",
                tab_rp_registers[0], UT_REGISTERS_TAB[0]);

    for (i=1; i < UT_REGISTERS_NB; i++) {
        ASSERT_TRUE(tab_rp_registers[i] == 0, "FAILED (%0X != %0X)\n",
                    tab_rp_registers[i], 0);
    }
    /* End of many registers */


    std::cout << "All Tests PASS. Stopping HMI ..." << std::endl;
    modbus_free(ctx);

    return 0;
}