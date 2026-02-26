/*12路pid温度调节*/
#include "config.h"
#include "math.h"

// 温度控制参数
#define KP 150.0f
#define KI 6.0f
#define KD 80.0f
#define MAX_PWM 1000
#define MIN_PWM 0

#define __MAX(a,b) ((a) > (b) ? (a) : (b))
#define __MIN(a,b) ((a) < (b) ? (a) : (b))


/*协议收发任务*/
static tmr_t tmr_start_task03_heat;
static void task03_heat(int timer_id, void *data);
bool flag_heat = 0; // 标志位，用于控制是否启动
volatile uint8_t target_temp = 0;//目标温度
volatile uint8_t over_temp = 0;//超温保护
volatile uint16_t pwm_duty[NUM_CHANNELS] = {0,0,0,0,0,0,0,0,0,0,0,0};//pwm
////////////////////////////////////////////0,2,3,4,5,6,7,8,9,10,11,12///////

void start_task03_heat(void)
{
    start_rpt_tmr(&tmr_start_task03_heat,task03_heat, MS_TO_TICKS(500));
}

static void task03_heat(int timer_id,void *data)
{
	static float last_error[NUM_CHANNELS] = {0};
	static float integral[NUM_CHANNELS] = {0};//

	// 12路温度控制
	for(uint8_t i = 0; i < NUM_CHANNELS; i++) {
			if(flag_heat == 0) {
					pwm_duty[i] = 0; //控制开关
					continue;
			}
							
			if(temp_value[i] >= over_temp){
				 pwm_duty[i] = 0; // 软件超温保护
				 continue;
			}
							
			float current_temp = temp_value[i];

			// 温度保护
			if(current_temp >= target_temp){
					pwm_duty[i] = MIN_PWM;
					continue;
			}

			float error = target_temp - current_temp;

			// 动态PID参数
			float dynamic_KP = KP;
			float dynamic_KI = KI;
			if(fabs(error) > 5.0f) {
					dynamic_KP = KP * 1.5f;
			}
			
			// PID计算
			integral[i] += error;
			integral[i] = (integral[i] > 2000.0f) ? 2000.0f : integral[i];
			integral[i] = (integral[i] < -2000.0f) ? -2000.0f : integral[i];

			float derivative = error - last_error[i];
			float output = dynamic_KP * error + dynamic_KI * integral[i] + KD * derivative;
			last_error[i] = error;

			pwm_duty[i] = (uint16_t)output;
			pwm_duty[i] = (pwm_duty[i] > MAX_PWM) ? MAX_PWM : pwm_duty[i];
	}
    // 设置12路PWM输出
		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_4,pwm_duty[0]);  // pwm1
		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_3,pwm_duty[1]);  // pwm2
		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2,pwm_duty[2]);  // pwm3
		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1,pwm_duty[3]);  // pwm4
		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4,pwm_duty[4]);  // pwm5
		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3,pwm_duty[5]);  // pwm6
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1,pwm_duty[6]);  // pwm7
		__HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_4,pwm_duty[7]);  // pwm8
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1,pwm_duty[8]);  // pwm9
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2,pwm_duty[9]);  // pwm10
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3,pwm_duty[10]); // pwm11
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4,pwm_duty[11]); // pwm12
}


#if 0









    static float last_error[NUM_CHANNELS] = {0};
    static float integral[NUM_CHANNELS] = {0};
    static uint8_t stable_count[NUM_CHANNELS] = {0}; // 温度稳定计数器

    for(uint8_t i = 0; i < NUM_CHANNELS; i++) {
        if(flag_heat[i] == 0) {
            pwm_duty[i] = 0;
            continue;
        }
        
        if(temp_value[i] >= over_temp) {
            pwm_duty[i] = 0;
            continue;
        }

        float current_temp = temp_value[i];
        float error = target_temp - current_temp;

        // 动态PID参数调整
        float dynamic_KP = KP;
        float dynamic_KI = KI;
        
        // 接近目标温度时降低KP防止震荡
        if(fabs(error) < 2.0f) {
            dynamic_KP = KP * 0.6f;
            dynamic_KI = KI * 1.2f; // 增加积分作用
            stable_count[i]++;
        } 
        // 温差较大时增强控制力度
        else if(fabs(error) > 5.0f) {
            dynamic_KP = KP * 1.5f;
            stable_count[i] = 0;
        }
        // 中等温差正常控制
        else {
            dynamic_KP = KP;
            stable_count[i] = 0;
        }

        // 温度稳定后保持最小维持功率
        if(stable_count[i] > 10) {
            pwm_duty[i] = MAX_PWM * 0.1f; // 10%维持功率
            continue;
        }

        // PID计算
        integral[i] += error;
        // 积分限幅
        integral[i] = __MAX(__MIN(integral[i], 2000.0f), -2000.0f);
        
        float derivative = error - last_error[i];
        float output = dynamic_KP * error + dynamic_KI * integral[i] + KD * derivative;
        last_error[i] = error;

        // 输出限幅
        pwm_duty[i] = (uint16_t)__MAX(__MIN(output, MAX_PWM), MIN_PWM);
    }





#endif




