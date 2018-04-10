#include <stdio.h>
#include "ntcan.h"
#include <unistd.h>

/*
* This example demonstrates how the NTCAN-API can be used, 
* the application is to control CAN-CBX-DIO8, set points 1~4 as input,
* and set 5~8 as output. When input 1 is detected, set output to 5. 
* 1->5, 2->6, 3->7, 4->8.
*/
int main()
{
  int net=0; /* logical net number (here: 0) */
  uint32_t mode=0; /* mode bits for canOpen */
  int32_t txqueuesize=8; /* maximum number of messages to transmit */
  int32_t rxqueuesize=8; /* maximum number of messages to receive */
  int32_t txtimeout=100; /* timeout for transmit in ms */
  int32_t rxtimeout=10000; /* timeout for receiving data in ms */

  NTCAN_HANDLE rxhandle; /* can handle returned by canOpen() */
  NTCAN_RESULT rxret; /* return values of NTCAN API calls */

  NTCAN_HANDLE txhandle; /* can handle returned by canOpen() */
  NTCAN_RESULT txret; /* return values of NTCAN API calls */

  uint32_t baud=2; /* configured CAN baudrate (here: 500 kBit/s.) */
  CMSG rxcmsg[8]; /* buffer for can messages */
  CMSG txcmsg[8];

  int i; /* loop counter */
  int32_t rxlen; /* Buffer size in # of messages for canRead() */
  int32_t txlen;
  uint32_t id; /* add ids */
/* ############################################################### */
  rxret = canOpen(net,
                  mode,
                  txqueuesize,
                  rxqueuesize,
                  txtimeout,
                  rxtimeout,
                  &rxhandle);
  if (rxret != NTCAN_SUCCESS)
  {
    printf("rx canOpen() failed with error in rxret %d!\n", rxret);
    return(-1);
  }
  printf("rx function canOpen() returned OK !\n");
/* ############################### ################################ */
  rxret = canSetBaudrate(rxhandle, baud);
  if (rxret != 0)
  {
    printf("rx canSetBaudrate() failed with error %d!\n", rxret);
    canClose(rxhandle);
    return(-1);
  }
  printf("rx function canSetBaudrate() returned OK !\n");
/* ############################################################### */
  rxret = canIdAdd(rxhandle, 0x186); /* Enable CAN-ID 0 */
  if (rxret != NTCAN_SUCCESS)
  {
    printf("rx canIdAdd() failed with error %d!\n", rxret);
    canClose(rxhandle);
    return(-1);
  }
  
  printf("rx function canIdAdd() returned OK !\n");


/* ############################################################### */
  txret = canOpen(net,
                  mode,
                  txqueuesize,
                  rxqueuesize,
                  txtimeout,
                  rxtimeout,
                  &txhandle);
  if (txret != NTCAN_SUCCESS)
  {
    printf("tx canOpen() failed with error in txret %d!\n", txret);
    return(-1);
  }
  printf("tx function canOpen() returned OK !\n");
/* ############################### ################################ */
  txret = canSetBaudrate(txhandle, baud);
  if (txret != 0)
  {
    printf("tx canSetBaudrate() failed with error %d!\n", txret);
    canClose(txhandle);
    return(-1);
  }
  printf("tx function canSetBaudrate() returned OK !\n");

/* ############################### ################################ */

//MNT Op
  txcmsg[0].id=0x00;
  txcmsg[0].len=0x02; 
  txcmsg[0].data[0]=0x01;
  txcmsg[0].data[1]=0x00;


//set io mask to all output
  txcmsg[1].id=0x606;
  txcmsg[1].len=0x08; 
  txcmsg[1].data[0]=0x2f;
  txcmsg[1].data[1]=0x50;
  txcmsg[1].data[2]=0x22;
  txcmsg[1].data[3]=0x01;  
  txcmsg[1].data[4]=0xf0;  
  txcmsg[1].data[5]=0x00;
  txcmsg[1].data[6]=0x00;
  txcmsg[1].data[7]=0x00;
 

  txlen=2; /* Number of valid messages in cmsg buffer*/
  txret = canWrite(txhandle, &txcmsg[0], &txlen, NULL);
 




/* ############################################################### */
  do {
/*
* Set max numbers of messages that can be returned with
* one canRead() call according to buffer size
*/
    rxlen = 8;
    rxret = canRead(rxhandle, &rxcmsg[0], &rxlen, NULL);
    if (rxret == NTCAN_RX_TIMEOUT)
    {
      printf("canRead() returned timeout\n");
      continue;
    }
    else if(rxret != NTCAN_SUCCESS)
    {
      printf("canRead() failed with error %d!\n", rxret);
    }
    else
    {
      printf("function canRead() returned OK !\n");

//      if (rxcmsg[0].id == 0x186 && (rxcmsg[0].len & 0x0f) == 1)
      if (rxcmsg[0].id == 0x186)
      {
        int rxdata = rxcmsg[0].data[0];
        printf("receive message with id 0x186! data is 0x%x \n", rxdata);

// sleep 1 seconds.
        usleep(1000000);

//send rxpdo
        txcmsg[0].id=0x206;
        txcmsg[0].len=0x01;
        txcmsg[0].data[0] = rxdata<<4;  

        txret = canWrite(txhandle, &txcmsg[0], &txlen, NULL);

      }

    }
//    break;
  } while(1);
/* ############################################################### */
  rxret = canIdDelete(rxhandle, 0);
  if (rxret != NTCAN_SUCCESS)
  printf("rx canIdDelete() failed with error %d!\n", rxret);
  printf("rx function canIdDelete() returned OK !\n");
/* ############################################################### */
  rxret = canClose(rxhandle);
  if (rxret != NTCAN_SUCCESS)
    printf("rx canClose() failed with error %d!\n", rxret);
  else
    printf("rx canClose() returned OK !\n");
/* ############################################################### */
  return(0);
}
