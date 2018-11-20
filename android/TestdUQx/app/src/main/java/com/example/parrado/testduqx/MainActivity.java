/*
 * dUQx Java API file
 *
 * This file is part of dUQx.
 *
 * Copyright (c) 2018 Alexander López Parrado
 *
 * dUQx is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * dUQx is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with dUQx.  If not, see <https://www.gnu.org/licenses/>.
 */
package com.example.parrado.testduqx;


import com.example.parrado.ThingSpeak;
import com.example.parrado.dUQx;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.os.Bundle;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;
import java.util.concurrent.Semaphore;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.List;
import java.util.Set;
import java.util.UUID;

import android.preference.PreferenceManager;

public class MainActivity extends AppCompatActivity implements Toolbar.OnMenuItemClickListener {

    private static final int REQUEST_ENABLE_BT = 1;



    BluetoothAdapter bluetoothAdapter;

    ArrayList<BluetoothDevice> pairedDeviceArrayList;


    ListView listViewPairedDevice;

    TextView textStatus;
    LinearLayout inputPane;
    ProgressBar progressBar;
    Button btnSend;

    ArrayAdapter<BluetoothDevice> pairedDeviceAdapter;
    private UUID myUUID;
    private final String UUID_STRING_WELL_KNOWN_SPP =
            "00001101-0000-1000-8000-00805F9B34FB";

    ThreadConnectBTdevice myThreadConnectBTdevice;
    ThreaddUQx myThreaddUQx;
    Semaphore flagSemaphore;
    Semaphore timeSemaphore;
    boolean flag = false;


    dUQx mydUQx;


    private ThingSpeak thingSpeak;

    private  SharedPreferences prefs;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


        textStatus = (TextView) findViewById(R.id.status);




        inputPane = (LinearLayout) findViewById(R.id.inputpane);
        progressBar = (ProgressBar) findViewById(R.id.progressBar);
        btnSend = (Button) findViewById(R.id.send);
        listViewPairedDevice = (ListView)findViewById(R.id.pairedlist);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        toolbar.setTitleTextColor(Color.WHITE);
        toolbar.setTitle("dUQx");
        toolbar.inflateMenu(R.menu.toolbar);
        toolbar.setOnMenuItemClickListener(this);

        progressBar.setVisibility(View.GONE);


        thingSpeak=new ThingSpeak();
        prefs = PreferenceManager.getDefaultSharedPreferences(this);

        myThreaddUQx = new ThreaddUQx();

        flagSemaphore=new Semaphore(1);
        timeSemaphore=new Semaphore(1);







