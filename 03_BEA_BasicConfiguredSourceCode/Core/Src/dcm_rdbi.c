
/*********************************************************/
/*********BOSCH BEA PROGRAM SKELETON DEMO CODE************/
/*********************************************************/

#include "dcm_rdbi.h"
extern uint8_t  REQ_BUFFER[];
extern UART_HandleTypeDef huart3;
extern CAN_TxHeaderTypeDef CAN1_pHeader;
extern uint16_t g_TemperatureSensorRawValue_u16[1];

void SID_22_Practice(void)
{
    uint8_t did_h = REQ_BUFFER[4];
    uint8_t did_l = REQ_BUFFER[5];

    if (did_h == 0x01 && did_l == 0x23)
    {
        /* DID 0x0123: Doc gia tri CAN ID hien tai */
        uint16_t canid = (uint16_t)(CAN1_pHeader.StdId & 0x7FF);
        uint8_t resp[] = {
            0x0F, 0xFF, 0xF0,           /* SOF */
            0x62,                        /* Positive Response SID (0x22 + 0x40) */
            did_h, did_l,               /* Echo DID */
            (uint8_t)(canid >> 8),      /* Data High */
            (uint8_t)(canid & 0xFF),    /* Data Low */
            0xF0, 0x00, 0x0F            /* EOF */
        };
        HAL_UART_Transmit(&huart3, resp, sizeof(resp), HAL_MAX_DELAY);
    }
    else if (did_h == 0x01 && did_l == 0x24)
    {
        /* DID 0x0124: Doc gia tri ADC (cam bien nhiet do) */
        uint16_t adc = g_TemperatureSensorRawValue_u16[0];
        uint8_t resp[] = {
            0x0F, 0xFF, 0xF0,           /* SOF */
            0x62,                        /* Positive Response SID */
            did_h, did_l,               /* Echo DID */
            (uint8_t)(adc >> 8),        /* Data High */
            (uint8_t)(adc & 0xFF),      /* Data Low */
            0xF0, 0x00, 0x0F            /* EOF */
        };
        HAL_UART_Transmit(&huart3, resp, sizeof(resp), HAL_MAX_DELAY);
    }
    else
    {
        /* DID khong duoc ho tro -> Negative Response NRC 0x31 */
        uint8_t neg[] = {
            0x0F, 0xFF, 0xF0,   /* SOF */
            0x7F,               /* Negative Response SID */
            0x22,               /* SID bi loi */
            0x31,               /* NRC: requestOutOfRange */
            0xF0, 0x00, 0x0F    /* EOF */
        };
        HAL_UART_Transmit(&huart3, neg, sizeof(neg), HAL_MAX_DELAY);
    }
}

