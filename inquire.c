#include <stdio.h>
#include "ntcan.h"
#include <unistd.h>
/*
* This example demonstrates how the NTCAN-API can be used to open a handle,
* set a baudrate and transmitting a CAN frame.
* Finally all proper cleanup operations are performed
*/
int main()
{
  int net=0; /* logical net number (here: 0) */
  uint32_t mode=0; /* mode used for canOpen() */
  int32_t txqueuesize=8; /* size of transmit queue */
  int32_t rxqueuesize=8; /* size of receive queue */
  int32_t txtimeout=100; /* timeout for transmit operations in ms */
  int32_t rxtimeout=1000; /* timeout for receive operations in ms */
  NTCAN_HANDLE txhandle; /* can handle returned by canOpen() */
  NTCAN_RESULT retvalue; /* return values of NTCAN API calls */
  uint32_t baud=2; /* configured CAN baudrate (here: 500 kBit/s.) */
  CMSG cmsg[8]; /* can message buffer */
  int rtr=0; /* rtr bit */
  int i; /* loop counter */
  int32_t len; /* # of CAN messages */
/* ############################################################### */
  retvalue = canOpen(net,
                     mode,
                     txqueuesize,
                     rxqueuesize,
                     txtimeout,
                     rxtimeout,
                     &txhandle);

  if (retvalue != NTCAN_SUCCESS)
  {
    printf("canOpen() failed with error %d!\n", retvalue);
    return(-1);
  }

  printf("function canOpen() returned OK !\n");
/* ############################################################### */
  retvalue = canSetBaudrate(txhandle, baud);

  if (retvalue != 0)
  {
    printf("canSetBaudrate() failed with error %d!\n", retvalue);
    canClose(txhandle);
    return(-1);
  }

  printf("function canSetBaudrate() returned OK !\n");


/* ############################################################### */
  uint32_t parg;
  retvalue = canIoctl(txhandle, NTCAN_IOCTL_GET_TX_TIMEOUT, &parg);
  printf("tx timeout is set to %d!\n", parg);

  NTCAN_INFO info;
  retvalue = canIoctl(txhandle, NTCAN_IOCTL_GET_INFO , &info);
  printf("nt lib ver is v%x!\n", info.dll);
  printf("board id is %s!\n", info.boardid);

  uint32_t current_baud;
  retvalue = canGetBaudrate(txhandle, &current_baud);
  printf("the baud rate currently set in the handle is %d!\n", current_baud);

/* ############################################################### */
  retvalue = canClose (txhandle);
  if (retvalue != NTCAN_SUCCESS)
    printf("canClose failed with error %d!\n", retvalue);
  else
    printf("canClose() returned OK !\n");
/* ############################################################### */
  return(0);
}
