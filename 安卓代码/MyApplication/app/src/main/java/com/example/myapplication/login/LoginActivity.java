/**
 * This is LoginActivity.登录界面
 * @author fufeng
 * @Time 2023-6-22
 */

package com.example.myapplication.login;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import com.example.myapplication.R;
import com.example.myapplication.db.Data;
import com.example.myapplication.db.SQLiteManager;

public class LoginActivity extends AppCompatActivity {

    EditText et_username;  //客户端ID输入
    EditText et_password;  //密码输入
    CheckBox cb_rm;//是否保存客户端信息
    Button btn_login;
    private SQLiteDatabase db;
    private SQLiteManager dataHelper;
    private String ClientID = null;
    private String Password = null;

    @SuppressLint("Range")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);

        et_username = findViewById(R.id.et_username);   //获取输入的客户端ID
        et_password = findViewById(R.id.et_password);     //获取输入的密码
        cb_rm = findViewById(R.id.cb_rm);
        btn_login = findViewById(R.id.btn_login);
        dataHelper = new SQLiteManager(LoginActivity.this);
        db = dataHelper.getReadableDatabase();

        /**
         * 显示上次被保存的账号信息
         * */
        Cursor cursor = db.query(Data.UserInfo_Table, null,
                null, null, null, null, null);
        if(cursor.moveToLast()){
            do{
                ClientID = cursor.getString(cursor.getColumnIndex(Data.COLUMN_ClientID));
                Password = cursor.getString(cursor.getColumnIndex(Data.COLUMN_Password));
                et_username.setText(ClientID);
                et_password.setText(Password);
            }while (cursor.moveToNext());
        }
        cursor.close();

        /**
         * 设置Button点击事件
         * */
        btn_login.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String username = et_username.getText().toString().trim();
                String pwd = et_password.getText().toString().trim();
                boolean saveFlag;
                if(cb_rm.isChecked() == true){
                    saveFlag = true;
                }else {
                    saveFlag = false;
                }
                if((username == null || username.length() == 0) && (pwd == null || pwd.length() == 0)){
                    Toast.makeText(LoginActivity.this, "信息为空", Toast.LENGTH_SHORT).show();
                }
                else if(username == null || username.length() == 0){
                    Toast.makeText(LoginActivity.this, "用户名为空", Toast.LENGTH_SHORT).show();
                }else if(pwd == null || pwd.length() == 0){
                    Toast.makeText(LoginActivity.this, "密码为空", Toast.LENGTH_SHORT).show();
                }else {
                    System.out.println("finish");
                    Intent result = new Intent();
                    result.putExtra(Data.COLUMN_ClientID, username);
                    result.putExtra(Data.COLUMN_Password, pwd);
                    result.putExtra(Data.COLUMN_SaveFlag, saveFlag);
                    setResult(RESULT_OK, result);
                    finish();
                }
            }
        });
    }
}

