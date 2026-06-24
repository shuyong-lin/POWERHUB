/******************************************************************************
* @file    usb_class_dual.c
* @author  MCD Application Team
* @brief   This file provides the high layer firmware functions to manage the
*          following functionalities of the USB CDC Class:
*           - Initialization and Configuration of high and low layer
*           - Enumeration as CDC Device (and enumeration for each implemented memory interface)
*           - OUT/IN data transfer
*           - Command IN transfer (class requests management)
*           - Error management
*
*          ===================================================================
*                                CDC Class Driver Description
*          ===================================================================
*           This driver manages the "Universal Serial Bus Class Definitions for Communications Devices
*           Revision 1.2 November 16, 2007" and the sub-protocol specification of "Universal Serial Bus
*           Communications Class Subclass Specification for PSTN Devices Revision 1.2 February 9, 2007"
*           This driver implements the following aspects of the specification:
*             - Device descriptor management
*             - Configuration descriptor management
*             - Enumeration as CDC device with 2 data endpoints (IN and OUT) and 1 command endpoint (IN)
*             - Requests management (as described in section 6.2 in specification)
*             - Abstract Control Model compliant
*             - Union Functional collection (using 1 IN endpoint for control)
*             - Data interface class
*
*           These aspects may be enriched or modified for a specific user application.
*
*            This driver doesn't implement the following aspects of the specification
*            (but it is possible to manage these features with some modifications on this driver):
*             - Any class-specific aspect relative to communication classes should be managed by user application.
*             - All communication classes other than PSTN are not managed
*
*  @endverbatim
*
*******************************************************************************
* @attention
*
* Copyright (c) 2015 STMicroelectronics.
* All rights reserved.
*
* This software component is licensed by ST under Ultimate Liberty license
* SLA0044, the "License"; You may not use this file except in compliance with
* the License. You may obtain a copy of the License at: www.st.com/SLA0044
*
******************************************************************************/

#include "usb_class_dual.h"
#include "usb_ctrlreq.h"

/* Private variables ---------------------------------------------------------*/
USBD_CDC_HandleTypeDef CDC1_Handle;
USBD_CDC_HandleTypeDef CDC2_Handle;

