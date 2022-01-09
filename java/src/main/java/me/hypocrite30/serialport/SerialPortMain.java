package me.hypocrite30.serialport;

import com.alibaba.excel.EasyExcel;
import me.hypocrite30.serialport.easyexcel.ADCVO;
import me.hypocrite30.serialport.serial.SerialPortUtils;
import gnu.io.SerialPort;

import javax.swing.*;
import java.util.ArrayList;
import java.util.List;

/**
 * 串口测试程序
 *
 * @Author: Hypocrite30
 * @Date: 2022/1/7 17:53
 */
public class SerialPortMain {

    // 温度阈值
    public static final int TEMPER_THRESHOLD = 2400;

    // 存储温度集合
    private static List<Integer> temperature_List = new ArrayList<>(5);

    public static void main(String[] args) {
        // 打开串口
        SerialPort serialPort = SerialPortUtils.open("COM5", 115200, SerialPort.DATABITS_8,
                SerialPort.STOPBITS_1, SerialPort.PARITY_NONE);

        // 给串口添加监听器
        SerialPortUtils.addListener(serialPort, () -> {
            // 获取串口传输PC端数据
            byte[] data = SerialPortUtils.read(serialPort);
            // 将字符数组转译成 ASCII 字符串
            String resultStr = new String(data);
            // 转换为 StringBuilder，便于字符串拼接
            StringBuilder result = new StringBuilder(resultStr);
            // 字符串拆分，获取温度
            String[] results = resultStr.split("#");
            int temperature = Integer.valueOf(results[1]);
            // 保存温度到内存
            temperature_List.add(temperature);
            // 计算温差
            int diff = Math.abs(temperature - TEMPER_THRESHOLD);
            result.append("  温差为：").append(diff);
            // 若按键按动超过 5 次并且温度超过阈值
            if (temperature_List.size() >= 5 && temperature < TEMPER_THRESHOLD) {
                // 获取温度总和
                int sum_temper = temperature_List.stream().mapToInt(i -> i).sum();
                int average_temper = sum_temper / temperature_List.size();
                result.append("  平均值：").append(average_temper);
                // 计算最小 ADC，即最大温度
                int min_temper = temperature_List.stream().mapToInt(i -> i).min().getAsInt();
                result.append("  adc最小值（最高温度）：").append(min_temper);
                // 警报弹窗提醒
                JOptionPane.showMessageDialog(null, result.toString(), "温度过高，警报！",
                        JOptionPane.WARNING_MESSAGE);
            }
            // 控制台输出串口数据
            System.out.println(result);
        });

        // 将数值保存本地磁盘，excel 储存
        List<ADCVO> excel_list = new ArrayList<>(temperature_List.size());
        for (int i = 0; i < temperature_List.size(); i++) {
            ADCVO adcvo = new ADCVO();
            // 设置 ADC 编号，adc值和差值
            adcvo.setUid(i);
            adcvo.setAdc(temperature_List.get(i));
            adcvo.setDiff(Math.abs(temperature_List.get(i) - TEMPER_THRESHOLD));
            excel_list.add(adcvo);
        }
        // 保存本地磁盘路径
        String filePath = "D:\\温度监控日志.xlsx";
        // 执行 EasyExcel 将内存数据格式化 excel 保存本地磁盘
        EasyExcel.write(filePath, ADCVO.class).sheet("ADC温度监控日志").doWrite(excel_list);

    }

}
