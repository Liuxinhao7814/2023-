/**
 * This is SQLiteManager.
 * @author fufeng
 * @Time 2023-6-22
 */

package com.example.myapplication.db;

import android.content.Context;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

public class SQLiteManager extends SQLiteOpenHelper {
    private Context mContext;
    public SQLiteManager(Context context){
        super(context, Data.SQLITE_NAME, null, Data.SQLITE_VERSION);
    }
    //创建数据库
    public void onCreate(SQLiteDatabase db){
        db.execSQL(Data.CREATE_IDArray_Table);
        db.execSQL(Data.CREATE_SensorData_Table);
        db.execSQL(Data.CREATE_USERINFO_Table);
    }
    //数据库版本更新
    public void onUpgrade(SQLiteDatabase db,int oldVersion,int newVersion)
    {

    }
}