/* Private functions ---------------------------------------------------------*/
static uint8_t USBD_CDC_Init_FS(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_CDC_DeInit_FS(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_CDC_Setup_FS(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static uint8_t USBD_CDC_DataIn_FS(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_CDC_DataOut_FS(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_CDC_DataOut2_FS(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_CDC_DataIn2_FS(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_CDC_EP0_Rx_Ready_FS(USBD_HandleTypeDef *pdev);
static uint8_t USBD_CDC_DataInNotif_FS(USBD_HandleTypeDef *pdev, uint8_t epnum);

/* USB CDC Class callbacks structure */
static USBD_ClassTypeDef USBD_CDC_Class = {
    USBD_CDC_Init_FS,
    USBD_CDC_DeInit_FS,
    USBD_CDC_Setup_FS,
    NULL, /* EP0_TxSent */
    USBD_CDC_EP0_Rx_Ready_FS,
    USBD_CDC_DataIn_FS,
    USBD_CDC_DataOut_FS,
    USBD_CDC_DataInNotif_FS,
};

/* Public functions ---------------------------------------------------------*/
/**
* @brief  USBD_CDC_RegisterInterface
*         Register CDC interface
* @param  pdev: device instance
* @param  fops: CDC interface callback
* @retval status
*/
uint8_t USBD_CDC_RegisterInterface(USBD_HandleTypeDef *pdev,
                                    USBD_CDC_ItfTypeDef *fops)
{
    if (fops == NULL)
    {
        return USBD_FAIL;
    }

    /* Set CDC interface callbacks */
    pdev->pClassData[fops->interface] = (void *)fops;

    return USBD_OK;
}

/**
* @brief  USBD_CDC1_SetTxBuffer
*         Set Tx buffer
* @param  pdev: device instance
* @param  pbuff: Tx buffer
* @param  length: Tx buffer length
* @retval status
*/
uint8_t USBD_CDC1_SetTxBuffer(USBD_HandleTypeDef *pdev,
                               uint8_t *pbuff,
                               uint16_t length)
{
    USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef *) pdev->pClassData[1];

    if (hcdc == NULL)
    {
        return USBD_FAIL;
    }

    hcdc->TxBuffer = pbuff;
    hcdc->TxLength = length;

    return USBD_OK;
}

/**
* @brief  USBD_CDC1_SetRxBuffer
*         Set Rx buffer
* @param  pdev: device instance
* @param  pbuff: Rx buffer
* @retval status
*/
uint8_t USBD_CDC1_SetRxBuffer(USBD_HandleTypeDef *pdev,
                               uint8_t *pbuff)
{
    USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef *) pdev->pClassData[1];

    if (hcdc == NULL)
    {
        return USBD_FAIL;
    }

    hcdc->RxBuffer = pbuff;

    return USBD_OK;
}

/**
* @brief  USBD_CDC1_ReceivePacket
*         Receive packet
* @param  pdev: device instance
* @retval status
*/
uint8_t USBD_CDC1_ReceivePacket(USBD_HandleTypeDef *pdev)
{
    USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef *) pdev->pClassData[1];

    if (hcdc == NULL)
    {
        return USBD_FAIL;
    }

    if (pdev->pClassData[1] == NULL)
    {
        return USBD_FAIL;
    }

    /* Receive next packet */
    USBD_LL_PrepareReceive(pdev, CDC1_OUT_EP, hcdc->RxBuffer, hcdc->RxBufferSize);

    return USBD_OK;
}

/**
* @brief  USBD_CDC1_TransmitPacket
*         Transmit packet
* @param  pdev: device instance
* @retval status
*/
uint8_t USBD_CDC1_TransmitPacket(USBD_HandleTypeDef *pdev)
{
    USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef *) pdev->pClassData[1];

    if (hcdc == NULL)
    {
        return USBD_FAIL;
    }

    if (pdev->pClassData[1] == NULL)
    {
        return USBD_FAIL;
    }

    if (hcdc->TxState == 0)
    {
        hcdc->TxState = 1;

        /* Transmit next packet */
        USBD_LL_Transmit(pdev, CDC1_IN_EP, hcdc->TxBuffer, hcdc->TxLength);

        return USBD_OK;
    }
    else
    {
        return USBD_BUSY;
    }
}

/**
* @brief  USBD_CDC2_SetTxBuffer
*         Set Tx buffer for CDC2
* @param  pdev: device instance
* @param  pbuff: Tx buffer
* @param  length: Tx buffer length
* @retval status
*/
uint8_t USBD_CDC2_SetTxBuffer(USBD_HandleTypeDef *pdev,
                               uint8_t *pbuff,
                               uint16_t length)
{
    USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef *) pdev->pClassData[3];

    if (hcdc == NULL)
    {
        return USBD_FAIL;
    }

    hcdc->TxBuffer = pbuff;
    hcdc->TxLength = length;

    return USBD_OK;
}

/**
* @brief  USBD_CDC2_SetRxBuffer
*         Set Rx buffer for CDC2
* @param  pdev: device instance
* @param  pbuff: Rx buffer
* @retval status
*/
uint8_t USBD_CDC2_SetRxBuffer(USBD_HandleTypeDef *pdev,
                               uint8_t *pbuff)
{
    USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef *) pdev->pClassData[3];

    if (hcdc == NULL)
    {
        return USBD_FAIL;
    }

    hcdc->RxBuffer = pbuff;

    return USBD_OK;
}

/**
* @brief  USBD_CDC2_ReceivePacket
*         Receive packet for CDC2
* @param  pdev: device instance
* @retval status
*/
uint8_t USBD_CDC2_ReceivePacket(USBD_HandleTypeDef *pdev)
{
    USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef *) pdev->pClassData[3];

    if (hcdc == NULL)
    {
        return USBD_FAIL;
    }

    if (pdev->pClassData[3] == NULL)
    {
        return USBD_FAIL;
    }

    /* Receive next packet */
    USBD_LL_PrepareReceive(pdev, CDC2_OUT_EP, hcdc->RxBuffer, hcdc->RxBufferSize);

    return USBD_OK;
}

