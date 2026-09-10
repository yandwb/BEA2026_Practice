
/*********************************************************/
/*********BOSCH BEA PROGRAM SKELETON DEMO CODE************/
/*********************************************************/

#include "dcm_wdbi.h"

extern uint8_t  REQ_BUFFER[];
extern UART_HandleTypeDef huart3;
extern CAN_TxHeaderTypeDef CAN1_pHeader;
void SID_2E_Practice(void) 
{
    uint8_t did_h = REQ_BUFFER[4];
    uint8_t did_l = REQ_BUFFER[5];

    extern uint8_t g_is_unlocked;
    /* Kiem tra Security Access */
    if (g_is_unlocked == 0)
    {
        uint8_t neg[] = {0x0F,0xFF,0xF0, 0x7F,0x2E,0x33, 0xF0,0x00,0x0F};
        HAL_UART_Transmit(&huart3, neg, sizeof(neg), HAL_MAX_DELAY);
        return;
    }

    if (did_h == 0x01 && did_l == 0x23)
    {
        /* Ghi CAN ID moi vao header (hieu luc sau IG cycle) */
        uint16_t new_id = ((uint16_t)REQ_BUFFER[6] << 8) | REQ_BUFFER[7];
        CAN1_pHeader.StdId = new_id & 0x7FF;

        uint8_t resp[] = {0x0F,0xFF,0xF0, 0x6E,did_h,did_l, 0xF0,0x00,0x0F};
        HAL_UART_Transmit(&huart3, resp, sizeof(resp), HAL_MAX_DELAY);
    }
    else
    {
        /* DID khong ho tro -> NRC 0x31 */
        uint8_t neg[] = {0x0F,0xFF,0xF0, 0x7F,0x2E,0x31, 0xF0,0x00,0x0F};
        HAL_UART_Transmit(&huart3, neg, sizeof(neg), HAL_MAX_DELAY);
    }
        
}
