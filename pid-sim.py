#!/usr/bin/env python
import matplotlib.pyplot as plt
import numpy as np

# 温度单位0.1摄氏度
class PID:
    def __init__(self, target_temp, current_temp):
        self._target_temp = target_temp * 10
        self._current_temp = current_temp * 10
        self._real_temp = current_temp * 10
        
        # 每秒加热3摄氏度
        self._speed = 30

        self._kp = 70
        self._ki = 4
        self._kd = 6
        self._deadTime = 2

        self._outputMin = 0
        self._outputMax = 10000

        self._integral = 0
        self._lastMeasurement = 0
        self._lastOutput = 0

        self._temp_history = []
        self._temp_history_real = []

        self._pwm = 0

    def calc_pwm(self):
        error = self._target_temp - self._current_temp

        rateOfChange = self._current_temp - self._lastMeasurement
        predictedMeasurement = self._current_temp + rateOfChange * self._deadTime
        predictedError = self._target_temp - predictedMeasurement

        p_term = self._kp * predictedError

        i_term = self._integral
        if self._lastOutput > self._outputMin and self._lastOutput < self._outputMax:
            i_term += self._ki * error

            if i_term > self._outputMax:
                i_term = self._outputMax
            elif i_term < self._outputMin:
                i_term = self._outputMin
        
        self._integral = i_term

        d_term = -self._kd * rateOfChange

        self._pwm = p_term + i_term + d_term
        if self._pwm > self._outputMax:
            self._pwm = self._outputMax
        elif self._pwm < self._outputMin:
            self._pwm = self._outputMin

        self._lastMeasurement = self._current_temp
        self._lastOutput = self._pwm
        print (p_term, i_term, d_term, self._pwm)
        
    def sim(self):
        self.calc_pwm()

        self._real_temp += self._pwm * self._speed // 10000

        # 这里假定温度每秒降低0.5摄氏度
        self._real_temp -= 5
        self._temp_history_real.append(self._real_temp)

        if len(self._temp_history_real) >= 3:
            self._current_temp = self._temp_history_real[-3]
            self._temp_history.append(self._current_temp)

pid = PID(150, 30)
n = 0
while n < 120:
    pid.sim()
    print ('no: %d\tpwm: %d\tcur: %d\ttarget: %d' % (n, pid._pwm, pid._current_temp, pid._real_temp))
    n += 1 
    

time = np.arange(0, 120, 1)

temperature1 = np.array([0, 0] + pid._temp_history) / 10
temperature2 = np.array(pid._temp_history_real) / 10
plt.figure(figsize=(15, 6))

plt.plot(time, temperature1, marker='o', linestyle='-', label='序列 1 (Series 1)')
plt.plot(time, temperature2, marker='s', linestyle='--', label='序列 2 (Series 2)')

plt.title('两个温度序列对比图 (Comparison of Two Temperature Series)')
plt.xlabel('时间 (秒) (Time (s))')
plt.ylabel('温度 (摄氏度) (Temperature (°C))')
plt.grid(True)

plt.legend()
plt.show()