/**
* @brief  USBD_CDC2_TransmitPacket
*         Transmit packet for CDC2
* @param  pdev: device instance
* @retval status
*/
uint8_t USBD_CDC2_TransmitPacket(USBD_HandleTypeDef *pdev)
{
    USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef *) pdev->pClassData[3];

    if (hcdc == NULL)
    {
        return USBD_FAIL;
    }

    if (pdev->pClassData[3] == NULL)
    {
        return USBD_FAIL;
    }

    if (hcdc->TxState == 0)
    {
        hcdc->TxState = 1;

        /* Transmit next packet */
        USBD_LL_Transmit(pdev, CDC2_IN_EP, hcdc->TxBuffer, hcdc->TxLength);

        return USBD_OK;
    }
    else
    {
        return USBD_BUSY;
    }
}

/* Private functions ---------------------------------------------------------*/
/**
* @brief  USBD_CDC_Init_FS
*         Initialize the CDC layer
* @param  pdev: device instance
* @param  cfgidx: configuration index
* @retval status
*/
static uint8_t USBD_CDC_Init_FS(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
    USBD_CDC_HandleTypeDef *hcdc;
    uint8_t ret = USBD_FAIL;

    if (pdev->pClassData[cfgidx] == NULL)
    {
        /* Allocate structure for CDC interface */
        hcdc = (USBD_CDC_HandleTypeDef *) USBD_malloc(sizeof(USBD_CDC_HandleTypeDef));
        if (hcdc == NULL)
        {
            return USBD_FAIL;
        }

        pdev->pClassData[cfgidx] = (void *) hcdc;
    }

    if (cfgidx == 1) // CDC1
    {
        /* Get CDC Interface handler */
        hcdc = (USBD_CDC_HandleTypeDef *) pdev->pClassData[cfgidx];

        /* Open EP IN */
        ret = USBD_LL_OpenEP(pdev, CDC1_IN_EP, USBD_EP_TYPE_BULK, CDC_DATA_FS_IN_PACKET_SIZE);
        if (ret != USBD_OK)
        {
            return ret;
        }

        /* Open EP OUT */
        ret = USBD_LL_OpenEP(pdev, CDC1_OUT_EP, USBD_EP_TYPE_BULK, CDC_DATA_FS_OUT_PACKET_SIZE);
        if (ret != USBD_OK)
        {
            return ret;
        }

        /* Open Notification EP */
        ret = USBD_LL_OpenEP(pdev, CDC1_NOTI_EP, USBD_EP_TYPE_INTR, CDC_NOTI_PACKET_SIZE);
        if (ret != USBD_OK)
        {
            return ret;
        }

        /* Init physical Interface components */
        if (USBD_CDC_RegisterInterface(pdev, &USBD_CDC1_InterfaceCallbacks) != USBD_OK)
        {
            return USBD_FAIL;
        }

        /* Init the CDC channel */
        if (((USBD_CDC_ItfTypeDef *)pdev->pClassData[cfgidx])->Init() != 0)
        {
            return USBD_FAIL;
        }

        /* Prepare Out endpoint to receive next packet */
        USBD_CDC1_ReceivePacket(pdev);

        /* Set CDC State */
        hcdc->TxState = 0;
        hcdc->RxState = 0;
    }
    else if (cfgidx == 3) // CDC2
    {
        /* Get CDC Interface handler */
        hcdc = (USBD_CDC_HandleTypeDef *) pdev->pClassData[cfgidx];

        /* Open EP IN */
        ret = USBD_LL_OpenEP(pdev, CDC2_IN_EP, USBD_EP_TYPE_BULK, CDC_DATA_FS_IN_PACKET_SIZE);
        if (ret != USBD_OK)
        {
            return ret;
        }

        /* Open EP OUT */
        ret = USBD_LL_OpenEP(pdev, CDC2_OUT_EP, USBD_EP_TYPE_BULK, CDC_DATA_FS_OUT_PACKET_SIZE);
        if (ret != USBD_OK)
        {
            return ret;
        }

        /* Open Notification EP */
        ret = USBD_LL_OpenEP(pdev, CDC2_NOTI_EP, USBD_EP_TYPE_INTR, CDC_NOTI_PACKET_SIZE);
        if (ret != USBD_OK)
        {
            return ret;
        }

        /* Init physical Interface components */
        if (USBD_CDC_RegisterInterface(pdev, &USBD_CDC2_InterfaceCallbacks) != USBD_OK)
        {
            return USBD_FAIL;
        }

        /* Init the CDC channel */
        if (((USBD_CDC_ItfTypeDef *)pdev->pClassData[cfgidx])->Init() != 0)
        {
            return USBD_FAIL;
        }

        /* Prepare Out endpoint to receive next packet */
        USBD_CDC2_ReceivePacket(pdev);

        /* Set CDC State */
        hcdc->TxState = 0;
        hcdc->RxState = 0;
    }

    return USBD_OK;
}

