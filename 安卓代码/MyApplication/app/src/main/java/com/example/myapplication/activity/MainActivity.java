/**
 * This is MainActivity.
 * @author fufeng
 * @Time 2023-6-22
 */

package com.example.myapplication.activity;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.ContentValues;
import android.content.Intent;
import android.database.sqlite.SQLiteDatabase;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import androidx.activity.result.ActivityResult;
import androidx.activity.result.ActivityResultCallback;
import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.appcompat.app.AppCompatActivity;

import com.example.myapplication.R;
import com.example.myapplication.db.Data;
import com.example.myapplication.db.SQLiteManager;
import com.example.myapplication.fragment.MainFragment;
import com.example.myapplication.login.LoginActivity;
import com.example.myapplication.onenet.MQTT;

import org.eclipse.paho.client.mqttv3.MqttException;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.List;


public class MainActivity extends AppCompatActivity implements MQTT.MessageListener,
                                                                MainFragment.MainFragmentListener {
    private MainFragment mainFragment;//显示传感器节点列表的Fragment
    private Spinner spinner;//显示龙芯节点的下拉栏控件
    private TextView textView;
    private Button refresh;//刷新传感器节点列表的Button控件
    private Button login;//用于登录的Button控件
    private SimpleDateFormat df = new SimpleDateFormat(Data.DATE_FORMAT_Y);//设置日期格式
    private Handler handler;//处理MQTT服务器返回消息的handler
    private SQLiteDatabase db;//SQLite
    private String clientID = null;//MQTT客户端ID
    private String passWord = null;//MQTT客户端密钥
    private static List<String> LoongSonItem = new ArrayList<String>();//存储龙芯节点名的容器
    private boolean saveFlag;//登录界面返回的是否保存密码的标志位
    private MQTT mqtt;
    private SQLiteManager dataHelper;//SQLite
    ActivityResultLauncher<Intent> DataActivityLauncher;//用于启动数据显示Activity
    ActivityResultLauncher<Intent> intentActivityResultLauncher;//用于启动登录Activity

    @SuppressLint("MissingInflatedId")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        textView = (TextView)findViewById(R.id.tv);

        login = (Button)findViewById(R.id.login);
        /**
         * 设置Button点击事件为启动登录
         * */
        login.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(MainActivity.this, LoginActivity.class);
                intentActivityResultLauncher.launch(intent);
            }
        });

        refresh = (Button)findViewById(R.id.refresh);
        /**
         * 设置Button点击事件为刷新Spinner
         * */
        refresh.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                List<String> oldItem = new ArrayList<String>();
                ArrayAdapter<String> adapter = new ArrayAdapter<String>(MainActivity.this,
                        android.R.layout.simple_list_item_1, LoongSonItem);
                spinner.setAdapter(adapter);
            }
        });
        spinner = (Spinner)findViewById(R.id.spinner);
        /**
         * 设置Spinner点击事件为刷新mainFragment为当前选定的龙芯节点下的传感器节点
         * */
        spinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                String loongsonID = parent.getItemAtPosition(position).toString();
                Bundle bundle = new Bundle();
                bundle.putString(Data.COLUMN_LoongSonID, loongsonID);
                mainFragment = MainFragment.newInstance(bundle);
                getFragmentManager().beginTransaction().replace(R.id.linearLayout, mainFragment,
                        "mainFragment").commitAllowingStateLoss();
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {

            }
        });

        LoongSonItem.clear();

        dataHelper = new SQLiteManager(this);
        db = dataHelper.getWritableDatabase();
        /**
         * 删除所有缓存的节点信息，等待最新的节点信息
         * */
        db.delete(Data.NodeArray_Table, null, null);

        /**
         * 初始化DataActivityLauncher
         * */
        DataActivityLauncher = registerForActivityResult(
                new ActivityResultContracts.StartActivityForResult(),
                new ActivityResultCallback<ActivityResult>() {
                    @Override
                    public void onActivityResult(ActivityResult result) {

                    }
                });

        /**
         * 初始化intentActivityResultLauncher，并处理返回数据
         * */
        intentActivityResultLauncher = registerForActivityResult(
                new ActivityResultContracts.StartActivityForResult(),
                new ActivityResultCallback<ActivityResult>() {
            @Override
            public void onActivityResult(ActivityResult result) {
                if (result.getData() != null && result.getResultCode() == Activity.RESULT_OK){
                    clientID = result.getData().getStringExtra(Data.COLUMN_ClientID);
                    passWord = result.getData().getStringExtra(Data.COLUMN_Password);
                    saveFlag = result.getData().getBooleanExtra(Data.COLUMN_SaveFlag, false);
                }
            }
        });
        Intent intent = new Intent(MainActivity.this, LoginActivity.class);
        intentActivityResultLauncher.launch(intent);
    }

    @Override
    protected void onResume() {
        super.onResume();
        /**
         * 因为每次Resume都需要重新连接MQTT服务器，并请求数据，所以需要删除之前的节点信息，不然就会有重复
         * */
        LoongSonItem.clear();
        db.delete(Data.NodeArray_Table, null, null);

        if((clientID != null && clientID.length() != 0) && (passWord != null && passWord.length() != 0)){
            System.out.println("New MQTT");
            mqtt = new MQTT(MainActivity.this, clientID, passWord, saveFlag);
        }

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
                            LoongSonItem.add(message);
                            ArrayAdapter<String> adapter = new ArrayAdapter<String>(MainActivity.this,
                                    android.R.layout.simple_list_item_1, LoongSonItem);
                            spinner.setAdapter(adapter);
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
                        } else if (topic.indexOf(Data.TopicLoongSonWill) != -1) { //收到龙芯遗嘱
                            String[] str = message.split("_");  //Tips:loongson0_UPLine
                            if (str[1].equals("DownLine")) {
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
                        }
                        textView.setText(message);

                        break;

                    case 30: //连接失败

                        Toast.makeText(MainActivity.this, "连接失败", Toast.LENGTH_SHORT).show();

                        break;

                    case 31: //连接成功

                        Toast.makeText(MainActivity.this, "连接成功", Toast.LENGTH_SHORT).show();

                        try {
                            /**
                             * 订阅同步龙芯节点信息主题和遗嘱主题
                             * */
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
    protected void onDestroy() {
        super.onDestroy();
        if(mqtt != null){
            System.out.println("Destroy");
            db.close();
            mqtt.onDestroy();
        }
    }

    /**
     * 用于MQTT信息传递
     * */
    @Override
    public void sendMessage(Message msg) {
        handler.sendMessage(msg);
    }

    /**
     * 用于mainFragment打开数据显示Activity
     * */
    @Override
    public void OpenDataActivity(Bundle bundle) {
        Intent intent = new Intent(MainActivity.this, DataActivity.class);

        bundle.putString(Data.COLUMN_ClientID, clientID);
        bundle.putString(Data.COLUMN_Password, passWord);
        intent.putExtras(bundle);
        DataActivityLauncher.launch(intent);
    }
}