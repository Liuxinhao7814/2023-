/**
 * This is DataActivity.
 * @author fufeng
 * @Time 2023-6-22
 */

package com.example.myapplication.activity;

import android.annotation.SuppressLint;
import android.content.ContentValues;
import android.content.Intent;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.widget.CompoundButton;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.example.myapplication.R;
import com.example.myapplication.db.Data;
import com.example.myapplication.db.SQLiteManager;
import com.example.myapplication.listview.SellerBean;
import com.example.myapplication.onenet.MQTT;
import com.example.myapplication.waveform.DynamicLineChartManager;
import com.github.mikephil.charting.charts.LineChart;
import com.lingber.mycontrol.datagridview.DataGridView;

import org.eclipse.paho.client.mqttv3.MqttException;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

public class DataActivity extends AppCompatActivity implements MQTT.MessageListener{
    private TextView textView;
    private TextView batteryLevel;//用于显示电池电量
    private Switch switchButton;//用于开关节点
    private Handler handler;//用于处理MQTT返回的数据
    private DynamicLineChartManager dynamicLineChartManager1;//用于以折线图形式显示传感器数据
    private SimpleDateFormat df = new SimpleDateFormat(Data.DATE_FORMAT_Y);//设置日期格式
    private List<Integer> list = new ArrayList<>(); //数据集合
    private List<String> names = new ArrayList<>(); //折线名字集合
    private List<Integer> colar = new ArrayList<>();//折线颜色集合
    private int[] colarArray;//用于存储颜色
    private String LoongSonID;//用于接收启动Activity时传入的龙芯节点名
    private String NodeName;//用于接收启动Activity时传入的传感器节点名
    private String clientID;//用于接收启动Activity时传入的MQTT客户端号
    private String passWord;//用于接收启动Activity时传入的MQTT密钥
    private int sensorNum;//用于接收启动Activity时传入的传感器
    private SQLiteManager helper;
    private SQLiteDatabase db;
    private final Timer timer = new Timer();//用于定时执行折线图数据更新
    Cursor cursor1;
    private int maxHistoryNum;
    /**
     * 用于规避重复数据，因为数据上传时间是不固定的，但是定时器更新数据时间是固定的，所以每次查询最后一行数据时
     * 难免会查到上次的数据，所以通过记录上次成功查询的数据的索引，在这次查询时和上次的对比，就可以知道是不是新数据
     * */
    private int SensorDataIndex = 0;
    private MQTT mqtt;
    List<SellerBean> dataSource = new ArrayList();// 设置表格数据源