/**
* @brief  USBD_CDC_DeInit_FS
*         DeInitialize the CDC layer
* @param  pdev: device instance
* @param  cfgidx: configuration index
* @retval status
*/
static uint8_t USBD_CDC_DeInit_FS(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
    USBD_CDC_HandleTypeDef *hcdc;

    if (pdev->pClassData[cfgidx] == NULL)
    {
        return USBD_FAIL;
    }

    if (cfgidx == 1) // CDC1
    {
        /* Get CDC Interface handler */
        hcdc = (USBD_CDC_HandleTypeDef *) pdev->pClassData[cfgidx];

        /* Open EP IN */
        USBD_LL_CloseEP(pdev, CDC1_IN_EP);
        /* Open EP OUT */
        USBD_LL_CloseEP(pdev, CDC1_OUT_EP);
        /* Open Notification EP */
        USBD_LL_CloseEP(pdev, CDC1_NOTI_EP);

        /* DeInit  physical Interface components */
        if (pdev->pClassData[cfgidx] != NULL)
        {
            ((USBD_CDC_ItfTypeDef *)pdev->pClassData[cfgidx])->DeInit();
            pdev->pClassData[cfgidx] = NULL;
        }

        /* Free allocated resources */
        USBD_free(pdev->pClassData[cfgidx]);
        pdev->pClassData[cfgidx] = NULL;
    }
    else if (cfgidx == 3) // CDC2
    {
        /* Get CDC Interface handler */
        hcdc = (USBD_CDC_HandleTypeDef *) pdev->pClassData[cfgidx];

        /* Open EP IN */
        USBD_LL_CloseEP(pdev, CDC2_IN_EP);
        /* Open EP OUT */
        USBD_LL_CloseEP(pdev, CDC2_OUT_EP);
        /* Open Notification EP */
        USBD_LL_CloseEP(pdev, CDC2_NOTI_EP);

        /* DeInit  physical Interface components */
        if (pdev->pClassData[cfgidx] != NULL)
        {
            ((USBD_CDC_ItfTypeDef *)pdev->pClassData[cfgidx])->DeInit();
            pdev->pClassData[cfgidx] = NULL;
        }

        /* Free allocated resources */
        USBD_free(pdev->pClassData[cfgidx]);
        pdev->pClassData[cfgidx] = NULL;
    }

    return USBD_OK;
}