        btnSend.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {

                if(btnSend.getText().toString().equals("START LOGGING")) {

                    try {
                        flagSemaphore.acquire();
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }

                        flag=true;
                   flagSemaphore.release();


                    btnSend.setText("STOP LOGGING");
                }else {

                    btnSend.setText("START LOGGING");

                    try {
                        flagSemaphore.acquire();
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }

                    flag=false;
                    flagSemaphore.release();


                }

            }
        });

        if (!getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH)) {
            Toast.makeText(this,
                    "FEATURE_BLUETOOTH NOT supported",
                    Toast.LENGTH_LONG).show();
            finish();
            return;
        }

        //using the well-known SPP UUID
        myUUID = UUID.fromString(UUID_STRING_WELL_KNOWN_SPP);

        bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if (bluetoothAdapter == null) {
            Toast.makeText(this,
                    "Bluetooth is not supported on this hardware platform",
                    Toast.LENGTH_LONG).show();
            finish();
            return;
        }

        String stInfo = bluetoothAdapter.getName() + "\n" +
                bluetoothAdapter.getAddress();

        setup();

    }

    @Override
    public boolean onMenuItemClick(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.action_settings:

                Intent intent = new Intent(this, SettingsActivity.class);
                startActivity(intent);

                return true;

            case R.id.action_about:
                AlertDialog alertDialog = new AlertDialog.Builder(MainActivity.this).create();
                alertDialog.setTitle("About");
                alertDialog.setMessage("dUQx is work of Alexander López-Parrado (2009-2018)\nUniversidad del Quindío\nGDSPROC");
                alertDialog.setButton(AlertDialog.BUTTON_NEUTRAL, "OK",
                        new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int which) {
                                dialog.dismiss();
                            }
                        });
                alertDialog.show();

                return true;

            case R.id.action_exit:

                this.finish();

                return  true;

            default:
                return false;
        }
    }

    @Override
    protected void onStart() {
        super.onStart();

        //Turn ON BlueTooth if it is OFF
        if (!bluetoothAdapter.isEnabled()) {
            Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableIntent, REQUEST_ENABLE_BT);
        }

      //  setup();
    }

    private void setup() {
        textStatus.setText("Please select paired HC-06 attached to dUQx");
        Set<BluetoothDevice> pairedDevices = bluetoothAdapter.getBondedDevices();
        if (pairedDevices.size() > 0) {
            List<String> s = new ArrayList<String>();
            pairedDeviceArrayList = new ArrayList<BluetoothDevice>();
            for(BluetoothDevice bt : pairedDevices) {
                s.add(bt.getName());
                pairedDeviceArrayList.add(bt);
            }

            listViewPairedDevice.setAdapter(new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, s));


            listViewPairedDevice.setOnItemClickListener(new AdapterView.OnItemClickListener() {

                @Override
                public void onItemClick(AdapterView<?> parent, View view,
                                        int position, long id) {
                    BluetoothDevice device =pairedDeviceArrayList.get(position) ;


                    Toast.makeText(MainActivity.this,
                            "Connecting to " + device.getName() + " attached to dUQx",
                            Toast.LENGTH_LONG).show();

                    textStatus.setTextColor(Color.rgb(0x00,0x92,0x3f));
                    textStatus.setText("start ThreadConnectBTdevice");
                    myThreadConnectBTdevice = new ThreadConnectBTdevice(device);
                    myThreadConnectBTdevice.start();
                }
            });
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        if (myThreadConnectBTdevice != null) {
            myThreadConnectBTdevice.cancel();
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == REQUEST_ENABLE_BT) {
            if (resultCode == Activity.RESULT_OK) {
                setup();
            } else {
                Toast.makeText(this,
                        "BlueTooth NOT enabled",
                        Toast.LENGTH_SHORT).show();
                finish();
            }
        }
    }


    /*
    ThreadConnectBTdevice:
    Background Thread to handle BlueTooth connecting
    */
    private class ThreadConnectBTdevice extends Thread {

        private BluetoothSocket bluetoothSocket = null;
        private final BluetoothDevice bluetoothDevice;


        private ThreadConnectBTdevice(BluetoothDevice device) {
            bluetoothDevice = device;

            try {
                bluetoothSocket = device.createRfcommSocketToServiceRecord(myUUID);
                textStatus.setTextColor(Color.rgb(0x00,0x92,0x3f));
                textStatus.setText("Bluetooth socket succeeded");
            } catch (IOException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
        }

        @Override
        public void run() {
            boolean success = false;
            try {
                bluetoothSocket.connect();
                success = true;
            } catch (IOException e) {
                e.printStackTrace();

                final String eMessage = e.getMessage();
                runOnUiThread(new Runnable() {

                    @Override
                    public void run() {
                        textStatus.setTextColor(Color.RED);
                        textStatus.setText("Problem while connecting to dUQx");
                    }
                });

                try {
                    bluetoothSocket.close();
                } catch (IOException e1) {
                    // TODO Auto-generated catch block
                    e1.printStackTrace();
                }
            }

            if (success) {
                //connect successful
                final String msgconnected = "Connection to dUQx succeded";

                runOnUiThread(new Runnable() {

                    @Override
                    public void run() {
                        textStatus.setTextColor(Color.rgb(0x00,0x92,0x3f));
                        textStatus.setText(msgconnected);

                        listViewPairedDevice.setVisibility(View.GONE);
                        inputPane.setVisibility(View.VISIBLE);
                    }
                });

                try {
                    mydUQx = new dUQx(bluetoothSocket.getInputStream(), bluetoothSocket.getOutputStream());
                    myThreaddUQx.start();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            } else {
                //fail
            }
        }

        public void cancel() {

            Toast.makeText(getApplicationContext(),
                    "close bluetoothSocket",
                    Toast.LENGTH_LONG).show();

            try {
                bluetoothSocket.close();
            } catch (IOException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }

        }

    }

    /*
    ThreadConnected:
    Background Thread to handle dUQx transfers
     */
    private class ThreaddUQx extends Thread {


        @Override
        public void run() {

            boolean lflag;
            long lloggingperiod;

            long nsuccessful=0,nunsuccesful=0;

            runOnUiThread(new Runnable() {

                @Override
                public void run() {
                    progressBar.setVisibility(View.VISIBLE);
                }
            });


            mydUQx.dUQx_CalibrateAnalog();

            mydUQx.dUQX_SetResolution((byte) 1);

            mydUQx.dUQx_WriteDigital(1023);

            runOnUiThread(new Runnable() {

                @Override
                public void run() {
                    progressBar.setVisibility(View.GONE);
                }
            });

            double rvalue;

            while (true) {

                ArrayList<Double> values = new ArrayList<Double>();
                ArrayList<Integer> fields = new ArrayList<Integer>();







                try {
                    flagSemaphore.acquire();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }

                lflag = flag;
                flagSemaphore.release();


                if (lflag) {


                    runOnUiThread(new Runnable() {

                        @Override
                        public void run() {
                            progressBar.setVisibility(View.VISIBLE);
                        }
                    });


                    thingSpeak.setChannel_id(Integer.valueOf(prefs.getString("channel_text","")));
                    thingSpeak.setWrite_key(prefs.getString("writekey_text",""));
                    thingSpeak.setRead_key(prefs.getString("readkey_text",""));

                    if(prefs.getBoolean("dporte",false)){

                        rvalue=(double)mydUQx.dUQx_ReadDigital();
                        values.add(rvalue);
                        fields.add(1);
                    }

                    if(prefs.getBoolean("a0e",false)){

                        rvalue=mydUQx.dUQx_ReadAnalogSingle(0);
                        values.add(rvalue);
                        fields.add(2);
                    }

                    if(prefs.getBoolean("a1e",false)){

                        rvalue=mydUQx.dUQx_ReadAnalogSingle(1);
                        values.add(rvalue);
                        fields.add(3);
                    }

                    if(prefs.getBoolean("a2e",false)){

                        rvalue=mydUQx.dUQx_ReadAnalogSingle(2);
                        values.add(rvalue);
                        fields.add(4);
                    }

                    if(prefs.getBoolean("a3e",false)){

                        rvalue=mydUQx.dUQx_ReadAnalogSingle(3);
                        values.add(rvalue);
                        fields.add(5);
                    }

                    if(prefs.getBoolean("a4e",false)){

                        rvalue=mydUQx.dUQx_ReadAnalogSingle(4);
                        values.add(rvalue);
                        fields.add(6);
                    }

                    if(prefs.getBoolean("a5e",false)){

                        rvalue=mydUQx.dUQx_ReadAnalogSingle(5);
                        values.add(rvalue);
                        fields.add(7);
                    }

                    Double[] avalues=new Double[values.size()];
                    Integer[] afields=new Integer[fields.size()];

                    values.toArray(avalues);
                    fields.toArray(afields);



                   if(thingSpeak.writeData(avalues,afields)==false){
                       nunsuccesful++;
                       runOnUiThread(new Runnable() {

                           @Override
                           public void run() {
                               Toast.makeText(MainActivity.this,"Problem Accessing Internet",Toast.LENGTH_LONG).show();
                           }
                       });

                   }else
                       nsuccessful++;











                    runOnUiThread(new Runnable() {

                        @Override
                        public void run() {
                            progressBar.setVisibility(View.GONE);
                        }
                    });


                    final String msg=Long.toString(nsuccessful)+" successful attempts out of "+Long.toString(nsuccessful+nunsuccesful);


                    runOnUiThread(new Runnable() {

                        @Override
                        public void run() {
                            textStatus.setTextColor(Color.rgb(0x00,0x92,0x3f));
                            textStatus.setText(msg);
                        }
                    });



                }

                try {
                    timeSemaphore.acquire();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                lloggingperiod=prefs.getInt("logging_period",15)*1000;
                timeSemaphore.release();


                try {
                    Thread.sleep(lloggingperiod);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }

        }

    }
}