    @SuppressLint("Range")
    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.layout_data);

        Intent intent = getIntent();
        Bundle bundle = intent.getExtras();

        clientID = bundle.getString(Data.COLUMN_ClientID);
        passWord = bundle.getString(Data.COLUMN_Password);

        mqtt = new MQTT(this, clientID, passWord);

        colarArray = new int[7];
        colarArray[0] = Color.BLACK;
        colarArray[1] = Color.BLUE;
        colarArray[2] = Color.YELLOW;
        colarArray[3] = Color.RED;
        colarArray[4] = Color.GRAY;
        colarArray[5] = Color.GREEN;
        colarArray[6] = Color.LTGRAY;

        helper = new SQLiteManager(this);

        textView= (TextView) findViewById(R.id.tv2);
        textView.setText("I'm DataFragment");
        batteryLevel = (TextView)findViewById(R.id.batteryLevel);
        switchButton = (Switch) findViewById(R.id.switchButton);
        LineChart mChart1 = (LineChart) findViewById(R.id.dynamic_chart1);
        DataGridView mDataGridView = findViewById(R.id.datagridview);

        if(bundle != null) {
            textView.setText(bundle.getString(Data.COLUMN_NodeName));
            LoongSonID = bundle.getString(Data.COLUMN_LoongSonID);
            NodeName= bundle.getString(Data.COLUMN_NodeName);
            String SensorTypeArray = bundle.getString(Data.COLUMN_SensorTypeArray);
            sensorNum = bundle.getInt(Data.COLUMN_SensorNum);
            String[] str = SensorTypeArray.split("_");
            for(int i = 0; i < str.length; i++){
                names.add(str[i]);
                colar.add(colarArray[i]);
            }
        }

        /**
         * 初始化折线图，并设置折线、折线名称和折线颜色
         * */
        dynamicLineChartManager1 = new DynamicLineChartManager(mChart1, names, colar);
        names.clear();
        dynamicLineChartManager1.setYAxis(100, -20, 10);//设置折线图上下限和步进

        db = helper.getReadableDatabase();
        cursor1 = db.query(Data.SensorData_Table, null, Data.COLUMN_LoongSonID+"=? and " +
                Data.COLUMN_NodeName+"=?", new String[]{LoongSonID, NodeName}, null, null, null);

        if(cursor1.getCount() >=0 && cursor1.getCount() <= Data.NumberHistoricalData){
            maxHistoryNum = cursor1.getCount();
        }else {
            maxHistoryNum = Data.NumberHistoricalData;
        }

        boolean endFlag = true;
        int m = 0;
        /**
         * 以表格的形式显示历史数据
         * */
        if(cursor1.moveToLast()){
            do{
                if(m == maxHistoryNum){
                    endFlag = false;
                }else if(cursor1.getString(cursor1.getColumnIndex(Data.COLUMN_NodeName)).equals(NodeName)) {
                    double[] dataArray = new double[sensorNum];
                    for (int j = 0; j < sensorNum; j++) {
                        dataArray[j] = cursor1.getDouble(cursor1.getColumnIndex(Data.COLUMN[7 + 2 * j]));
                    }
                    Integer index = cursor1.getInt(cursor1.getColumnIndex(Data.COLUMN_Index));
                    String time = cursor1.getString(cursor1.getColumnIndex(Data.COLUMN_Time));
                    dataSource.add(new SellerBean(index, time, sensorNum, dataArray));
                }
                batteryLevel.setText("电量:"+String.valueOf(cursor1.getInt(cursor1.getColumnIndex(Data.COLUMN_BatteryLevel))));//显示电量
                m++;
            }while (cursor1.moveToPrevious() && endFlag);
        }
        cursor1.close();

        //以固定顺序存储表头
        int[] headArray = new int[]{R.string.index, R.string.time, R.string.Sensor0, R.string.Sensor1,
                R.string.Sensor2, R.string.Sensor3, R.string.Sensor4, R.string.Sensor5};
        int[] head = new int[sensorNum+2];
        float[] colunmWeight = new float[sensorNum+2];

         //给表头赋值，并设置每一列的宽度占比
        for (int i = 0; i < sensorNum+2; i++){
            head[i] = headArray[i];
            colunmWeight[i] = 1;
        }
        // 设置列数
        mDataGridView.setColunms(2+sensorNum);
        // 设置表头内容,必须对照
        mDataGridView.setHeaderContentByStringId(head);

        // 绑定字段
        mDataGridView.setFieldNames(new String[]{"index","time","sensor0","sensor1","sensor2",
                                                                "sensor3","sensor4", "sensor5"});
        // 每个column占比
        mDataGridView.setColunmWeight(colunmWeight);
        mDataGridView.setRowHeight(120); // 设置行高
        mDataGridView.setSortIsEnabled(new int[]{0}, true);
        // 每个单元格包含控件
        mDataGridView.setCellContentView(new Class[]{TextView.class, TextView.class, TextView.class,
                    TextView.class, TextView.class, TextView.class, TextView.class, TextView.class});
        // 设置数据源
        mDataGridView.setDataSource(dataSource);
        mDataGridView.setSortIsEnabled(new int[]{0}, true);//设置根据索引排序