/**
* @brief  USBD_CDC_Setup_FS
*         Handle the CDC specific requests
* @param  pdev: instance
* @param  req: usb request
* @retval status
*/
static uint8_t USBD_CDC_Setup_FS(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
    USBD_CDC_HandleTypeDef *hcdc;
    uint8_t ret = USBD_OK;
    uint16_t status_info;
    USBD_CDC_ItfTypeDef *cdc;

    if (req->bmRequest & USB_REQ_TYPE_MASK)
    {
        switch (req->bRequest)
        {
            case CDC_SEND_ENCAPSULATED_COMMAND:
                /* Check if the command class is supported */
                if (req->wIndex == 1) // CDC1
                {
                    cdc = (USBD_CDC_ItfTypeDef *) pdev->pClassData[1];
                    cdc->Control(req->bRequest, req->wValue, req->wLength);
                }
                else if (req->wIndex == 3) // CDC2
                {
                    cdc = (USBD_CDC_ItfTypeDef *) pdev->pClassData[3];
                    cdc->Control(req->bRequest, req->wValue, req->wLength);
                }
                break;

            case CDC_GET_ENCAPSULATED_RESPONSE:
                /* Check if the command class is supported */
                if (req->wIndex == 1) // CDC1
                {
                    cdc = (USBD_CDC_ItfTypeDef *) pdev->pClassData[1];
                    cdc->Control(req->bRequest, req->wValue, req->wLength);
                }
                else if (req->wIndex == 3) // CDC2
                {
                    cdc = (USBD_CDC_ItfTypeDef *) pdev->pClassData[3];
                    cdc->Control(req->bRequest, req->wValue, req->wLength);
                }
                break;

            case CDC_SET_COMM_FEATURE:
                /* Check if the command class is supported */
                if (req->wIndex == 1) // CDC1
                {
                    cdc = (USBD_CDC_ItfTypeDef *) pdev->pClassData[1];
                    cdc->Control(req->bRequest, req->wValue, req->wLength);
                }
                else if (req->wIndex == 3) // CDC2
                {
                    cdc = (USBD_CDC_ItfTypeDef *) pdev->pClassData[3];
                    cdc->Control(req->bRequest, req->wValue, req->wLength);
                }
                break;

            case CDC_GET_COMM_FEATURE:
                /* Check if the command class is supported */
                if (req->wIndex == 1) // CDC1
                {
                    cdc = (USBD_CDC_ItfTypeDef *) pdev->pClassData[1];
                    cdc->Control(req->bRequest, req->wValue, req->wLength);
                }
                else if (req->wIndex == 3) // CDC2
                {
                    cdc = (USBD_CDC_ItfTypeDef *) pdev->pClassData[3];
                    cdc->Control(req->bRequest, req->wValue, req->wLength);
                }
                break;

            case CDC_CLEAR_COMM_FEATURE:
                /* Check if the command class is supported */
                if (req->wIndex == 1) // CDC1
                {
                    cdc = (USBD_CDC_ItfTypeDef *) pdev->pClassData[1];
                    cdc->Control(req->bRequest, req->wValue, req->wLength);
                }
                else if (req->wIndex == 3) // CDC2
                {
                    cdc = (USBD_CDC_ItfTypeDef *) pdev->pClassData[3];
                    cdc->Control(req->bRequest, req->wValue, req->wLength);
                }
                break;

            case CDC_SET_LINE_CODING:
                /* Check if the command class is supported */
                if (req->wIndex == 1) // CDC1
                {
                    cdc = (USBD_CDC_ItfTypeDef *) pdev->pClassData[1];
                    cdc->Control(req->bRequest, req->wValue, req->wLength);
                }
                else if (req->wIndex == 3) // CDC2
                {
                    cdc = (USBD_CDC_ItfTypeDef *) pdev->pClassData[3];
                    cdc->Control(req->bRequest, req->wValue, req->wLength);
                }
                break;

            case CDC_GET_LINE_CODING:
                /* Check if the command class is supported */
                if (req->wIndex == 1) // CDC1
                {
                    cdc = (USBD_CDC_ItfTypeDef *) pdev->pClassData[1];
                    cdc->Control(req->bRequest, req->wValue, req->wLength);
                }
                else if (req->wIndex == 3) // CDC2
                {
                    cdc = (USBD_CDC_ItfTypeDef *) pdev->pClassData[3];
                    cdc->Control(req->bRequest, req->wValue, req->wLength);
                }
                break;

            case CDC_SET_CONTROL_LINE_STATE:
                /* Check if the command class is supported */
                if (req->wIndex == 1) // CDC1
                {
                    cdc = (USBD_CDC_ItfTypeDef *) pdev->pClassData[1];
                    cdc->Control(req->bRequest, req->wValue, req->wLength);
                }
                else if (req->wIndex == 3) // CDC2
                {
                    cdc = (USBD_CDC_ItfTypeDef *) pdev->pClassData[3];
                    cdc->Control(req->bRequest, req->wValue, req->wLength);
                }
                break;

            case CDC_SEND_BREAK:
                /* Check if the command class is supported */
                if (req->wIndex == 1) // CDC1
                {
                    cdc = (USBD_CDC_ItfTypeDef *) pdev->pClassData[1];
                    cdc->Control(req->bRequest, req->wValue, req->wLength);
                }
                else if (req->wIndex == 3) // CDC2
                {
                    cdc = (USBD_CDC_ItfTypeDef *) pdev->pClassData[3];
                    cdc->Control(req->bRequest, req->wValue, req->wLength);
                }
                break;

            default:
                break;
        }
    }
    else
    {
        switch (req->bRequest)
        {
            case CDC_GET_LINE_CODING:
                if (req->wIndex == 1) // CDC1
                {
                    cdc = (USBD_CDC_ItfTypeDef *) pdev->pClassData[1];
                    cdc->Control(req->bRequest, req->wValue, req->wLength);
                }
                else if (req->wIndex == 3) // CDC2
                {
                    cdc = (USBD_CDC_ItfTypeDef *) pdev->pClassData[3];
                    cdc->Control(req->bRequest, req->wValue, req->wLength);
                }
                break;

            case CDC_SET_LINE_CODING:
                if (req->wIndex == 1) // CDC1
                {
                    cdc = (USBD_CDC_ItfTypeDef *) pdev->pClassData[1];
                    cdc->Control(req->bRequest, req->wValue, req->wLength);
                }
                else if (req->wIndex == 3) // CDC2
                {
                    cdc = (USBD_CDC_ItfTypeDef *) pdev->pClassData[3];
                    cdc->Control(req->bRequest, req->wValue, req->wLength);
                }
                break;

            case CDC_SET_CONTROL_LINE_STATE:
                if (req->wIndex == 1) // CDC1
                {
                    cdc = (USBD_CDC_ItfTypeDef *) pdev->pClassData[1];
                    cdc->Control(req->bRequest, req->wValue, req->wLength);
                }
                else if (req->wIndex == 3) // CDC2
                {
                    cdc = (USBD_CDC_ItfTypeDef *) pdev->pClassData[3];
                    cdc->Control(req->bRequest, req->wValue, req->wLength);
                }
                break;

            default:
                ret = USBD_FAIL;
                break;
        }
    }

    return ret;
}

