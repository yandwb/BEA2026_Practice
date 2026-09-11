/*********************************************************/
/*********BOSCH BEA PROGRAM SKELETON DEMO CODE************/
/*********************************************************/

#include "dcm_seca.h"
extern uint8_t  REQ_BUFFER[];
extern UART_HandleTypeDef huart3;

/* Luu seed de kiem tra key sau */
static uint8_t g_seed[4] = {0};
uint8_t g_is_unlocked = 0;

void SID_27_Practice(void)
{
    uint8_t sub_func = REQ_BUFFER[4];

    if (sub_func == 0x01)
    {
        /* Tao seed tu tick timer */
        uint32_t tick = HAL_GetTick();
        g_seed[0] = (tick >> 24) & 0xFF;
        g_seed[1] = (tick >> 16) & 0xFF;
        g_seed[2] = (tick >>  8) & 0xFF;
        g_seed[3] = (tick >>  0) & 0xFF;

        uint8_t resp[] = {
            0x0F, 0xFF, 0xF0,          /* SOF */
            0x67, 0x01,                /* Positive Response + sub_func */
            g_seed[0], g_seed[1],
            g_seed[2], g_seed[3],
            0xF0, 0x00, 0x0F           /* EOF */
        };
        HAL_UART_Transmit(&huart3, resp, sizeof(resp), HAL_MAX_DELAY);
    }
    else if (sub_func == 0x02)
    {
        /* Nhan key tu PC: REQ_BUFFER[5..8] */
        uint8_t received_key[4];
        received_key[0] = REQ_BUFFER[5];
        received_key[1] = REQ_BUFFER[6];
        received_key[2] = REQ_BUFFER[7];
        received_key[3] = REQ_BUFFER[8];

        /* Tinh expected key tu seed da luu */
        uint8_t expected_key[4];
        expected_key[0] = g_seed[0] ^ g_seed[1];
        expected_key[1] = (g_seed[1] + g_seed[2]) & 0xFF;
        expected_key[2] = g_seed[2] ^ g_seed[3];
        expected_key[3] = (g_seed[3] + g_seed[0]) & 0xFF;

        /* So sanh key */
        if (received_key[0] == expected_key[0] &&
            received_key[1] == expected_key[1] &&
            received_key[2] == expected_key[2] &&
            received_key[3] == expected_key[3])
        {
            /* Dung key -> mo khoa, bat LED PB0 va set cờ */
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
            g_is_unlocked = 1;
            uint8_t resp[] = {
                0x0F, 0xFF, 0xF0,  /* SOF */
                0x67, 0x02,        /* Positive Response */
                0xF0, 0x00, 0x0F   /* EOF */
            };
            HAL_UART_Transmit(&huart3, resp, sizeof(resp), HAL_MAX_DELAY);
        }
        else
        {
            /* Sai key -> NRC 0x35 (invalidKey) */
            uint8_t neg[] = {
                0x0F, 0xFF, 0xF0,  /* SOF */
                0x7F, 0x27, 0x35,  /* NRS + SID + NRC */
                0xF0, 0x00, 0x0F   /* EOF */
            };
            HAL_UART_Transmit(&huart3, neg, sizeof(neg), HAL_MAX_DELAY);
        }
    }

}
