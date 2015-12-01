package woods.androiddisckioskapp;


import android.annotation.TargetApi;
import android.app.Activity;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.view.Menu;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.RadioButton;
import android.widget.RelativeLayout;

import java.io.*;
import java.net.*;



public class KioskControlFullscreenActivity extends Activity
{
    private static final int MAXSIZE = 255;

    private RelativeLayout screen_controls1, screen_controls2;
    private Button burn_disc, return_disc, panic;
    private String burn_buffer;
    private String return_buffer = "RETURN 0";
    private String panic_buffer = "PANIC 0";
    private boolean burn_operation = true;
    private boolean return_operation = false;

    //Socket declarations
    private static final String SERVER_IP = "192.168.0.110";
    private static final int port_number = 2015;
    private Socket echoSocket;



    private void burn_disc_button_Click()   //Method for burn disc button
    {
        //Read the "burn_buffer."  Send message on socket to burn disc

        Intent intent = new Intent("android.intent.action.BURN");
        intent.putExtra("burn_notice", burn_buffer);
        burn_operation = true;
        return_operation = false;
        new Thread(new ClientThread()).start();

        startActivity(intent);
    }

    private void return_disc_button_Click() //Method for return disc button
    {
        //Read the "return_buffer."  Send message on socket to return disc

        Intent intent = new Intent("android.intent.action.RETURN");
        intent.putExtra("return_notice", return_buffer);
        burn_operation = false;
        return_operation = true;
        new Thread(new ClientThread()).start();

        startActivity(intent);

    }

    private void panic_button_Click()   //Method for panic button
    {
        //Tell the client program to "panic" for debugging

        Intent intent = new Intent("android.intent.action.PANIC");
        intent.putExtra("panic_notice", panic_buffer);
        burn_operation = false;
        return_operation = false;
        new Thread(new ClientThread()).start();

        startActivity(intent);
    }



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_kiosk_control_fullscreen);


        screen_controls1 = (RelativeLayout) findViewById(R.id.fullscreen_content_controls1);
        screen_controls2 = (RelativeLayout) findViewById(R.id.confirm_controls1);

        burn_disc = (Button) findViewById(R.id.burn_disc_button);
        return_disc = (Button) findViewById(R.id.return_disc_button);
        panic = (Button) findViewById(R.id.panic_button);



        burn_disc.setOnClickListener(new View.OnClickListener() /*Instructions for burn_disc button press*/ {
            @Override
            public void onClick(View view) {
            switch (view.getId()) {
                case R.id.burn_disc_button:
                    burn_disc_button_Click();
                    break;
            }
            }
        });

        return_disc.setOnClickListener(new View.OnClickListener()   /*Instructions for return_disc button press*/ {
            @Override
            public void onClick(View view) {
                switch (view.getId()) {
                    case R.id.return_disc_button:
                        return_disc_button_Click();
                        break;
                }
            }
        });


        panic.setOnClickListener(new View.OnClickListener()     {
            @Override
            public void onClick(View view){
                switch (view.getId()) {
                    case R.id.panic_button:
                        panic_button_Click();
                        break;
                }
            }
        });
    }

    class ClientThread implements Runnable{

        @Override
        public void run() {

            //Establishing socket; this is a blocking process that must be performed in a thread
            try
            {
                InetAddress server_address = null;
                server_address = InetAddress.getByName(SERVER_IP);
                echoSocket = new Socket(server_address, port_number);

                OutputStream out_buffer = echoSocket.getOutputStream();
                PrintStream printStream = new PrintStream(out_buffer);

                if(burn_operation == true)
                    printStream.print(burn_buffer);
                else if(return_operation == true)
                    printStream.print(return_buffer);
                else
                    printStream.print(panic_buffer);
                //printStream.close();

            } catch (UnknownHostException e) {
                e.printStackTrace();
                //System.exit(1);
            } catch (IOException e) {
                e.printStackTrace();
                //System.exit(1);
            }

            try
            {
                echoSocket.close();
            } catch (UnknownHostException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    public void onRadioButtonClicked(View view)
    {
        boolean button_check = ((RadioButton) view).isChecked();    /*Checks for button check*/

        switch(view.getId())
        {
            case R.id.radioButton0:
                //Place image ID 0 in "burn_buffer"
                burn_buffer = "BURN 0";
                break;

            case R.id.radioButton1:
                //Place image ID 1 in "burn_buffer"
                burn_buffer = "BURN 1";
                break;

            case R.id.radioButton2:
                //Place image ID 2 in "burn_buffer"
                burn_buffer = "BURN 2";
                break;

            case R.id.radioButton3:
                //Place image ID 3 in "burn_buffer"
                burn_buffer = "BURN 3";
                break;

            case R.id.radioButton4:
                //Place image ID 4 in "burn_buffer"
                burn_buffer = "BURN 4";
                break;

            case R.id.radioButton5:
                //Place image ID 5 in "burn_buffer"
                burn_buffer = "BURN 5";
                break;

            case R.id.radioButton6:
                //Place image ID 6 in "burn_buffer"
                burn_buffer = "BURN 6";
                break;

            case R.id.radioButton7:
                //Place image ID 7 in "burn_buffer"
                burn_buffer = "BURN 7";
                break;

            case R.id.radioButton8:
                //Place image ID 8 in "burn_buffer"
                burn_buffer = "BURN 8";
                break;

            case R.id.radioButton9:
                //Place image ID 9 in "burn_buffer"
                burn_buffer = "BURN 9";
                break;

            case R.id.radioButton10:
                //Place image ID 10 in "burn_buffer"
                burn_buffer = "BURN 10";
                break;

            case R.id.radioButton11:
                //Place image ID 11 in "burn_buffer"
                burn_buffer = "BURN 11";
                break;

            case R.id.radioButton12:
                //Place image ID 12 in "burn_buffer"
                burn_buffer = "BURN 12";
                break;

            case R.id.radioButton13:
                //Place image ID 13 in "burn_buffer"
                burn_buffer = "BURN 13";
                break;

            case R.id.radioButton14:
                //Place image ID 14 in "burn_buffer"
                burn_buffer = "BURN 14";
                break;

            case R.id.radioButton15:
                //Place image ID 15 in "burn_buffer"
                burn_buffer = "BURN 15";
                break;

            case R.id.radioButton16:
                //Place image ID 16 in "burn_buffer"
                burn_buffer = "BURN 16";
                break;

            case R.id.radioButton17:
                //Place image ID 17 in "burn_buffer"
                burn_buffer = "BURN 17";
                break;

            case R.id.radioButton18:
                //Place image ID 18 in "burn_buffer"
                burn_buffer = "BURN 18";
                break;

            case R.id.radioButton19:
                //Place image ID 19 in "burn_buffer"
                burn_buffer = "BURN 19";
                break;

            case R.id.radioButton20:
                //Place image ID 20 in "burn_buffer"
                burn_buffer = "BURN 20";
                break;

            case R.id.radioButton21:
                //Place image ID 21 in "burn_buffer"
                burn_buffer = "BURN 21";
                break;

            case R.id.radioButton22:
                //Place image ID 22 in "burn_buffer"
                burn_buffer = "BURN 22";
                break;

            case R.id.radioButton23:
                //Place image ID 23 in "burn_buffer"
                burn_buffer = "BURN 23";
                break;

            case R.id.radioButton24:
                //Place DEMO in "burn_buffer"
                burn_buffer = "DEMO 0";
                break;

        }
    }


}