/**
* @brief  USBD_CDC_DataIn_FS
*         Handle CDC data IN stage
* @param  pdev: device instance
* @param  epnum: endpoint number
* @retval status
*/
static uint8_t USBD_CDC_DataIn_FS(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
    USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef *) pdev->pClassData[1];

    if (hcdc == NULL)
    {
        return USBD_FAIL;
    }

    if (pdev->pClassData[1] == NULL)
    {
        return USBD_FAIL;
    }

    hcdc->TxState = 0;

    return USBD_OK;
}

/**
* @brief  USBD_CDC_DataOut_FS
*         Handle CDC data OUT stage
* @param  pdev: device instance
* @param  epnum: endpoint number
* @retval status
*/
static uint8_t USBD_CDC_DataOut_FS(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
    USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef *) pdev->pClassData[1];

    if (hcdc == NULL)
    {
        return USBD_FAIL;
    }

    if (pdev->pClassData[1] == NULL)
    {
        return USBD_FAIL;
    }

    /* Get received data length */
    hcdc->RxLength = USBD_LL_GetRxDataSize(pdev, epnum);

    /* Call receive data callback */
    if (hcdc->RxState == 0)
    {
        ((USBD_CDC_ItfTypeDef *)pdev->pClassData[1])->Receive(hcdc->RxBuffer, &hcdc->RxLength);
        hcdc->RxState = 1;
    }

    return USBD_OK;
}

