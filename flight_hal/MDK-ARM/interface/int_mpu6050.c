#include "int_mpu6050.h"


int16_t accel_offset_x;
int16_t accel_offset_y;
int16_t accel_offset_z;
int16_t gyro_offset_x; 
int16_t gyro_offset_y;
int16_t gyro_offset_z;


static void int_mpu6050_reg_write(uint8_t reg, uint8_t data)
{
    HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR_WRITE, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, 1000);
}

static void int_mpu6050_reg_read(uint8_t reg, uint8_t *data)
{
    HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR_READ, reg, I2C_MEMADD_SIZE_8BIT, data, 1, 1000);
}

void int_mput6050_calc_offset(void)
{
    mpu6050_accel_t last_accel = {0};
    mpu6050_accel_t cur_accel = {0};
    mpu6050_data_t data = {0};

    // 等待飞机平稳
    uint8_t count = 0;
    int_mpu6050_get_accel(&last_accel);
    while (count < 100)
    {
        int_mpu6050_get_accel(&cur_accel);
        if (abs(cur_accel.accel_x - last_accel.accel_x < 300) && abs(cur_accel.accel_y - last_accel.accel_y < 300) && abs(cur_accel.accel_z - last_accel.accel_z < 300))
        {
            count++;
        }
        else
        {
            count = 0;
        }
        last_accel = cur_accel;
        vTaskDelay(pdMS_TO_TICKS(6));
    }

    int32_t accel_x_sum = 0;
    int32_t accel_y_sum = 0;
    int32_t accel_z_sum = 0;
    int32_t gyro_x_sum = 0;
    int32_t gyro_y_sum = 0;
    int32_t gyro_z_sum = 0;

    for (uint8_t j = 0; j < 100; j ++)
    {
        int_mpu6050_get_data(&data);
        accel_x_sum += (data.accel.accel_x - 0);
        accel_y_sum += (data.accel.accel_y - 0);
        accel_z_sum += (data.accel.accel_z - 16384);
        gyro_x_sum += (data.gyro.gyro_x - 0);
        gyro_y_sum += (data.gyro.gyro_y - 0);
        gyro_z_sum += (data.gyro.gyro_z - 0);
        vTaskDelay(pdMS_TO_TICKS(6));
    }

    accel_offset_x = accel_x_sum / 100;
    accel_offset_y = accel_y_sum / 100;
    accel_offset_z = accel_z_sum / 100;
    gyro_offset_x = gyro_x_sum / 100;
    gyro_offset_y = gyro_y_sum / 100;
    gyro_offset_z = gyro_z_sum / 100;
}

void int_mpu6050_init(void)
{
    // 复位mpu6050
    int_mpu6050_reg_write(MPU_POWER_MANAGEMENT_1, 0x80);
    uint8_t reg_val = 0;
    while (reg_val != 0x40)
    { 
        // 该寄存器复位后默认值为0x40，及进入低功耗模式
        int_mpu6050_reg_read(MPU_POWER_MANAGEMENT_1, &reg_val);
    }
    // 配置系统时钟使用PLL，使用陀螺仪x轴为时钟源
    int_mpu6050_reg_write(MPU_POWER_MANAGEMENT_1, 0x01);

    // 设置陀螺仪量程 +-2000°/s
    int_mpu6050_reg_write(MPU_GYROSCOPE_CONFIG, 0x03 << 3);

    // 设置加速度计 +-2g
    int_mpu6050_reg_write(MPU_ACCELEROMETER_CONFIG, 0x00);

    int_mpu6050_reg_write(MPU_INTERRUPT_ENABLE, 0x00);

    int_mpu6050_reg_write(MPU_USER_CONTROL, 0x00);

    int_mpu6050_reg_write(MPU_FIFO_ENABLE, 0x00);

    // 2分频，500HZ
    int_mpu6050_reg_write(MPU_SAMPLE_RATE_DIVIDER, 0x01);

    // 配置低通滤波，加速度184Hz，陀螺仪为188Hz，采样频率1kHz
    int_mpu6050_reg_write(MPU_CONFIGURATION, 0x01);

    // 使能加速度计和陀螺仪，置1进入待机模式
    int_mpu6050_reg_write(MPU_POWER_MANAGEMENT_2, 0x00);

    //零偏校准
    int_mput6050_calc_offset();

}

void int_mpu6050_get_accel(mpu6050_accel_t *accel) // 抖动严重，需要零偏校准   
{
    uint8_t high8_reg = 0;
    uint8_t low8_reg = 0;

    int_mpu6050_reg_read(MPU_ACCEL_XOUT_H, &high8_reg);
    int_mpu6050_reg_read(MPU_ACCEL_XOUT_L, &low8_reg);
    accel->accel_x = ((high8_reg << 8) | low8_reg) - accel_offset_x;
    
    int_mpu6050_reg_read(MPU_ACCEL_YOUT_H, &high8_reg);
    int_mpu6050_reg_read(MPU_ACCEL_YOUT_L, &low8_reg);
    accel->accel_y = ((high8_reg << 8) | low8_reg) - accel_offset_y;
    
    int_mpu6050_reg_read(MPU_ACCEL_ZOUT_H, &high8_reg);
    int_mpu6050_reg_read(MPU_ACCEL_ZOUT_L, &low8_reg);
    accel->accel_z = ((high8_reg << 8) | low8_reg) - accel_offset_z;
}

void int_mpu6050_get_gyro(mpu6050_gyro_t *gyro) // 后续需要零偏校准
{
    uint8_t high8_reg = 0;
    uint8_t low8_reg = 0;

    int_mpu6050_reg_read(MPU_GYRO_XOUT_H, &high8_reg);
    int_mpu6050_reg_read(MPU_GYRO_XOUT_L, &low8_reg);
    gyro->gyro_x = ((high8_reg << 8) | low8_reg) - gyro_offset_x;

    int_mpu6050_reg_read(MPU_GYRO_YOUT_H, &high8_reg);
    int_mpu6050_reg_read(MPU_GYRO_YOUT_L, &low8_reg);
    gyro->gyro_y = ((high8_reg << 8) | low8_reg) - gyro_offset_y;
    
    int_mpu6050_reg_read(MPU_GYRO_ZOUT_H, &high8_reg);
    int_mpu6050_reg_read(MPU_GYRO_ZOUT_L, &low8_reg);
    gyro->gyro_z = ((high8_reg << 8) | low8_reg) - gyro_offset_z;
} 


void int_mpu6050_get_data(mpu6050_data_t *data)
{
    int_mpu6050_get_accel(&data->accel);
    int_mpu6050_get_gyro(&data->gyro);
}

 