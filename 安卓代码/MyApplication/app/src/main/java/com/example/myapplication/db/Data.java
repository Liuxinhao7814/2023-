/**
 * This is Data.保存全局变量
 * @author fufeng
 * @Time 2023-6-22
 */

package com.example.myapplication.db;

import android.provider.BaseColumns;


public final class Data implements BaseColumns {
    /**
     * 数据库信息
     */
    public static final String SQLITE_NAME="Users.db";
    public static final int SQLITE_VERSION=1;
    /**
     * 信息表，及其字段
     */
    public static final String NodeArray_Table = "NodeArray";
    public static final String SensorData_Table = "SensorData";
    public static final String UserInfo_Table = "UserInfo";

    public static final String COLUMN_Index="_index";
    public static final String COLUMN_LoongSonID = "LoongSonID";
    public static final String COLUMN_NodeName="NodeName";
    public static final String COLUMN_NodeState = "NodeState";
    public static final String COLUMN_Time="Time";
    public static final String COLUMN_BatteryLevel="BatteryLevel";
    public static final String COLUMN_SensorNum = "SensorNum";
    public static final String COLUMN_SensorType0="SensorType0";
    public static final String COLUMN_SensorData0="SensorData0";
    public static final String COLUMN_SensorType1="SensorType1";
    public static final String COLUMN_SensorData1="SensorData1";
    public static final String COLUMN_SensorType2="SensorType2";
    public static final String COLUMN_SensorData2="SensorData2";
    public static final String COLUMN_SensorType3="SensorType3";
    public static final String COLUMN_SensorData3="SensorData3";
    public static final String COLUMN_SensorType4="SensorType4";
    public static final String COLUMN_SensorData4="SensorData4";
    public static final String COLUMN_SensorType5="SensorType5";
    public static final String COLUMN_SensorData5="SensorData5";
    public static final String COLUMN_SensorTypeArray="SensorTypeArray";
    public static final String COLUMN_ClientID = "Username";
    public static final String COLUMN_Password = "Password";
    public static final String COLUMN_SaveFlag = "SaveFlag";
    public static  final String[] COLUMN = new String[]{COLUMN_Index, COLUMN_LoongSonID, COLUMN_NodeName, COLUMN_Time,
            COLUMN_BatteryLevel, COLUMN_SensorNum, COLUMN_SensorType0, COLUMN_SensorData0, COLUMN_SensorType1,
            COLUMN_SensorData1, COLUMN_SensorType2, COLUMN_SensorData2, COLUMN_SensorType3, COLUMN_SensorData3,
            COLUMN_SensorType4, COLUMN_SensorData4, COLUMN_SensorType5, COLUMN_SensorData5};
    /**
     * 创建数据库的命令
     * */
    public static final String CREATE_SensorData_Table ="create table "+
            Data.SensorData_Table+"("+
            Data.COLUMN_Index+" integer primary key autoincrement,"+
            Data.COLUMN_LoongSonID+" TEXT(10),"+
            Data.COLUMN_NodeName+" TEXT(10),"+
            Data.COLUMN_Time+" TEXT(10),"+
            Data.COLUMN_BatteryLevel+" integer,"+
            Data.COLUMN_SensorNum+" integer,"+
            Data.COLUMN_SensorType0+" TEXT(10),"+
            Data.COLUMN_SensorData0+" real,"+
            Data.COLUMN_SensorType1+" TEXT(10),"+
            Data.COLUMN_SensorData1+" real,"+
            Data.COLUMN_SensorType2+" TEXT(10),"+
            Data.COLUMN_SensorData2+" real,"+
            Data.COLUMN_SensorType3+" TEXT(10),"+
            Data.COLUMN_SensorData3+" real,"+
            Data.COLUMN_SensorType4+" TEXT(10),"+
            Data.COLUMN_SensorData4+" real,"+
            Data.COLUMN_SensorType5+" TEXT(10),"+
            Data.COLUMN_SensorData5+" real"+
            ")";
    public static final String CREATE_IDArray_Table = "create table "+
            Data.NodeArray_Table+"("+
            Data.COLUMN_Index+" integer primary key autoincrement,"+
            Data.COLUMN_LoongSonID+" TEXT(10),"+
            Data.COLUMN_NodeName+" TEXT(10),"+
            Data.COLUMN_NodeState+" integer,"+
            Data.COLUMN_SensorNum+" integer,"+
            Data.COLUMN_SensorTypeArray+" TEXT"+
            ")";
    public static final String CREATE_USERINFO_Table = "create table "+
            Data.UserInfo_Table+"("+
            Data.COLUMN_Index+" integer primary key autoincrement,"+
            Data.COLUMN_ClientID +" TEXT,"+
            Data.COLUMN_Password+" TEXT"+
            ")";
    /**
     * MQTT主题定义
     * */
    public static final String MQTTReceiveMessage = "MQTTReceiveMessage";
    public static final String TopicLoongSonSync = "LoongSonSync";
    public static final String TopicLoongSonSyncRequest = "LoongSonSyncRequest";
    public static final String TopicNodeSync = "NodeSync";
    public static final String TopicNodeSyncRequest = "/NodeSyncRequest";
    public static final String TopicLoongSonWill = "LoongSonWill";
    public static final String TopicControlNodeCmd = "/ControlNodeCmd";
    public static final String TopicSyncNodeStatus = "SyncNodeStatus";
    public static final String TopicData = "Data";
    /**
     * OneNet平台信息
     * */
    public static final String MqttHost = "tcp://183.230.40.39:6002";
    public static final String Mqtt_Username ="601964";
    public static final int MqttConnect_Maximum = 3;
    /**
     * 数据表显示的最大历史数据量
     * */
    public static final int NumberHistoricalData = 50;
    /**
     * 时间字段的格式
     */
    public static final String DATE_FORMAT_Y="yyyy-MM-dd HH:mm:ss";
    public static final String DATE_FORMAT_D="HH:mm:ss";
    /**
     * 时间字段的降序，采用date函数比较
     */
    public static final String ORDER_BY="date("+COLUMN_Time+") desc";
}