//        mDataGridView.updateAll();
        // 单行选中模式
        mDataGridView.setSelectedMode(1);
        // 初始化表格
        mDataGridView.initDataGridView();


        /**
         * 设置SwitchButton点击事件为向MQTT发送节点控制命令，命令为控制当前节点是否发送数据，也就是控制节点的上下线
         * */
        switchButton.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if(switchButton.isChecked()){
                    Toast.makeText(DataActivity.this,"开启",Toast.LENGTH_SHORT).show();
                    db = helper.getReadableDatabase();
                    ContentValues values = new ContentValues();
                    values.put(Data.COLUMN_NodeState, 1);
                    db.update(Data.NodeArray_Table, values, Data.COLUMN_LoongSonID+"=? and " +
                            Data.COLUMN_NodeName+"=?", new String[]{LoongSonID, NodeName});
                    Bundle bundle = new Bundle();
                    bundle.putString(Data.COLUMN_LoongSonID, LoongSonID);
                    bundle.putString(Data.COLUMN_NodeName, NodeName);
                    bundle.putString(Data.TopicControlNodeCmd, "1");
                    sendControlCMD(bundle);
                }else {
                    Toast.makeText(DataActivity.this,"关闭",Toast.LENGTH_SHORT).show();
                    db = helper.getReadableDatabase();
                    ContentValues values = new ContentValues();
                    values.put(Data.COLUMN_NodeState, 0);
                    db.update(Data.NodeArray_Table, values, Data.COLUMN_LoongSonID+"=? and " +
                            Data.COLUMN_NodeName+"=?", new String[]{LoongSonID, NodeName});
                    Bundle bundle = new Bundle();
                    bundle.putString(Data.COLUMN_LoongSonID, LoongSonID);
                    bundle.putString(Data.COLUMN_NodeName, NodeName);
                    bundle.putString(Data.TopicControlNodeCmd, "0");
                    sendControlCMD(bundle);
                }
            }
        });


        /**
         * 设置定时器，定时查询数据库是否有新数据，有的话更新在折线图和表格中
         * */
        timer.scheduleAtFixedRate(new TimerTask() {
            @SuppressLint("Range")
            @Override
            public void run() {
// TODO Auto-generated method stub

                db = helper.getReadableDatabase();
                cursor1 = db.query(Data.SensorData_Table, null, Data.COLUMN_LoongSonID+"=? and " +
                        Data.COLUMN_NodeName+"=?", new String[]{LoongSonID, NodeName}, null, null, null);

                boolean i = true;
                int m;
                if(cursor1.moveToLast()){
                    do{
                        m = cursor1.getInt(cursor1.getColumnIndex(Data.COLUMN_Index));
                        if(cursor1.getString(cursor1.getColumnIndex(Data.COLUMN_NodeName)).equals(NodeName) && m>SensorDataIndex){
                            //batteryLevel.setText("电量:"+String.valueOf(cursor1.getInt(cursor1.getColumnIndex(Data.COLUMN_BatteryLevel))));//更新电量
                            double[] dataArray = new double[sensorNum];
                            Integer index = null;
                            String time = null;
                            for (int j = 0; j < sensorNum; j++){
                                list.add((int) cursor1.getDouble(cursor1.getColumnIndex(Data.COLUMN[7+2*j])));//更新数据
                                dataArray[j] = cursor1.getDouble(cursor1.getColumnIndex(Data.COLUMN[7 + 2 * j]));
                                index = cursor1.getInt(cursor1.getColumnIndex(Data.COLUMN_Index));
                                time = cursor1.getString(cursor1.getColumnIndex(Data.COLUMN_Time));
                            }
                            dataSource.add(new SellerBean(index, time, sensorNum, dataArray));
//                            mDataGridView.updateAll();
                            dynamicLineChartManager1.addEntry(list);
                            list.clear();
                            System.out.println(String.valueOf(m)+","+String.valueOf(SensorDataIndex));
                            System.out.println(cursor1.getString(cursor1.getColumnIndex(Data.COLUMN_NodeName)));
                            SensorDataIndex = cursor1.getInt(cursor1.getColumnIndex(Data.COLUMN_Index));//为下一轮做准备
                            i = false;
                        }
                    }while (cursor1.moveToPrevious() && i);
                }
                cursor1.close();
            }
        }, 100, 1000);

        /**
         * 处理MQTT返回数据的handler
         * */
        handler = new Handler() {

            @SuppressLint({"SetTextIl8n", "HandlerLeak"})

            public void handleMessage(Message msg) {

                super.handleMessage(msg);

                switch (msg.what) {

                    case 1: //开机校验更新回传

                        break;

                    case 2: //反馈回转

                        break;

                    case 3: //MQTT收到消息
                        String message = msg.getData().getString(Data.MQTTReceiveMessage);
                        String topic = (String) msg.obj;
                        if(topic.indexOf(Data.TopicLoongSonSync) != -1){
                            //收到同步的龙芯ID号，将其加入到下拉栏里
                            try {
                                mqtt.mqttSubscribe(message + "/" + Data.TopicSyncNodeStatus, 1);
                                mqtt.mqttSubscribe(message + "/" + Data.TopicNodeSync, 2);
                            } catch (MqttException e) {
                                e.printStackTrace();
                            }
                            //发出同步当前龙芯终端下节点信息的请求
                            mqtt.publishMessage(message + Data.TopicNodeSyncRequest, "1");

                        }else if (topic.indexOf(Data.TopicNodeSync) != -1) {
                            String[] str = message.split("~");//Tips:龙芯id号~节点id号~传感器数量~传感器类型_传感器类型
                            if (!message.equals("1")) {
                                ContentValues values = new ContentValues();
                                values.put(Data.COLUMN_LoongSonID, str[0]);
                                values.put(Data.COLUMN_NodeName, str[1]);
                                values.put(Data.COLUMN_NodeState, 1);//标记为在线
                                values.put(Data.COLUMN_SensorNum, str[2]);
                                values.put(Data.COLUMN_SensorTypeArray, str[3]);
                                db.insert(Data.NodeArray_Table, null, values);
                                try {
                                    mqtt.mqttSubscribe(str[0] + "/" + str[1] + "/#", 2);//Tips:loongson0/01/#
                                } catch (MqttException e) {
                                    e.printStackTrace();
                                }
                            } else {
                                try {
                                    mqtt.mqttSubscribe(str[0] + "/" + Data.TopicSyncNodeStatus, 2);
                                } catch (MqttException e) {
                                    e.printStackTrace();
                                }
                            }
                            textView.setText(message);
                        } else if (topic.indexOf(Data.TopicLoongSonWill) != -1) { //收到龙芯遗嘱
                            String[] str = message.split("_");  //Tips:loongson0_UPLine
                            if (str[1].equals("DownLine")) {
                                textView.setText(str[0] + ",请处理异常");
                            }
                        } else if (topic.indexOf(Data.TopicSyncNodeStatus) != -1) {
                            //更改相应的节点状态到数据库
                            String[] str = message.split("_");
                            ContentValues values = new ContentValues();
                            values.put(Data.COLUMN_NodeState, str[2]);
                            db.update(Data.NodeArray_Table, values, Data.COLUMN_LoongSonID+"=? and " +
                                    Data.COLUMN_NodeName+"=?", new String[]{str[0], str[1]});
                        } else if(topic.indexOf(Data.TopicData) != -1){
                            /**
                             * //Tips:龙芯ID_id号_时间_电池电量_传感器数量_传感器类型_传感器数据_传感器类型_传感器数据
                             * */
                            String[] str = message.split("_");
                            ContentValues values = new ContentValues();
                            values.put(Data.COLUMN_LoongSonID, str[0]);
                            values.put(Data.COLUMN_NodeName, str[1]);
                            values.put(Data.COLUMN_Time, df.format(System.currentTimeMillis()).toString());
                            values.put(Data.COLUMN_BatteryLevel, str[2]);
                            values.put(Data.COLUMN_SensorNum, str[3]);
                            for (int i = 0; i < Integer.valueOf(str[3]).intValue(); i++) {
                                values.put(Data.COLUMN[6 + i * 2], str[4 + i * 2]);
                                values.put(Data.COLUMN[7 + i * 2], str[5 + i * 2]);
                            }
                            db.insert(Data.SensorData_Table, null, values);
                            textView.setText(message);
                        }


                        break;

                    case 30: //连接失败

                        Toast.makeText(DataActivity.this, "连接失败", Toast.LENGTH_SHORT).show();

                        break;

                    case 31: //连接成功

                        Toast.makeText(DataActivity.this, "连接成功", Toast.LENGTH_SHORT).show();

                        try {

                            mqtt.mqttSubscribe(Data.TopicLoongSonSync, 2);
                            mqtt.mqttSubscribe(Data.TopicLoongSonWill, 2);
                        } catch (MqttException e) {

                            e.printStackTrace();

                        }
                        /**
                         * 发出同步请求
                         * */
                        mqtt.publishMessage(Data.TopicLoongSonSyncRequest, "1");

                        break;
                    default:

                        break;
                }
            }
        };
    }

    @Override
    protected void onPause() {
        super.onPause();
        timer.cancel();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        timer.cancel();
        db.close();
        mqtt.onDestroy();
    }

    /**
     * param bundle 构造命令所需的数据
     * Description 发送控制节点的命令给MQTT服务器
     * */
    private void sendControlCMD(Bundle bundle){
        String LoongsonID = bundle.getString(Data.COLUMN_LoongSonID);
        String NodeName = bundle.getString(Data.COLUMN_NodeName);
        String cmd = bundle.getString(Data.TopicControlNodeCmd);
        System.out.println(NodeName+"_"+cmd);
        mqtt.publishMessage(LoongsonID + Data.TopicControlNodeCmd,NodeName+"_"+cmd);//Tips:节点名_命令
    }

    /**
     * 用于MQTT信息传递
     * */
    @Override
    public void sendMessage(Message msg) {
        handler.sendMessage(msg);
    }
}
