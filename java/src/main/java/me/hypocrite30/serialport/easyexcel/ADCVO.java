package me.hypocrite30.serialport.easyexcel;

import com.alibaba.excel.annotation.ExcelProperty;

import java.util.Objects;

/**
 * ADC 保存再 Excel 表格属性对象
 *
 * @Author: Hypocrite30
 * @Date: 2022/1/7 20:07
 */
public class ADCVO {

    @ExcelProperty(value = "ADC编号", index = 0)
    private int uid;

    @ExcelProperty(value = "ADC值", index = 1)
    private int adc;

    @ExcelProperty(value = "差值", index = 2)
    private int diff;

    @ExcelProperty(value = "阈值", index = 5)
    private static final int threshold = 2400;

    public ADCVO() {
    }

    public ADCVO(int uid, int adc, int diff) {
        this.uid = uid;
        this.adc = adc;
        this.diff = diff;
    }

    @Override
    public String toString() {
        return "ADCVO{" +
                "id=" + uid +
                ", adc=" + adc +
                ", diff=" + diff +
                '}';
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        ADCVO adcvo = (ADCVO) o;
        return uid == adcvo.uid && adc == adcvo.adc && diff == adcvo.diff;
    }

    @Override
    public int hashCode() {
        return Objects.hash(uid, adc, diff);
    }

    /**
     * getter & setter
     */
    public int getUid() {
        return uid;
    }

    public void setUid(int uid) {
        this.uid = uid;
    }

    public int getAdc() {
        return adc;
    }

    public void setAdc(int adc) {
        this.adc = adc;
    }

    public int getDiff() {
        return diff;
    }

    public void setDiff(int diff) {
        this.diff = diff;
    }
}
