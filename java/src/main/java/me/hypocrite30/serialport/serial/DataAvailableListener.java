package me.hypocrite30.serialport.serial;

/**
 * 串口监听器接口
 *
 * @Author: Hypocrite30
 * @Date:
 */
@FunctionalInterface
public interface DataAvailableListener {

    /**
     * 串口存在有效数据时调用
     */
    void dataAvailable();
}
