package me.hypocrite30.serialport.serial;

import cn.hutool.core.util.ArrayUtil;
import gnu.io.CommPort;
import gnu.io.CommPortIdentifier;
import gnu.io.SerialPort;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.IOException;
import java.io.InputStream;
import java.util.TooManyListenersException;

/**
 * 串口工具类
 *
 * @Author: Hypocrite30
 * @Date: 2022/1/7 17:32
 */
public class SerialPortUtils {

    private static Logger log = LoggerFactory.getLogger(SerialPortUtils.class);

    /**
     * 打开串口
     *
     * @param portName 串口名
     * @param baudRate 波特率
     * @param dataBits 数据位
     * @param stopBits 停止位
     * @param parity   校验位
     * @return 串口对象
     */
    public static SerialPort open(String portName, Integer baudRate, Integer dataBits,
                                  Integer stopBits, Integer parity) {
        SerialPort result = null;
        try {
            // 通过端口名识别端口
            CommPortIdentifier identifier = CommPortIdentifier.getPortIdentifier(portName);
            // 打开端口，并给端口命名和超时时间
            CommPort commPort = identifier.open(portName, 2000);
            // 判断是否为串口
            if (commPort instanceof SerialPort) {
                result = (SerialPort) commPort;
                // 设置串口的波特率等参数
                result.setSerialPortParams(baudRate, dataBits, stopBits, parity);
                log.info("打开串口 {} 成功", portName);
            } else {
                log.info("{} 不是串口", portName);
            }
        } catch (Exception e) {
            log.error("打开串口 {} 错误", portName, e);
        }
        return result;
    }

    /**
     * 给串口添加监听器
     *
     * @param serialPort 串口对象
     * @param listener   监听器
     */
    public static void addListener(SerialPort serialPort, DataAvailableListener listener) {
        if (serialPort == null) {
            return;
        }
        try {
            // 给串口添加监听器
            serialPort.addEventListener(new SerialPortListener(listener));
            // 设置当有数据到达时唤醒监听接收线程
            serialPort.notifyOnDataAvailable(Boolean.TRUE);
            // 设置当通信中断时唤醒中断线程
            serialPort.notifyOnBreakInterrupt(Boolean.TRUE);
        } catch (TooManyListenersException e) {
            log.error("串口 {} 增加监听器错误", serialPort.getName(), e);
        }
    }

    /**
     * 从串口读取数据
     *
     * @param serialPort 串口对象
     * @return 串口通信获取的字节数据
     */
    public static byte[] read(SerialPort serialPort) {
        byte[] result = {};
        if (serialPort == null) {
            return result;
        }
        try (InputStream inputStream = serialPort.getInputStream()) {
            byte[] readBuffer = new byte[1024];
            while (inputStream.available() > 0) {
                inputStream.read(readBuffer);
                // 将读入的 buffer 中数据累加到 result 中
                result = ArrayUtil.addAll(result, readBuffer);
            }
        } catch (IOException e) {
            log.error("串口 {} 读取数据错误", serialPort.getName(), e);
        }
        return result;
    }
}
