package me.hypocrite30.serialport.easyexcel;

import com.alibaba.excel.context.AnalysisContext;
import com.alibaba.excel.event.AnalysisEventListener;

/**
 * EasyExcel 监听器
 *
 * @Author: Hypocrite30
 * @Date: 2022/1/9 20:08
 */
public class ExcelListener extends AnalysisEventListener<ADCVO> {

    // 一行行读取 excel 内容，从第二行开始读取
    @Override
    public void invoke(ADCVO adcvo, AnalysisContext analysisContext) {
        System.out.println(adcvo);
    }

    @Override
    public void doAfterAllAnalysed(AnalysisContext analysisContext) {
        System.out.println("读取excel结束...");
    }
}