/**
* @brief  USBD_CDC_DataIn2_FS
*         Handle CDC2 data IN stage
* @param  pdev: device instance
* @param  epnum: endpoint number
* @retval status
*/
static uint8_t USBD_CDC_DataIn2_FS(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
    USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef *) pdev->pClassData[3];

    if (hcdc == NULL)
    {
        return USBD_FAIL;
    }

    if (pdev->pClassData[3] == NULL)
    {
        return USBD_FAIL;
    }

    hcdc->TxState = 0;

    return USBD_OK;
}

/**
* @brief  USBD_CDC_DataOut2_FS
*         Handle CDC2 data OUT stage
* @param  pdev: device instance
* @param  epnum: endpoint number
* @retval status
*/
static uint8_t USBD_CDC_DataOut2_FS(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
    USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef *) pdev->pClassData[3];

    if (hcdc == NULL)
    {
        return USBD_FAIL;
    }

    if (pdev->pClassData[3] == NULL)
    {
        return USBD_FAIL;
    }

    /* Get received data length */
    hcdc->RxLength = USBD_LL_GetRxDataSize(pdev, epnum);

    /* Call receive data callback */
    if (hcdc->RxState == 0)
    {
        ((USBD_CDC_ItfTypeDef *)pdev->pClassData[3])->Receive(hcdc->RxBuffer, &hcdc->RxLength);
        hcdc->RxState = 1;
    }

    return USBD_OK;
}

/**
* @brief  USBD_CDC_EP0_Rx_Ready_FS
*         Handle CDC ready stage
* @param  pdev: device instance
* @retval status
*/
static uint8_t USBD_CDC_EP0_Rx_Ready_FS(USBD_HandleTypeDef *pdev)
{
    USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef *) pdev->pClassData[1];

    if (hcdc == NULL)
    {
        return USBD_FAIL;
    }

    if (pdev->pClassData[1] == NULL)
    {
        return USBD_FAIL;
    }

    if (hcdc->RxState == 1)
    {
        ((USBD_CDC_ItfTypeDef *)pdev->pClassData[1])->Receive(hcdc->RxBuffer, &hcdc->RxLength);
        hcdc->RxState = 0;
    }

    return USBD_OK;
}

/**
* @brief  USBD_CDC_DataInNotif_FS
*         Handle CDC data IN notification stage
* @param  pdev: device instance
* @param  epnum: endpoint number
* @retval status
*/
static uint8_t USBD_CDC_DataInNotif_FS(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
    return USBD_OK;
}

/**
* @brief  USBD_CDC_GetTxState_FS
*         Get the current Tx state
* @param  pdev: device instance
* @retval Tx state
*/
uint8_t USBD_CDC_GetTxState_FS(USBD_HandleTypeDef *pdev)
{
    USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef *) pdev->pClassData[1];

    if (hcdc == NULL)
    {
        return 0;
    }

    return hcdc->TxState;
}

/**
* @brief  USBD_CDC_GetTxState2_FS
*         Get the current Tx state for CDC2
* @param  pdev: device instance
* @retval Tx state
*/
uint8_t USBD_CDC_GetTxState2_FS(USBD_HandleTypeDef *pdev)
{
    USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef *) pdev->pClassData[3];

    if (hcdc == NULL)
    {
        return 0;
    }

    return hcdc->TxState;
}

/**
* @brief  USBD_Get_CDC_Class
*         Get the CDC class handler
* @param  pdev: device instance
* @retval class handler
*/
USBD_ClassTypeDef *USBD_Get_CDC_Class_FS(void)
{
    return &USBD_CDC_Class;
}
