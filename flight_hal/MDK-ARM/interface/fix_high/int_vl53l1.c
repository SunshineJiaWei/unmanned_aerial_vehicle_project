#include "int_vl53l1.h"

#define VL53L1_DEV  0x52


void int_vl53l1_init(void)
{
    uint8_t byteData = 0;
    uint16_t wordData = 0;
    uint8_t sensorState = 0;

    // 复位
    HAL_GPIO_WritePin(VX_XSHUT_GPIO_Port, VX_XSHUT_Pin, GPIO_PIN_RESET);
    vTaskDelay(pdMS_TO_TICKS(2));
    HAL_GPIO_WritePin(VX_XSHUT_GPIO_Port, VX_XSHUT_Pin, GPIO_PIN_SET);
    vTaskDelay(pdMS_TO_TICKS(2));

    /* Those basic I2C read functions can be used to check your own I2C functions */
    VL53L1_RdByte(VL53L1_DEV, 0x010F, &byteData);
    printf("VL53L1X Model_ID: %X\n", byteData);
    VL53L1_RdByte(VL53L1_DEV, 0x0110, &byteData);
    printf("VL53L1X Module_Type: %X\n", byteData);
    VL53L1_RdWord(VL53L1_DEV, 0x010F, &wordData);
    printf("VL53L1X: %X\n", wordData);
    while (sensorState == 0)
    {
        VL53L1X_BootState(VL53L1_DEV, &sensorState);
        vTaskDelay(pdMS_TO_TICKS(2));
    }
    printf("Chip booted\n");

    /* This function must to be called to initialize the sensor with the default setting  */
    VL53L1X_SensorInit(VL53L1_DEV);
    /* Optional functions to be used to change the main ranging parameters according the application requirements to get the best ranging performances */
    VL53L1X_SetDistanceMode(VL53L1_DEV, 2);           /* 1=short, 2=long */
    VL53L1X_SetTimingBudgetInMs(VL53L1_DEV, 20);     /* in ms possible values [20, 50, 100, 200, 500] */
    VL53L1X_SetInterMeasurementInMs(VL53L1_DEV, 20); /* in ms, IM must be > = TB */
                                                        //  status = VL53L1X_SetOffset(VL53L1_DEV,20); /* offset compensation in mm */
                                                        //  status = VL53L1X_SetROI(VL53L1_DEV, 16, 16); /* minimum ROI 4,4 */
                                                        //	status = VL53L1X_CalibrateOffset(VL53L1_DEV, 140, &offset); /* may take few second to perform the offset cal*/
                                                        //	status = VL53L1X_CalibrateXtalk(VL53L1_DEV, 1000, &xtalk); /* may take few second to perform the xtalk cal */
    printf("VL53L1X Ultra Lite Driver Example running ...\n");
    VL53L1X_StartRanging(VL53L1_DEV); /* This function has to be called to enable the ranging */
}

uint16_t int_vl53l1_get_distance(void)
{
    uint16_t distance;
    VL53L1X_GetDistance(VL53L1_DEV, &distance);

    return distance;
}