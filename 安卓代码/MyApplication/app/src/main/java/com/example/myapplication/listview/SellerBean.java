/**
 * This is SellerBean.用于构建表格数据源的格式
 * @author fufeng
 * @Time 2023-6-22
 */

package com.example.myapplication.listview;


public class SellerBean {
    private Integer index;
    private String time;
    private double sensor0 = 0;
    private double sensor1 = 0;
    private double sensor2 = 0;
    private double sensor3 = 0;
    private double sensor4 = 0;

    /**
     * @param index 数据索引
     * @param sensorNum 传感器数量
     * @param data 传感器数据
     * */
    public SellerBean(Integer index, String time, int sensorNum, double[] data) {
        this.index = index;
        this.time = time;
        switch (sensorNum){
            case 1:
                this.sensor0 = data[0];
                break;
            case 2:
                this.sensor0 = data[0];
                this.sensor1 = data[1];
                break;
            case 3:
                this.sensor0 = data[0];
                this.sensor1 = data[1];
                this.sensor2 = data[2];
                break;
            case 4:
                this.sensor0 = data[0];
                this.sensor1 = data[1];
                this.sensor2 = data[2];
                this.sensor3 = data[3];
                break;
            case 5:
                this.sensor0 = data[0];
                this.sensor1 = data[1];
                this.sensor2 = data[2];
                this.sensor3 = data[3];
                this.sensor4 = data[4];
                break;
            default:break;
        }
    }
}

