#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xil_io.h"
#include "xuartps.h"
#include "xbram.h"
#include "xparameters.h"
#include <unistd.h>

#define NUM_INPUTS          100     // pixels or values you want to store
#define BYTES_PR_INPUT      4       // 32-bit word
#define BASE_ADDR           XPAR_AXI_BRAM_CTRL_0_S_AXI_BASEADDR

XBram           x_bram;
XBram_Config   *px_config;

XUartPs         Uart_PS_0;
XUartPs_Config *Config_0;

uint8_t ucAXIInit(void);
int     xuartps_init(void);

int main()
{
    init_platform();

    xil_printf("\n\r===== Cora-Z7 BRAM Loader =====\n\r");
    xil_printf("Initializing peripherals...\n\r");

    if (ucAXIInit() != XST_SUCCESS) {
        xil_printf("BRAM init FAILED!\n\r");
        return -1;
    }

    if (xuartps_init() != XST_SUCCESS) {
        xil_printf("UART init FAILED!\n\r");
        return -1;
    }

    xil_printf("Initialization OK.\n\r");
    xil_printf("Waiting for %d x 32-bit values...\n\r\n\r", NUM_INPUTS);

    uint8_t  BufferPtr_rx[NUM_INPUTS * BYTES_PR_INPUT] = {0};
    uint32_t tempInt;

    while (1)
    {
        int received = 0;

        // ----------- receive 4*NUM_INPUTS bytes over UART --------------
        while (received < NUM_INPUTS * BYTES_PR_INPUT)
        {
            BufferPtr_rx[received] =
                XUartPs_RecvByte(XPAR_XUARTPS_0_BASEADDR);
            received++;
        }

        xil_printf("\n\rReceived frame. Writing to BRAM...\n\r");

        // ----------- convert to 32-bit words + write BRAM ------------
        for (int i = 0; i < NUM_INPUTS; i++)
        {
            tempInt =
                ((BufferPtr_rx[i*4+3] << 24) |
                 (BufferPtr_rx[i*4+2] << 16) |
                 (BufferPtr_rx[i*4+1] << 8 ) |
                  BufferPtr_rx[i*4+0]);

            // write to BRAM
            Xil_Out32(BASE_ADDR + (i * 4), tempInt);

            xil_printf("BRAM[%03d] = 0x%08lx\n\r", i, (unsigned long)tempInt);
        }

        xil_printf("Done. Ready for next frame.\n\r");
    }

    cleanup_platform();
    return 0;
}


// =========================================================
// BRAM INIT
// =========================================================
uint8_t ucAXIInit(void)
{
    px_config = XBram_LookupConfig(XPAR_BRAM_0_DEVICE_ID);
    if (px_config == NULL)
        return XST_FAILURE;

    if (XBram_CfgInitialize(&x_bram,
                            px_config,
                            px_config->CtrlBaseAddress) != XST_SUCCESS)
        return XST_FAILURE;

    return XST_SUCCESS;
}


// =========================================================
// UART INIT
// =========================================================
int xuartps_init(void)
{
    Config_0 = XUartPs_LookupConfig(XPAR_XUARTPS_0_DEVICE_ID);
    if (Config_0 == NULL)
        return XST_FAILURE;

    if (XUartPs_CfgInitialize(&Uart_PS_0,
                              Config_0,
                              Config_0->BaseAddress) != XST_SUCCESS)
        return XST_FAILURE;

    return XST_SUCCESS;
}
