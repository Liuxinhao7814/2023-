/**
 * This is MQTT.
 * @author fufeng
 * @Time 2023-6-22
 */

package com.example.myapplication.onenet;

import android.annotation.SuppressLint;
import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.os.Bundle;
import android.os.Message;

import com.example.myapplication.db.Data;
import com.example.myapplication.db.SQLiteManager;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;

import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

public class MQTT {
    //    private String clientID = "1079864316";
//    private String passWord = "222222";
    private String clientID = null;
    private String passWord = null;
    private String ClientID = null;
    private String Password = null;
    private boolean saveFlag;

    public MqttClient client;
    private MqttConnectOptions options;
    private ScheduledExecutorService scheduler;
    private MessageListener messageListener;
    private int connectNum;//连接次数计数
    private SQLiteDatabase db;
    private SQLiteManager dataHelper;

    public interface MessageListener{
        void sendMessage(Message msg);
    }

    /**
     * 用于DataActivity初始化mqtt
     * */
    public MQTT(Context context, String clientID, String passWord){
        this.clientID = clientID;
        this.passWord = passWord;
        try{
            messageListener = (MQTT.MessageListener) context;
        }catch(ClassCastException e){
            e.printStackTrace();
        }
        mqttInit();
        mqttConnect();
    }

    /**
     * 用于MainActivity初始化mqtt
     * */
    @SuppressLint("Range")
    public MQTT(Context context, String clientID, String passWord, boolean saveFlag){
        this.clientID = clientID;
        this.passWord = passWord;
        this.saveFlag = saveFlag;
        connectNum = 0;
        try{
            messageListener = (MQTT.MessageListener) context;
        }catch(ClassCastException e){
            e.printStackTrace();
        }
        dataHelper = new SQLiteManager(context);
        db = dataHelper.getReadableDatabase();
        Cursor cursor = db.query(Data.UserInfo_Table, null,
                null, null, null, null, null);
        if(cursor.moveToLast()){
            do{
                ClientID = cursor.getString(cursor.getColumnIndex(Data.COLUMN_ClientID));
                Password = cursor.getString(cursor.getColumnIndex(Data.COLUMN_Password));
            }while (cursor.moveToNext());
        }
        mqttInit();
        mqttConnect();
    }

    private void mqttInit() {
        try {
            //host为主机名，test为clientid即连接MQTT的客户端ID，一般以客户端唯一标识符表示，MemoryPersistence设置clientid的保存形式，默认为以内存保存
            client = new MqttClient(Data.MqttHost, clientID,
                    new MemoryPersistence());

            //MQTT的连接设置
            options = new MqttConnectOptions();

            //设置是否清空session,这里如果设置为false表示服务器会保留客户端的连接记录，这里设置为true表示每次连接到服务器都以新的身份连接
            options.setCleanSession(true);

            //设置连接的用户名
            options.setUserName(Data.Mqtt_Username);

            //设置连接的密码
            options.setPassword(passWord.toCharArray());

            // 设置超时时间 单位为秒
            options.setConnectionTimeout(10);

            // 设置会话心跳时间 单位为秒 服务器会每隔1.5*20秒的时间向客户端发送个消息判断客户端是否在线，但这个方法并没有重连的机制
            options.setKeepAliveInterval(20);

            //设置回调
            client.setCallback(new MqttCallback() {

                @Override

                public void connectionLost(Throwable cause) {
                    //连接丢失后，一般在这里面进行重连
                    System.out.println("connectionLost----------");

                }

                @Override

                public void deliveryComplete(IMqttDeliveryToken token) {

                    //publish后会执行到这里

                    System.out.println("deliveryComplete---------"
                            + token.isComplete());
                }
                @Override

                public void messageArrived(String topicName, MqttMessage message)
                        throws Exception {

                    //subscribe后得到的消息会执行到这里面

                    System.out.println("messageArrived----------");
                    System.out.println(topicName);
                    Message msg = new Message();
                    Bundle bundle = new Bundle();

                    msg.what = 3;
                    msg.obj = topicName;
                    bundle.putString(Data.MQTTReceiveMessage, message.toString());
                    msg.setData(bundle);

                    messageListener.sendMessage(msg);
                }
            });
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void mqttConnect() {
        new Thread(new Runnable() {

            @Override

            public void run() {
                try {
                    if (!client.isConnected()){

                        client.connect(options);

                        Message msg = new Message();

                        msg.what=31;

                        messageListener.sendMessage(msg);
                        /**
                         * 如果连接成功，并且初始化时传递过来的信息和上次保存的信息不同，并且在登录时勾选了保存信息
                         * 那就会保存本次登录的信息
                         * */
                        if(client.isConnected() && !clientID.equals(ClientID) && !passWord.equals(Password) && saveFlag){
                            System.out.println("isConnected");
                            db = dataHelper.getWritableDatabase();
                            ContentValues values = new ContentValues();
                            values.put(Data.COLUMN_ClientID, clientID);
                            values.put(Data.COLUMN_Password, passWord);
                            db.insert(Data.UserInfo_Table, null, values);
                            db.close();
                            saveFlag = false;
                        }
                    }
                } catch (Exception e) {

                    e.printStackTrace();

                    Message msg = new Message();

                    msg.what = 30;

                    messageListener.sendMessage(msg);
                }
            }
        }).start();
    }

    /**
     * 创建线程连接MQTT服务器
     * */
    private void startReconnect() {
        scheduler = Executors.newSingleThreadScheduledExecutor();

        scheduler.scheduleAtFixedRate(new Runnable() {
            @Override

            public void run() {
                if (!client.isConnected() && connectNum < Data.MqttConnect_Maximum) {
                    System.out.println("connectNum = " + connectNum);
                    mqttConnect();
                    connectNum++;
                }
            }
        }, 0 * 1000, 10 * 1000, TimeUnit.MILLISECONDS);

    }

    public void onDestroy() {
        try {
            client.disconnect();
        }catch (Exception e){
            e.printStackTrace();
        }
    }

    /**
     * param topicFilter 需要订阅的主题
     * param qos 订阅等级
     * see 订阅主题
     * */
    public void mqttSubscribe(String topicFilter, int qos) throws MqttException {
        client.subscribe(topicFilter, qos);
    }

    /**
     * param topic 指定的主题
     * param message2 需要发布的信息
     * see 发布信息
     * */
    public void publishMessage(String topic,String message2) {
        if (client == null || !client.isConnected()) {
            return;
        }

        MqttMessage message = new MqttMessage();

        message.setPayload(message2.getBytes());

        try {
            client.publish(topic,message);
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }
}
