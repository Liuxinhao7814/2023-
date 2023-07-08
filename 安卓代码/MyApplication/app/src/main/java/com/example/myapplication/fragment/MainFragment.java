/**
 * This is MainFragment.
 * @author fufeng
 * @Time 2023-6-22
 */

package com.example.myapplication.fragment;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.Fragment;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.TextView;

import androidx.annotation.Nullable;

import com.example.myapplication.R;
import com.example.myapplication.db.Data;
import com.example.myapplication.db.SQLiteManager;

import java.util.ArrayList;
import java.util.List;


public class MainFragment extends Fragment{
    private Context mContext = null;
    private Activity mActivity = null;
    private TextView textView;
    private android.widget.ListView listView;
    private SQLiteDatabase db;
    private static List<String> listViewItem = new ArrayList<String>();
    private String[] NodeNameArray;//保存节点ID
    private String LoongSonID;//保存龙芯ID
    private SQLiteManager helper;
    private MainFragmentListener listener;

    /**
     *用于与Activity的数据传递
     * */
    public interface MainFragmentListener{
        void OpenDataActivity(Bundle bundle);//用于通过MainActivity打开DataActivity
    }

    public MainFragment() {
    }

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.layout_main,container,false);
        mContext = getContext();
        mActivity = getActivity();
        NodeNameArray = new String[50];
        helper = new SQLiteManager(mContext);
        return view;
    }

    @SuppressLint("Range")
    @Override
    public void onActivityCreated(Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        textView= (TextView)mActivity.findViewById(R.id.tv1);
        listView = (android.widget.ListView)mActivity.findViewById(R.id.listView);
        if(getArguments() != null){
            LoongSonID = getArguments().getString(Data.COLUMN_LoongSonID);
            textView.setText(LoongSonID);
        }

        /**
         * 添加节点
         * */
        db = helper.getReadableDatabase();
        Cursor cursor1 = db.query(Data.NodeArray_Table, new String[]{Data.COLUMN_NodeName},
                Data.COLUMN_LoongSonID+"=?", new  String[]{ LoongSonID }, null, null, null);
        listViewItem.clear();
        if(cursor1.moveToFirst()){
            int i = 0;
            do{
                System.out.println(cursor1.getString(cursor1.getColumnIndex(Data.COLUMN_NodeName)));
                listViewItem.add(cursor1.getString(cursor1.getColumnIndex(Data.COLUMN_NodeName)));
                NodeNameArray[i] = cursor1.getString(cursor1.getColumnIndex(Data.COLUMN_NodeName));
                i++;
            }while (cursor1.moveToNext());
        }
        cursor1.close();

        ArrayAdapter<String> adapter = new ArrayAdapter<String>(mContext,
                android.R.layout.simple_list_item_1, listViewItem);
        listView.setAdapter(adapter);
        /**
         * 设置点击事件
         * */
        AdapterView.OnItemClickListener listViewListener = new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
                Bundle bundle = new Bundle();
                db = helper.getReadableDatabase();
                Cursor cursor1 = db.query(Data.NodeArray_Table, null, Data.COLUMN_LoongSonID +
                        "=?", new String[]{LoongSonID}, null, null, null);
                String SensorTypeArray = null;
                int sensorNum = 0;
                boolean a = true;

                if(cursor1.moveToFirst()){
                    do{
                        if(cursor1.getString(cursor1.getColumnIndex(Data.COLUMN_NodeName)).equals(NodeNameArray[i])){
                            SensorTypeArray = cursor1.getString(cursor1.getColumnIndex(Data.COLUMN_SensorTypeArray));
                            sensorNum = cursor1.getInt(cursor1.getColumnIndex(Data.COLUMN_SensorNum));
                            a = false;
                        }
                    }while (cursor1.moveToNext() && a);
                }
                cursor1.close();

                bundle.putString(Data.COLUMN_LoongSonID, LoongSonID);
                bundle.putString(Data.COLUMN_NodeName, NodeNameArray[i]);
                bundle.putInt(Data.COLUMN_SensorNum, sensorNum);
                bundle.putString(Data.COLUMN_SensorTypeArray, SensorTypeArray);

                textView.setText(NodeNameArray[i] + "被选中");
                /**
                 * 为了更新传递过去的数据，每次点击都需要重建bundle
                 * */
                listener.OpenDataActivity(bundle);
            }
        };
        listView.setOnItemClickListener(listViewListener);
    }

    public static  MainFragment newInstance(Bundle bundle){
        MainFragment fragment = new MainFragment();
        fragment.setArguments(bundle);
        return fragment;
    }

    @Override
    public void onAttach(Context context) {
        super.onAttach(context);
        try{
            listener = (MainFragment.MainFragmentListener) context;
        }catch(ClassCastException e){
            e.printStackTrace();
        }
    }
}
